#include <stdlib.h>
#include <string.h>

#include "firestr.h"
#include "main.h"


FIRECHAR *toFireStr(char *s)
{
	FIRECHAR *fs = NULL;
	u32 i, sz;

	i = strlen(s);
	sz = (i + 1) * sizeof(FIRECHAR);
	fs = (FIRECHAR*) malloc(sz);

	if (fs != NULL) {
		memset(fs, 0, sz);
		while (i--) fs[i].ch = s[i];
	}

	return fs;
}

char *toSBCS(FIRECHAR *fs) // SBCS = Single-byte-character string
{
	char *s = NULL;
	u32 i, sz;

	i = getFireStrLen(fs);
	sz = (i + 1) * sizeof(char);
	s = (char*) malloc(sz);

	if (s != NULL) {
		memset(s, 0, sz);
		while (i--) s[i] = fs[i].ch;
	}

	return s;
}

u32 getFireStrLen(FIRECHAR *fs)
{
	u32 len = 0;

	while (fs++->ch) len++;

	return len;
}

u32 getFireStrSize(FIRECHAR *fs)
{
	return ((getFireStrLen(fs) + 1) * sizeof(FIRECHAR));
}

int putFireStr(FIRECHAR *fs, FILE *stream)
{
	u32 count = getFireStrLen(fs) + 1;

	return (fwrite(fs, sizeof(FIRECHAR), count, stream) == count);
}
