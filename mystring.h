#ifndef _MYSTRING_H_
#define _MYSTRING_H_

/* Includes */

#include "mytypes.h"

/* Defines */

#define NUL	0x00

// White-Space Characters
#define TAB	0x09	// Horizontal tab
#define LF	0x0A	// Line feed, New line
#define CR	0x0D	// Carriage return
#define SPACE	0x20

/* Prototypes */

char *addStr(char *s1, char *s2);
u32 indexOfChr(char *s, char c);
u32 lastIndexOfChr(char *s, char c);
int mkPrintStr(char *s, char c);
void remCmtFromStr(char *s);
int setMaxStrLen(char *s, u32 max);
int trimStr(char *s);

int isAsciiChr(char c);
int isCmtStr(char *s);
int isEmptyStr(char *s);
int _isEmptyStr(char *s, u32 count);
int isPrintStr(char *s);

#endif // _MYSTRING_H_
