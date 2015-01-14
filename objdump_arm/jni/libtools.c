/*
** libtools.c
** 
** Made by zhangwei@cmmobi.com
**
** on 2014-4-23
** 
*/

#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include "my_constant.h"
#include "libtools.h"

static char* TAG = "LIBTOOLS";

static int  check_elf32_format(Elf32_Ehdr *elf, char *path)
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

static int check_elf64_format(Elf64_Ehdr *elf, char *path)
{
  if (elf->e_ident[EI_MAG0] != ELFMAG0
      || elf->e_ident[EI_MAG1] != ELFMAG1
      || elf->e_ident[EI_MAG2] != ELFMAG2
      || elf->e_ident[EI_MAG3] != ELFMAG3
      || (elf->e_type != ET_EXEC && elf->e_type != ET_REL && elf->e_type != ET_DYN))
  {
     printf("error: nm: %s: File format not recognized\n", path);
     return (-1);
  }
  return (1);
}

static SectionData* dump_sect32(char *strtab, Elf32_Ehdr *elf, Elf32_Shdr *shdr, int i)
{
  LOGI(TAG, "dump_sect32  - In, i:%d" , i);
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

  LOGI(TAG, "dump_sect32  - Out");
  return ret;
}

static SectionData* dump_sect64(char *strtab, Elf64_Ehdr *elf, Elf64_Shdr *shdr, int i)
{
  LOGI(TAG, "dump_sect64  - In, i:%d" , i);
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


  LOGI(TAG, "dump_sect64  - Out");
  return ret;
}

static SectionData*  dump_section32(char *strtab, Elf32_Ehdr *elf, Elf32_Shdr *shdr,  char* section_name)
{
  LOGI(TAG, "dump_section32  - In, section_name:%s" , section_name);
  int   i;

  i = -1;
  while (++i < elf->e_shnum){
      if (shdr[i].sh_size
            && !strcmp(&strtab[shdr[i].sh_name], section_name))
      {
         SectionData* ret =  dump_sect32(strtab, elf, shdr, i);
         LOGI(TAG, "dump_section32  - Out, SectionData len:%d" , ret->len);
         return  ret;
      }
  }

  LOGI(TAG, "dump_section32  - Out, SectionData:null");
  return NULL;
}

static SectionData*  dump_section64(char *strtab, Elf64_Ehdr *elf, Elf64_Shdr *shdr,  char* section_name)
{
  LOGI(TAG, "dump_section64  - In, section_name:%s" , section_name);
  int   i;

  i = -1;
  while (++i < elf->e_shnum){
      if (shdr[i].sh_size
            && !strcmp(&strtab[shdr[i].sh_name], section_name))
      {
         SectionData* ret =  dump_sect64(strtab, elf, shdr, i);
         LOGI(TAG, "dump_section64  - Out, SectionData len:%d" , ret->len);
         return  ret;
      }
  }

  LOGI(TAG, "dump_section64  - Out, SectionData:null");
  return NULL;
}

char* lookup(SectionData* sd, char *funcname){
        if(sd==NULL  || funcname==NULL  || sd->data==NULL || sd->len<=0){
               return NULL;
        }

       LOGI(TAG, "lookup - In, filename:%s" , funcname);


       int start = 0;
       int end = 0;
       int index = 0;

       char* foundFunc = NULL;


       int len = strlen(funcname);
       int correctIndex = 0;
       int i;
       int status = 0;//0 matching  1 matched
       for(i=0; i<sd->len; i++){
              char curChar = (sd->data)[i];
              char correctChar = funcname[correctIndex];
              LOGI(TAG,"lookup  - for loop - i:%d, start:%d, curChar: %0x, correctChar:%0x, correctIndex:%d, len:%d",  
                                 i, start,
                                 curChar, correctChar,
                                 correctIndex,  len);

#if 0
             if(status==0){
                  if(curChar==correctChar){//include last  \0
                        correctIndex++;
                  }else if(curChar==0){
                        correctIndex=0;
                        start=i;
                  }

                  if(correctIndex>=len){//"abc" -->  0, 1, 2, 3 
                         status = 1; //matched
                  }
             }else{
                  if(curChar==0){
                         end=i;  //point to the last \0
                         break;
                  }
             }
#else
                  if(curChar==correctChar){//include last  \0
                        correctIndex++;
                  }else if(curChar==0){
                        correctIndex=0;
                        start=i;
                  }

                  if(correctIndex>len){//"abc" -->  0, 1, 2, 3 
                         status = 1; //matched
                         end=i;  //point to the last \0
                         break;
                  }
#endif


             
       }

       if(end>start){
              //found, the string is from (start, end]
              foundFunc = (char*)malloc(end-start);
               for(i=start+1; i<=end; i++){
                     foundFunc[i-start-1] =  (sd->data)[i];
               }

              LOGI(TAG, "lookup - Out, foundFunc:%s" , foundFunc);

              return foundFunc;
        }

        LOGI(TAG, "lookup - Out, foundFunc:null" );
        return NULL;


}

