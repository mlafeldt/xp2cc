// Based on P2M Tool v1.2

/* Includes */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "myutil.h"
#include "p2m.h"

/* Defines */

#define P2M_DIR_VERS		"P2V2"

#define OFFSET_DELTA		68

#define	NUM_ATTRS		16

#define ATT_NONE		0x0000
#define ATT_READABLE		0x0001
#define ATT_WRITABLE		0x0002
#define ATT_EXECUTABLE		0x0004
#define ATT_COPYPROTECT		0x0008
#define ATT_0010		0x0010
#define ATT_SUBFOLDER		0x0020
#define ATT_0040		0x0040
#define ATT_PRESERVED		0x0080
#define ATT_0100		0x0100
#define ATT_0200		0x0200
#define ATT_0400		0x0400
#define ATT_PDA			0x0800
#define ATT_PS1DATA		0x1000
#define ATT_2000		0x2000
#define ATT_HIDDEN		0x4000
#define ATT_8000		0x8000

#define P2M_DIR_ATTRS		ATT_READABLE | ATT_WRITABLE | ATT_EXECUTABLE | ATT_SUBFOLDER | ATT_0400 | ATT_8000
#define P2M_DIRFILE_ATTRS	ATT_READABLE | ATT_WRITABLE | ATT_EXECUTABLE | ATT_0010 | ATT_0400 | ATT_8000

/* Function Prototypes */

int isValidP2mFile(u8 *p2m, u32 p2mlen);
void scramble(P2M_DIR *dir);
void unscramble(u8 *p2m);
u32 getCrc(u8 *blk, u32 len);

P2M_DIR *newP2mDir(char *gamename, char *rootname);
int addP2mDirFile(P2M_DIR *dir, char *filename, u8 *bin, u32 binlen);
u8 *findP2mDirFile(u8 *p2m, char *rootname, char *filename, u32 *binlen);
void writeP2mDir(P2M_DIR *dir, FILE *fp);
void freeP2mDir(P2M_DIR *dir);

/* Function Declarations */

int isValidP2mFile(u8 *p2m, u32 p2mlen)
{
	return (p2mlen > 4 + sizeof(P2M_DIR_HDR) + sizeof(P2M_DIRFILE_HDR) &&
		!strncmp(p2m, P2M_FILE_ID, strlen(P2M_FILE_ID))
	);
}

void scramble(P2M_DIR *dir)
{
	u32 i;
	u32 offset = sizeof(P2M_DIR_HDR) + (dir->dirhdr->filenum * sizeof(P2M_DIRFILE_HDR)) - OFFSET_DELTA;

	for (i = 0; i < dir->dirhdr->filenum; i++) {
		dir->filehdr[i].offset = offset;
		offset += dir->filehdr[i].size;
	}

	dir->dirhdr->filenum++;
}

void unscramble(u8 *p2m)
{
	u32 fileoff = 4;
	P2M_DIR_HDR *dirhdr = (P2M_DIR_HDR*) &p2m[fileoff];
	P2M_DIRFILE_HDR *filehdr = NULL;
	u32 i;

	while (dirhdr->totalsize) {
		if (i = --dirhdr->filenum) { // Fix number of files
			filehdr = (P2M_DIRFILE_HDR*) &p2m[fileoff + sizeof(P2M_DIR_HDR)];
			while (i--) filehdr++->offset += OFFSET_DELTA; // Fix offset
		}
		fileoff += dirhdr->totalsize + 4;
		dirhdr = (P2M_DIR_HDR*) &p2m[fileoff];
	}
}

u32 getCrc(u8 *blk, u32 len)
{
	u32 crc = 0;
	int i;

	while (len--) {
		crc ^= *blk++ << 24;
		for (i = 8; i; i--) {
			if (crc & 0x80000000) crc = (crc << 1) ^ 0x04C11DB7;
			else crc <<= 1;
		}
	}

	return crc;
}

