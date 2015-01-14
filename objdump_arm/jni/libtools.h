/*
** libtools.h
** 
** Made by zhangwei@cmmobi.com
**
** on 2014-4-23
** 
*/

#ifndef _LIBTOOLS_H_
# define _LIBTOOLS_H_

#include <elf.h>


int xaccess(char *filename);
int xopen(char *filename);
void *xmmap(int fd, char * filename, unsigned int size);

void objdp32_format(void *data, char *filename);
void objdp64_format(void *data, char *filename);

typedef struct{
     char* data;
     int len;
}SectionData;

#endif /* !_LIBTOOLS_H_ */