char* objdp32_getFuncName(void *data, char *filename, char *funcname)
{
  LOGI(TAG, "objdp32_getFuncName - In, filename:%s, funcname:%s", filename , funcname);
  Elf32_Ehdr  *elf;
  Elf32_Shdr  *shdr;
  char   *strtab;

  elf = (Elf32_Ehdr*)data;
  if (check_elf32_format(elf, filename) == -1)
    return NULL;

  shdr = (Elf32_Shdr*)(data + elf->e_shoff);
  strtab = (char*)(data + shdr[elf->e_shstrndx].sh_offset);
  
  SectionData* sd =  dump_section32(strtab, elf, shdr, ".dynstr");

  char* ret =  lookup(sd,  funcname);

  if(sd->data!=NULL){
         free(sd->data);
         sd->data = NULL;
   }

  if(sd!=NULL){
      free(sd);
      sd = NULL;
  }

  LOGI(TAG, "objdp64_getFuncName - Out");

  return ret;
}

char* objdp64_getFuncName(void *data, char *filename, char *funcname)
{
  LOGI(TAG, "objdp64_getFuncName - In, filename:%s, funcname:%s", filename , funcname);
  Elf64_Ehdr  *elf;
  Elf64_Shdr  *shdr;
  char   *strtab;

  elf = (Elf64_Ehdr*)data;
  if (check_elf64_format(elf, filename) == -1)
    return NULL;

  shdr = (Elf64_Shdr*)(data + elf->e_shoff);
  strtab = (char*)(data + shdr[elf->e_shstrndx].sh_offset);
  
  SectionData* sd =  dump_section64(strtab, elf, shdr, ".dynstr");

  char* ret =  lookup(sd,  funcname);

  if(sd->data!=NULL){
         free(sd->data);
         sd->data = NULL;
   }

  if(sd!=NULL){
      free(sd);
      sd = NULL;
  }

  LOGI(TAG, "objdp64_getFuncName - Out");
  
  return ret;
}

static char*  my_objdump(char *filename, char* funcname)
{
  LOGI(TAG, "my_objdump - In, filename:%s, funcname:%s", filename , funcname);
  void            *data;
  Elf32_Ehdr        *elf;
  int            fd;
  long            size;
  char* ret;

  if (xaccess(filename) == -1)
      return NULL;
  if ((fd = xopen(filename)) == -1)
      return NULL;
  if ((size = lseek(fd, 0, SEEK_END)) == -1){
      printf("error: lseek\n");
      return NULL;
  }

  if ((data = xmmap(fd, filename, size)) == MAP_FAILED)
      return NULL;

  elf = (Elf32_Ehdr*)data;
  
  if (elf->e_ident[EI_CLASS] == ELFCLASS32){
    ret = objdp32_getFuncName(data, filename, funcname);
  }
  else if (elf->e_ident[EI_CLASS] == ELFCLASS64){
    ret = objdp64_getFuncName(data, filename, funcname);
  }
  
  if (close(fd) == -1){
    printf("error: munmap\n" );
  }
  
  if (munmap(data, size) == -1){
    printf("error: munmap\n");
  }

  LOGI(TAG, "my_objdump - Out, ret:%s" , ret);
  return ret;
}

int main(int ac, char **av)
{
  int i;
  LOGI(TAG, "main - In");
  if (ac > 2){
        printf("av[1]:%s,  av[2]:%s\n", av[1], av[2]);
        char* funcname = my_objdump(av[1], av[2]);
        printf("%s\n", funcname);
  }
  else {
    printf("error: no enough argvs\n");
  }

  return 1;
}
