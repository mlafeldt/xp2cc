// Based on P2M Tool v1.2

#ifndef _P2M_H_
#define _P2M_H_

/* Includes */

#include "mytypes.h"

/* Defines */

#define P2M_FILE_ID	"P2MS"

/* Typedefs */

typedef struct {
	u8		pad;		// Always 0
	u8		sec;		// Seconds after minute (0 - 59)
	u8		min;		// Minutes after hour (0 - 59)
	u8		hour;		// Hours after midnight (0 - 23)
	u8		mday;		// Day of month (1 - 31)
	u8		mon;		// Month (1 - 12)
	u16		year;		// Year
} P2M_DATE;

typedef struct {
	u32		totalsize;	// Total size of dir (size of headers + size of files - 4)
	char		gamename[36];	// Name of game (nul-terminated)
	u8		pada[20];	// Padding
	char		version[8];	// Version (nul-terminated?)
	u32		filenum;	// Number of files in dir (+1)
	u32		datasize;	// Total size of all files in dir
	P2M_DATE	created;	// Date of creation
	P2M_DATE	modified;	// Date of modification
	u8		padb[4];	// Padding
	u16		attrs;		// Attributes
	u8		padc[10];	// Padding
	char		rootname[32];	// Root/ID (nul-terminated)
	u8		ffbytes[8];	// Block filled with byte 0xFF
	u8		padd[56];	// Padding
} P2M_DIR_HDR;

typedef struct {
	P2M_DATE	created;	// Date of creation
	P2M_DATE	modified;	// Date of modification
	u32		size2;		// cf. size
	u16		attrs;		// Attributes
	u8		pada[10];	// Padding
	char		filename[32];	// Name of file (nul-terminated)
	u32		offset;		// Offset of data (-68)
	u32		size;		// Length of file
	u32		crc;		// CRC
	u8		padb[52];	// Padding
} P2M_DIRFILE_HDR;

typedef struct {
	P2M_DIR_HDR	*dirhdr;
	P2M_DIRFILE_HDR *filehdr;
	u8		*data;
} P2M_DIR;

/* Function Prototypes */

int isValidP2mFile(u8 *p2m, u32 p2mlen);
void scramble(P2M_DIR *dir);
void unscramble(u8 *p2m);

P2M_DIR *newP2mDir(char *gamename, char *rootname);
int addP2mDirFile(P2M_DIR *dir, char *filename, u8 *bin, u32 binlen);
u8 *findP2mDirFile(u8 *p2m, char *rootname, char *filename, u32 *binlen);
void writeP2mDir(P2M_DIR *dir, FILE *fp);
void freeP2mDir(P2M_DIR *dir);

#endif // _P2M_H_
