/*
** objdump.c for objdump in /home/catuss_a//Desktop/nmobjdump/objdump
** 
** Made by axel catusse
** Login   <catuss_a@epitech.net>
** 
** Started on  Sat Mar 12 16:52:54 2011 axel catusse
** Last update Sun Mar 13 22:28:35 2011 axel catusse
*/

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include "objdump.h"

static void my_objdump(char * filename){
  void            *data;
  Elf32_Ehdr        *elf;
  int            fd;
  long            size;

  if (xaccess(filename) == -1)
      return ;
  if ((fd = xopen(filename)) == -1)
      return ;
  if ((size = lseek(fd, 0, SEEK_END)) == -1){
      printf("error: lseek\n");
      return ;
  }

  if ((data = xmmap(fd, filename, size)) == MAP_FAILED)
      return ;

  elf = (Elf32_Ehdr*)data;
  
  if (elf->e_ident[EI_CLASS] == ELFCLASS32){
    objdp32_format(data, filename);
  }
  else if (elf->e_ident[EI_CLASS] == ELFCLASS64){
    objdp64_format(data, filename);
  }
  
  if (close(fd) == -1){
    printf("error: munmap\n" );
  }
  
  if (munmap(data, size) == -1){
    printf("error: munmap\n");
  }
}

int main(int ac, char **av)
{
  int i;

  if (ac > 1){
      i = 0;
      while (++i < ac){
        my_objdump(av[i]);
      }
  }
  else {
    my_objdump("./a.out");
  }

  return 1;
}
