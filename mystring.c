/*	Filename.....: mystring.c
 *	Version......: 1.4
 *	Last updated.: 2004/08/10
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "mystring.h"


/*	char *addStr(char *s1, char *s2);
 *
 *	addStr appends s2 to s1 and returns a pointer to the new string.
 */
char *addStr(char *s1, char *s2)
{
	char *s = (char*) malloc(strlen(s1) + strlen(s2) + 1);

	if (s != NULL) {
		strcpy(s, s1);
		strcat(s, s2);
	}

	return s;
}

/*	u32 indexOfChr(char *s, char c);
 *
 *	indexOfChr returns the index within s of the first occurrence of the
 *	specified char c. If no such char occurs in s, then -1 is returned.
 */
u32 indexOfChr(char *s, char c)
{
	u32 i = 0;

	while (s[i] && s[i] != c) i++;

	if (s[i] == c) return i;

	return -1;
}

/*	u32 lastIndexOfChr(char *s, char c);
 *
 *	lastIndexOfChr returns the index within s of the last occurrence of the
 *	specified char c. If the char does not occur in s, then -1 is returned.
 */
u32 lastIndexOfChr(char *s, char c)
{
	u32 i = strlen(s);

	while (i && s[--i] != c);

	if (s[i] == c) return i;

	return -1;
}

/*	int mkPrintStr(char *s, char c);
 *
 *	mkPrintStr replaces non-printable chars in s with printable char c.
 */
int mkPrintStr(char *s, char c)
{
	int i = 0;

	while (*s) {
		if (!isprint((u8)*s)) {
			*s = c;
			i++;
		}
		s++;
	}

	return i;
}

/*	void remCmtFromStr(char *s);
 *
 *	remCmtFromStr removes a trailing comment (if present) from string s.
 */
void remCmtFromStr(char *s)
{
	// NUL-terminate string where comment begins
	while (*s) {
		if (isCmtStr(s)) {
			*s = NUL;
			break;
		}
		s++;
	}
}

/*	int setMaxStrLen(char *s, u32 max);
 *
 *	setMaxStrLen sets the maximum length of string s.
 *	If s is longer, it will be shortened to max chars.
 */
int setMaxStrLen(char *s, u32 max)
{
	if (strlen(s) <= max) return FALSE;

	s[max] = NUL;

	return TRUE;
}

/*	int trimStr(char *s);
 *
 *	trimStr removes white space from both ends of the string s.
 */
int trimStr(char *s)
{
	u32 first = 0;
	u32 last;
	u32 slen;
	char *t = s;

	// Return if string is empty
	if (isEmptyStr(s)) return -1;

	// Get first non-space char
	while (isspace((u8)*t++)) first++;

	// Get last non-space char
	last = strlen(s) - 1;
	t = &s[last];
	while (isspace((u8)*t--)) last--;

	// Kill leading/trailing spaces
	slen = last - first + 1;
	memmove(s, s + first, slen);
	s[slen] = NUL;

	return 0;
}




/*	int isAsciiChr(char c);
 *
 *	isAsciiChr returns TRUE if c is an ASCII character
 *	(in the range 0x00 - 0x7F).
 */
int isAsciiChr(char c)
{
	return (c >= 0 && c <= 127);
}

/*	int isCmtStr(char *s);
 *
 *	isCmtStr returns TRUE if s indicates a comment.
 */
int isCmtStr(char *s)
{
	return (strlen(s) >= 2 && !strncmp(s, "//", 2)) || (*s == ';');
}

/*	int isEmptyStr(char *s);
 *
 *	isEmptyStr returns TRUE if s contains no printable chars other than
 *	white space.
 */
int isEmptyStr(char *s)
{
	u32 slen = strlen(s);

	while (slen--) {
		if (isgraph((u8)*s++)) return FALSE;
	}

	return TRUE;
}

/*	int _isEmptyStr(char *s, u32 count);
 *
 *	_isEmptyStr returns TRUE if the first count chars of s are not printable
 *	(apart from white space).
 */
int _isEmptyStr(char *s, u32 count)
{
	while (count--) {
		if (isgraph((u8)*s++)) return FALSE;
	}

	return TRUE;
}

/*	int isPrintStr(char *s);
 *
 *	isPrintStr returns TRUE if all chars of s are printable.
 */
int isPrintStr(char *s)
{
	while (*s) {
		if (!isprint((u8)*s++)) return FALSE;
	}

	return TRUE;
}
