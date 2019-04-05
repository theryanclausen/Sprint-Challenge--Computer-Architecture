#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

/**
 * Main
 */
int main(int argc, char **argv)
{
  struct cpu cpu;
  char *path;

  if(argc == 2)
  {
    path = argv[1];
  }
  else
  {
    printf("where is your path?\n");
    exit(1);
  }
  

  cpu_init(&cpu);
  cpu_load(&cpu, path);
  cpu_run(&cpu);

  return 0;
}