/*	Filename.....: myutil.c
 *	Version......: 1.6
 *	Last updated.: 2004/08/31
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myutil.h"

char *sizeToStr(u32 size)
{
	char *s = (char*) calloc(20, sizeof(char));

	if (size < 1024) sprintf(s, "%u Byte", size); // Byte
	else if (size < (1024 * 1024)) sprintf(s, "%.2f KB", (float)size / 1024); // KB
	else sprintf(s, "%.2f MB", (float)size / (1024 * 1024)); // MB

	return s;
}

void sanityChecks(void)
{
    u32 val = 0x12345678;
    int err = 0;

    if (*(u8*) &val != 0x78) {
	printf("Error: This program requires Little Endian byte ordering!\n");
	err++;
    }

    if (sizeof(u8) != 1) {
	printf("Error: sizeof(u8) != 1\n");
	err++;
    }

    if (sizeof(u16) != 2) {
	printf("Error: sizeof(u16) != 2\n");
	err++;
    }

    if (sizeof(u32) != 4) {
	printf("Error: sizeof(u32) != 4\n");
	err++;
    }

    if (err) {
	    printf("Bad sanity, program terminated.\n");
	    exit(EXIT_FAILURE);
    }
}

long getBinFileLen(FILE *stream)
{
	long len;
	long oldfp;

	if (stream == NULL) return -1;

	oldfp = ftell(stream);
	fseek(stream, 0, SEEK_END);
	len = ftell(stream);
	fseek(stream, oldfp, SEEK_SET);

	return len;
}

long getTxtFileLen(FILE *stream)
{
	long len = 0;

	if (stream == NULL) return -1;

	fseek(stream, 0, SEEK_SET);
	while (getc(stream) != EOF) len++;

	return len;
}

void *readFromFile(FILE *fp, long offset, long size)
{
	void *block = NULL;

	if ((fp == NULL) || (offset < 0) || (size <= 0)) return NULL;

	if ((block = malloc(size)) == NULL) return NULL;

	if (fseek(fp, offset, SEEK_SET)) return NULL;

	if (!fread(block, size, 1, fp)) return NULL;

	return block;
}

int writeToFile(FILE *fp, void *block, long offset, long size)
{
	if (fp == NULL) return -1;

	if (block == NULL) return -2;

	if (offset < 0) return -3;

	if (size <= 0) return -4;

	if (fseek(fp, offset, SEEK_SET)) return -5;

	if (!fwrite(block, size, 1, fp)) return -6;

	return 0;
}

int put_u16(u16 val, FILE *stream)
{
	return fwrite(&val, sizeof(u16), 1, stream);
}

int put_u32(u32 val, FILE *stream)
{
	return fwrite(&val, sizeof(u32), 1, stream);
}

u8 *addBuf(u8 **dest, u32 *destlen, u8 *src, u32 srclen)
{
	u32 tmp = *destlen;

	*destlen += srclen;
	*dest = (u8*) realloc(*dest, *destlen);
	if (*dest == NULL) return NULL;
	memmove(*dest + tmp, src, srclen);

	return *dest;
}

u8 *expandBuf(u8 **dest, u32 *destlen, int nbytes)
{
	*destlen += nbytes;
	*dest = (u8*) realloc(*dest, *destlen);
	if (*dest == NULL) return NULL;
	memset(*dest + *destlen - nbytes, 0, nbytes);

	return *dest;
}

void printErrMsg(const char *msg)
{
	if (msg != NULL) fprintf(stderr, "Error: %s\n", msg);

	fprintf(stderr, "Abnormal program termination\n");

	exit(EXIT_FAILURE);
}
