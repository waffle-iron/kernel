/*
 *      This file is part of the SmokeOS project.
 *  Copyright (C) 2015  <Fabien Bavent>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   - - - - - - - - - - - - - - -
 *
 *      Scheduling, tasks and cpu ressources managment.
 */
#include <smkos/kapi.h>
#include <smkos/kstruct/map.h>
#include <smkos/kstruct/task.h>
#include <smkos/sync.h>

/* SCHEDULER
  * The smoke OS scheduler is a customized round-robin based algorithm.
  * The round robin is build from a circular linked list that
  * contains all thread ready to be executed. A semaphore hold the number
  * of available thread, that each CPU may acquire.
  * Each CPU turn around the list and execute the first suitable task.
  * This algorithm in itself is really far from optimal, so we have several
  * system that intend to optimized it.
  * First, every tasks are weighted. Each task received a different quantum
  * which is equal to:
  *         quantum = latency * latencyFactor * weight / totalWeight
  * A task will gain weight if the task need all it's quantum to finish and
  * loose if IO bounded. Meaning that CPU bounded task will have large amount
  * of time to complete their task and other will have just what they need.
  * Second, if IO bounded task have shorter elapsed time to execute, they have
  * priority on other tasks.
  * @note To read or update schNext_ value we need to lock on schLock_.
  */


/* ----------------------------------------------------------------------- */
/** Thread a signal */
int sched_signal (int raise, size_t data, const char *at)
{
  int err;
  kProcess_t *process;

  if (kCPU.current_ == NULL) {
    kpanic ("Kernel trigger an exception ; signal: %d (%x) at %s.\n", raise, data, at);
  } else {
    process = kCPU.current_->process_;
    kprintf ("\033[31mProcess %d failed\033[0m: %d (%x) at %s.\n", kCPU.current_->process_->pid_, raise, data, at);
    kstacktrace(12);
    area_display(&process->mspace_);
    err = sched_stop (kSYS.scheduler_, kCPU.current_, SCHED_ZOMBIE);
    if (err == 0)
      process_exit(process, 0);
    sched_next(kSYS.scheduler_);
  }

  return EAGAIN;
}


/* ----------------------------------------------------------------------- */
/** Insert a new thread on the scheduler */
void sched_insert(kScheduler_t *sched, kThread_t *task)
{
  klock(&sched->lock_);

  assert (task->state_ < SCHED_READY);
  task->state_ = SCHED_READY;

  if (sched->anchor_ == NULL) {
    task->schNext_ = task;
    sched->anchor_ = task;
  } else {
    // klock(&sched->anchor_->schLock_);
    task->schNext_ = sched->anchor_->schNext_;
    sched->anchor_->schNext_ = task;
    // kunlock(&sched->anchor_->schLock_);
    sched->anchor_ = task;
  }

  semaphore_release (&sched->taskSem_, 1);
  kunlock(&sched->lock_);
  // We add a thread on the scheduler and increase totalWeight_;
}


/* ----------------------------------------------------------------------- */
void sched_remove(kScheduler_t *sched, kThread_t *thread)
{
  kThread_t *pick;
  klock(&sched->lock_);

  if (thread->state_ == SCHED_READY) {
    if (!semaphore_tryaquire(&sched->taskSem_, 1))
      kpanic("Thread have been stolen");

    thread->state_ = SCHED_ZOMBIE;
  }

  assert (thread->state_ < SCHED_READY);
  // assert (task->event_ == NULL);

  if (kCPU.current_ == thread)
    kCPU.current_ = NULL;

  if (sched->anchor_ == thread) {
    if (thread->schNext_ == thread)
      sched->anchor_ = NULL;
    else {
      pick = sched->anchor_;

      while (pick->schNext_ != thread)
        pick = pick->schNext_;

      pick->schNext_ = thread->schNext_;
      sched->anchor_ = pick;
    }

  } else {
    pick = sched->anchor_;

    while (pick->schNext_ != thread)
      pick = pick->schNext_;

    pick->schNext_ = thread->schNext_;
  }

  kunlock(&sched->lock_);
}


/* ----------------------------------------------------------------------- */
/** Change the status of the current executing task and save the current registers */
int sched_stop (kScheduler_t *sched, kThread_t *thread, int state)
{
  klock (&thread->process_->lock_);
  assert(thread == kCPU.current_);
  assert(thread->state_ == SCHED_EXEC ||
         (thread->state_ == SCHED_ABORT && state == SCHED_ZOMBIE));
  assert (state != SCHED_EXEC && state != SCHED_ABORT);

  if (state != SCHED_ZOMBIE)
    cpu_save_task (thread); // FIXME Save registers

  // if (thread->state_ == SCHED_ABORTING)
  //  thread->state_ = SCHED_RUNNING;

  kCPU.current_ = thread->schNext_;
  thread->state_ = state;

  if (state == SCHED_ZOMBIE) {

    sched_remove (sched, thread);
    atomic_dec (&thread->process_->runningTask_);

    /// @todo And all signal have been sended...
    if (thread->process_->runningTask_ == 0) {
      kCPU.current_ = NULL;
      destroy_process (thread->process_);
      return -1;
    }

  } else if (state == SCHED_READY) {
    semaphore_release(&sched->taskSem_, 1);
  } else if (state == SCHED_BLOCKED) {
    sched_remove (sched, thread);
  }

  kunlock (&thread->process_->lock_);
  return 0;
}

/* ----------------------------------------------------------------------- */
void sched_next(kScheduler_t *sched)
{
  if (kCPU.current_ != NULL) {
    assert(kCPU.current_->state_ >= SCHED_READY && kCPU.current_->state_ != SCHED_ABORT);

    // @todo stay linked to an external task might be risky
    if (kCPU.current_->state_ >= SCHED_EXEC) {
      // cpu_save_task (kCPU.current_);
      semaphore_release (&sched->taskSem_, 1);
      kCPU.current_->state_ = SCHED_READY;
      kCPU.current_ = kCPU.current_->schNext_;
    }

  } else {
    kCPU.current_ = sched->anchor_;
  }

  if (semaphore_tryaquire(&sched->taskSem_, 1)) {
    if (kCPU.current_ != NULL)
      cpu_run_task(kCPU.current_);

    kpanic("Error on scheduler\n");
  } else {
    cpu_halt();
  }

  kprintf("I got an error here.\n");
}


