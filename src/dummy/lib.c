#include <smoke/syscall.h>



int strlen(const char* str)
{
  int i =0;
  while(*(str++)) ++i;
  return i;
}

void _puts (const char* str) 
{
  write(1, str, strlen(str));
}

void _delay ()
{
  int volatile k = 0xdeadbeaf;
  int volatile i = 0x8000000;
  while (--i > 0) {
    k  = (k | (i >> 3)) * 0x75f;
  }
}

time_t time(time_t* now)
{
  return (time_t)SYSCALL1(SYS_TIME, now);
}


int sleep (int seconds) 
{
  return SYSCALL1(SYS_SLEEP, seconds * 1000);
}

int execv_s(const char *path, const char * args)
{
  sStartInfo_t param = {args, NULL, 0, 0, 0, 0, 0, 0};
  return exec (path, &param);
}




