#ifndef _FIRESTR_H_
#define _FIRESTR_H_

#include <stdio.h>
#include "mytypes.h"

typedef	struct {
	char	ch;
	char	pad[3];	// always 0
} FIRECHAR;

FIRECHAR *toFireStr(char *s);
char *toSBCS(FIRECHAR *fs);
u32 getFireStrLen(FIRECHAR *fs);
u32 getFireStrSize(FIRECHAR *fs);
int putFireStr(FIRECHAR *fs, FILE *stream);

#endif // _FIRESTR_H_
