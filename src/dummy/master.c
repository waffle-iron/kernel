#include "lib.c"


int main ()
{
  const char * logs[] = {
     "master log...         \n",
     "nothing happened!     \n",
     "gosh I'm bored!       \n",
     "call me MASTER!       \n",
     "waiting for something?\n",
     "I'll better continue. \n",
  };

  write (1, "\e[53,38mKernel master\e[0m] v1.0\n", 32);
  int sc = open ("master.log", 0x43, 0644); // O_WRONLY | O_CREAT
  if (sc != 3) {
    write (1, "The first file open on master is not 3!\n", 40);
  } 

  if (sc == 0) {
    write (1, "The first file open on master is zero!!\n", 40);
  }
  int s1 = open ("/dev/fb0", 0x02, 0644); // O_WRONLY

  int j=0;

  execv_s ("DEAMON.", NULL);


  for (;;) {
    
    // _delay();
    sleep (1);

    write (1, "\e[31mMaster\e[0m - still here\n", 29);

    write (sc, logs[j++ % 6], 23);

  }



  // printf ("Kernel master v1.0\n");
  // chdir ("/mnt/OS_CORE/");

  // open (".syr");
  // read (".syr");
  // exec ("enguig")
  return 0;
}
