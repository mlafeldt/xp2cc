#ifndef _MAIN_H_
#define _MAIN_H_

#define MODE_DEFAULT	0 // Compile P2M file using ASCII
#define MODE_EXTRACT	1
#define MODE_BIN	2
#define MODE_ANSI	4
#define MODE_SORT	8

#define BIN_MIN_LEN	22
#define BIN_MKR_EOF	(u32)0xFFFFFFFF

#define P2M_GAMENAME	"XP2CC"
#define P2M_ROOTNAME	"FCD"
#define P2M_FILENAME	"cheats"

int mode;

#endif // _MAIN_H_
