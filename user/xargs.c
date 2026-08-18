#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
  char buf[512];
  char *xargv[MAXARG];
  int i;

  if (argc < 2) {
    fprintf(2, "usage: xargs command ...\n");
    exit(1);
  }

  // Copy the command and its original arguments into xargv.
  for (i = 1; i < argc; i++)
    xargv[i - 1] = argv[i];

  // Read stdin one character at a time until a newline.
  i = 0;
  while (read(0, &buf[i], 1) == 1) {
    if (buf[i] == '\n') {
      buf[i] = 0;
      xargv[argc - 1] = buf;   // append the whole line as one argument
      xargv[argc] = 0;         // NULL-terminate the argument vector

      if (fork() == 0) {
        exec(xargv[0], xargv);
        fprintf(2, "xargs: exec %s failed\n", xargv[0]);
        exit(1);
      }
      wait(0);
      i = 0;
    } else {
      i++;
    }
  }
  exit(0);
}