P2M_DIR *newP2mDir(char *gamename, char *rootname)
{
	P2M_DIR *dir = (P2M_DIR*) malloc(sizeof(P2M_DIR));
	P2M_DATE date = { 0, 0, 19, 1, 27, 12, 1983 }; // My birthday ;)
	time_t timer;
	struct tm *lt = NULL;

	if (dir == NULL) return NULL;

	memset(dir, 0, sizeof(P2M_DIR));

	dir->dirhdr = (P2M_DIR_HDR*) malloc(sizeof(P2M_DIR_HDR));
	if (dir->dirhdr == NULL) return NULL;

	// Initialize dir header
	memset(dir->dirhdr, 0, sizeof(P2M_DIR_HDR));
	memset(dir->dirhdr->ffbytes, 0xFF, sizeof(dir->dirhdr->ffbytes));

	strcpy(dir->dirhdr->gamename, gamename);
	strcpy(dir->dirhdr->rootname, rootname);
	strcpy(dir->dirhdr->version, P2M_DIR_VERS);

	memcpy(&dir->dirhdr->created, &date, sizeof(P2M_DATE));
	time(&timer);
	lt = localtime(&timer);
	date.sec  = lt->tm_sec;
	date.min  = lt->tm_min;
	date.hour = lt->tm_hour;
	date.mday = lt->tm_mday;
	date.mon  = lt->tm_mon + 1;
	date.year = lt->tm_year + 1900;
	memcpy(&dir->dirhdr->modified, &date, sizeof(P2M_DATE));

	dir->dirhdr->totalsize = sizeof(P2M_DIR_HDR) - 4;
	dir->dirhdr->attrs = P2M_DIR_ATTRS;

	return dir;
}

int addP2mDirFile(P2M_DIR *dir, char *filename, u8 *bin, u32 binlen)
{
	P2M_DIRFILE_HDR *last;

	// Update dir header
	dir->dirhdr->totalsize += sizeof(P2M_DIRFILE_HDR) + binlen;
	dir->dirhdr->datasize += binlen;
	dir->dirhdr->filenum++;

	// Add file header
	dir->filehdr = (P2M_DIRFILE_HDR*) realloc(dir->filehdr, dir->dirhdr->filenum * sizeof(P2M_DIRFILE_HDR));
	if (dir->filehdr == NULL) return -1;

	last = &dir->filehdr[dir->dirhdr->filenum - 1];
	memset(last, 0, sizeof(P2M_DIRFILE_HDR));

	strcpy(last->filename, filename);

	memcpy(&last->created, &dir->dirhdr->created, sizeof(P2M_DATE));
	memcpy(&last->modified, &dir->dirhdr->modified, sizeof(P2M_DATE));

	last->size  = binlen;
	last->size2 = binlen;
	last->crc   = getCrc(bin, binlen);
	last->attrs = P2M_DIRFILE_ATTRS;

	// Add data
	dir->data = (u8*) realloc(dir->data, dir->dirhdr->datasize);
	if (dir->data == NULL) return -2;

	memcpy(&dir->data[dir->dirhdr->datasize - binlen], bin, binlen);

	return 0;
}

u8 *findP2mDirFile(u8 *p2m, char *rootname, char *filename, u32 *binlen)
{
	u32 fileoff = 4;
	P2M_DIR_HDR *dirhdr = (P2M_DIR_HDR*) &p2m[fileoff];
	P2M_DIRFILE_HDR *filehdr = NULL;
	u32 i;

	while (dirhdr->totalsize) {
		if (!strcmp(dirhdr->rootname, rootname) && (i = dirhdr->filenum)) {
			filehdr = (P2M_DIRFILE_HDR*) &p2m[fileoff + sizeof(P2M_DIR_HDR)];
			while (i--) {
				if (!strcmp(filehdr->filename, filename)) {
					// File found
					*binlen = filehdr->size;
					return &p2m[filehdr->offset + fileoff];
				}
				filehdr++;
			}
		}
		fileoff += dirhdr->totalsize + 4;
		dirhdr = (P2M_DIR_HDR*) &p2m[fileoff];
	}

	// File not found
	*binlen = 0;
	return NULL;
}

void writeP2mDir(P2M_DIR *dir, FILE *fp)
{
	fwrite(dir->dirhdr, sizeof(P2M_DIR_HDR), 1, fp);
	fwrite(dir->filehdr, sizeof(P2M_DIRFILE_HDR), dir->dirhdr->filenum - 1, fp);
	fwrite(dir->data, dir->dirhdr->datasize, 1, fp);
}

void freeP2mDir(P2M_DIR *dir)
{
	free(dir->dirhdr);
	free(dir->filehdr);
	free(dir->data);
	free(dir);
}
