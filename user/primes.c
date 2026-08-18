#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Each call reads the first number (a prime), prints it, forks the next
// sieve stage, and forwards every number not divisible by that prime.
__attribute__((noreturn)) void
sieve(int read_fd)
{
  int prime;
  int n;
  int fd[2];
  int pid;

  if (read(read_fd, &prime, sizeof(prime)) != sizeof(prime)) {
    close(read_fd);
    exit(0);
  }
  printf("prime %d\n", prime);

  pipe(fd);
  pid = fork();
  if (pid == 0) {
    close(read_fd);
    close(fd[1]);
    sieve(fd[0]);
    exit(0);
  }

  close(fd[0]);
  while (read(read_fd, &n, sizeof(n)) == sizeof(n)) {
    if (n % prime != 0)
      write(fd[1], &n, sizeof(n));
  }
  close(read_fd);
  close(fd[1]);
  wait(0);
  exit(0);
}

int
main(int argc, char *argv[])
{
  int fd[2];
  int pid;
  int i;

  pipe(fd);
  pid = fork();
  if (pid == 0) {
    close(fd[1]);
    sieve(fd[0]);
    exit(0);
  }

  close(fd[0]);
  for (i = 2; i <= 280; i++)
    write(fd[1], &i, sizeof(i));
  close(fd[1]);
  wait(0);
  exit(0);
}
