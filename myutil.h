#ifndef _MYUTIL_H_
#define _MYUTIL_H_

/* Includes */

#include <stdio.h>
#include "mytypes.h"

/* Prototypes */

char *sizeToStr(u32 size);

void sanityChecks(void);

long getBinFileLen(FILE *stream);
long getTxtFileLen(FILE *stream);

void *readFromFile(FILE *fp, long offset, long size);
int writeToFile(FILE *fp, void *block, long offset, long size);

int put_u16(u16 val, FILE *stream);
int put_u32(u32 val, FILE *stream);

#endif // _MYUTIL_H_
