/*
** objdp32_format.c for obj in /home/catuss_a//Desktop/nmobjdump
** 
** Made by axel catusse
** Login   <catuss_a@epitech.net>
** 
** Started on  Sun Mar 13 22:01:04 2011 axel catusse
** Last update Sun Mar 13 22:34:17 2011 axel catusse
*/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "objdump.h"

static int  check_elf_format(Elf32_Ehdr *elf, char *path)
{
  if (elf->e_ident[EI_MAG0] != ELFMAG0
      || elf->e_ident[EI_MAG1] != ELFMAG1
      || elf->e_ident[EI_MAG2] != ELFMAG2
      || elf->e_ident[EI_MAG3] != ELFMAG3
      || (elf->e_type != ET_EXEC && elf->e_type != ET_REL && elf->e_type != ET_DYN)) {
         printf("error: nm: %s: File format not recognized\n", path);
         return (-1);
  }

  return (1);
}

static void  print_ascii(unsigned int j, unsigned int *z, unsigned char *w, char *t)
{
  int   x;

  x = j;
  while (x % 16 != 0){
      if (x % 4 == 0 && x % 16){
         printf(" ");
      }
      printf("  ");
      ++x;
  }
  
  printf("  ");
  
  while (*z < j){
      if (isprint(w[*z])){
         printf("%c", t[*z]);
      }else{
         printf(".");
      }
      (*z)++;
  }

  while ((*z)++ % 16 != 0){
     printf(" ");
  }

  *z = j;

  printf("\n");
}

static void  print_sect(char *strtab, Elf32_Ehdr *elf, Elf32_Shdr *shdr, int i)
{
  unsigned char  *w;
  unsigned int  j;
  int   addr;
  unsigned int  z;

  addr = shdr[i].sh_addr;

  printf("Contents of section %s:\n", &strtab[shdr[i].sh_name]);

  w = (unsigned char*)((char*)elf + shdr[i].sh_offset);
  j = 0;
  z = 0;

  while (j < shdr[i].sh_size){
      if (j % 16 == 0){ 
        printf(" %04x ", addr);
      }

      printf("%02x", w[j++]);

      if (j % 4 == 0 && j % 16 && j < shdr[i].sh_size){
         printf(" ");
      }

      if (j % 16 == 0){
         addr += 16;
      }

      if (j % 16 == 0 || j >= shdr[i].sh_size){
         print_ascii(j, &z, w, (char*)w);
      }
  }
}

static SectionData* dump_sect(char *strtab, Elf32_Ehdr *elf, Elf32_Shdr *shdr, int i)
{
  unsigned char  *w;
  unsigned int  j;
  int   addr;
  unsigned int  z;

  addr = shdr[i].sh_addr;

  //printf("Contents of section %s:\n", &strtab[shdr[i].sh_name]);

  w = (unsigned char*)((char*)elf + shdr[i].sh_offset);
  j = 0;
  z = 0;

 SectionData* ret = (SectionData*)malloc(sizeof(SectionData));
 ret->data = (char*)malloc(shdr[i].sh_size);
 ret->len = shdr[i].sh_size;
 
  int index=0;

  while (j < shdr[i].sh_size) {
       (ret->data)[index++] = w[j++];
  }


  return ret;
}

static void  print_section32(char *strtab, Elf32_Ehdr *elf, Elf32_Shdr *shdr)
{
  int   i;

  i = -1;
  while (++i < elf->e_shnum){
      if (shdr[i].sh_size
            && strcmp(&strtab[shdr[i].sh_name], ".strtab")
            && strcmp(&strtab[shdr[i].sh_name], ".symtab")
            && strcmp(&strtab[shdr[i].sh_name], ".shstrtab")
            && strcmp(&strtab[shdr[i].sh_name], ".bss"))
      {
          print_sect(strtab, elf, shdr, i);
      }
  }
}

static SectionData*  dump_section32(char *strtab, Elf32_Ehdr *elf, Elf32_Shdr *shdr,  char* section_name)
{
  int   i;

  i = -1;
  while (++i < elf->e_shnum){
      if (shdr[i].sh_size
            && !strcmp(&strtab[shdr[i].sh_name], section_name))
      {
         return  dump_sect(strtab, elf, shdr, i);
      }
  }

  return NULL;
}

void objdp32_format(void *data, char *filename)
{
  Elf32_Ehdr  *elf;
  Elf32_Shdr  *shdr;
  char   *strtab;

  elf = (Elf32_Ehdr*)data;
  if (check_elf_format(elf, filename) == -1)
    return ;

  shdr = (Elf32_Shdr*)(data + elf->e_shoff);
  strtab = (char*)(data + shdr[elf->e_shstrndx].sh_offset);
  printf("\n%s:%5cfile format\n", filename, ' ');
  printf("architecture: %s, flags 0x%08x:\n\n",
  elf->e_machine == 3 ? "i386" : "notf", elf->e_flags);
  printf("start address 0x%08x\n\n", elf->e_entry);
  print_section32(strtab, elf, shdr);
}

char* lookup(SectionData* sd, char *funcname){
        if(sd==NULL  || funcname==NULL  || sd->data==NULL || sd->len<=0){
               return NULL;
        }


       int start = 0;
       int end = 0;
       int index = 0;

       char* foundFunc = NULL;


       int len = strlen(funcname);
       int correctIndex = 0;
       for(int i=0; start<sd->len; i++){
              char curChar = (sd->data)[i];
              char correctChar = funcname[correctIndex];

              if(curChar==correctChar){//include last  \0
                    correctIndex++;
              }else if(curChar==0){
                    correctIndex=0;
                    start=i;
              }

              if(correctIndex>=len){//"abc" -->  0, 1, 2, 3 
                     end=i;  //point to the last \0
                     break; //found
              }
             
       }

       if(end>start){
              //found, the string is from (start, end]
              foundFunc = (char*)malloc(end-start);
               for(int i=start+1; i<=end; i++){
                     foundFunc[i-start-1] =  (sd->data)[i];
               }

              return foundFunc;
        }

        return NULL;


}

char* objdp32_getFuncName(void *data, char *filename, char *funcname)
{
  Elf32_Ehdr  *elf;
  Elf32_Shdr  *shdr;
  char   *strtab;

  elf = (Elf32_Ehdr*)data;
  if (check_elf_format(elf, filename) == -1)
    return NULL;

  shdr = (Elf32_Shdr*)(data + elf->e_shoff);
  strtab = (char*)(data + shdr[elf->e_shstrndx].sh_offset);
  
  SectionData* sd =  dump_section32(strtab, elf, shdr, funcname);

  char* ret =  lookup(sd,  funcname);

  if(sd->data!=NULL){
         free(sd->data);
         sd->data = NULL;
   }

  if(sd!=NULL){
      free(sd);
      sd = NULL;
  }
  
  return ret;
}

