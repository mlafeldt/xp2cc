/* Includes */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chksum.h"
#include "compile.h"
#include "firestr.h"
#include "icon.h"
#include "main.h"
#include "mystring.h"
#include "myutil.h"
#include "p2m.h"

/* Defines */

#define MAX_STRING_LENGTH	38

#define MAX_CHEATS_PER_GAME	65535
#define MAX_CODES_PER_CHEAT	65535

#define NUM_DIGITS_CHEAT_CODE	16
#define NUM_BYTES_CHEAT_CODE	8

#define TOK_GAME_TITLE		1
#define TOK_CHEAT_DESC		2
#define TOK_CHEAT_CODE		4
#define TOK_ALL			7 // TOK_GAME_TITLE | TOK_CHEAT_DESC | TOK_CHEAT_CODE

/* Typedefs */

typedef struct {
	char	s[MAX_STRING_LENGTH + 1]; // Cheat Description
	u16	ncodes;
	u32	*codes; // Cheat Codes
} CHEAT;

typedef struct {
	char	s[MAX_STRING_LENGTH + 1]; // Game Title
	u16	ncheats;
	CHEAT	*cheats;
} GAME;

typedef struct {
	u16	ngames;
	GAME	*games;
	u32	nallcheats;
	u32	nallcodes;
} AST; // Abstract Syntax Tree

/* Global Variables */

int nline = 1;
int nerr = 0;
int nwarn = 0;

/* Function Declarations */

void checkStr(char *s)
{
	int i;

	if (setMaxStrLen(s, MAX_STRING_LENGTH)) {
		trimStr(s);
		fprintf(stderr, "WARNING(%i): String shortened to '%s'\n", nline, s);
		nwarn++;
	}

	if (i = mkPrintStr(s, '?')) {
		fprintf(stderr, "WARNING(%i): %i bad char(s) replaced: '%s'\n", nline, i, s);
		nwarn++;
	}
}

int isGameTitle(char *s)
{
	u32 slen = strlen(s);

	return ((slen >= 2) && (*s == '"') && (s[slen - 1] == '"'));
}

int isCheatCode(char *s)
{
	u32 i = 0;

	while (*s) {
		if (isxdigit((u8)*s)) i++;
		else if (!isspace((u8)*s)) return FALSE;
		s++;
	}

	return (i == NUM_DIGITS_CHEAT_CODE);
}

int getToken(char *s)
{
	if (isGameTitle(s)) return TOK_GAME_TITLE;

	if (isCheatCode(s)) return TOK_CHEAT_CODE;

	return TOK_CHEAT_DESC;
}

char *getGameTitle(char *s)
{
//	if (!isGameTitle(s)) return NULL;

	s[strlen(s) - 1] = NUL;

	return ++s;
}

u32 *getCheatCode(char *s)
{
	char digits[NUM_DIGITS_CHEAT_CODE];
	static u32 code[2];
	u32 i = 0;

//	if (!isCheatCode(s)) return NULL;

	while (*s) {
		if (isxdigit((u8)*s)) digits[i++] = *s;
		s++;
	}

	sscanf(digits, "%08X%08X", &code[0], &code[1]);

	return code;
}

int addGameToTree(AST *tree, char *s)
{
	tree->games = (GAME*) realloc(tree->games, ++tree->ngames * sizeof(GAME));
	if (tree->games == NULL) return -1;

	strcpy(tree->games[tree->ngames - 1].s, s);
	tree->games[tree->ngames - 1].ncheats = 0;
	tree->games[tree->ngames - 1].cheats = NULL;

	return 0;
}

int addCheatToTree(AST *tree, char *s)
{
	GAME *lastgame = &tree->games[tree->ngames - 1];

	if (lastgame->ncheats == MAX_CHEATS_PER_GAME) return -1;

	tree->nallcheats++;
	lastgame->ncheats++;

	lastgame->cheats = (CHEAT*) realloc(lastgame->cheats, lastgame->ncheats * sizeof(CHEAT));
	if (lastgame->cheats == NULL) return -2;

	strcpy(lastgame->cheats[lastgame->ncheats - 1].s, s);
	lastgame->cheats[lastgame->ncheats - 1].ncodes = 0;
	lastgame->cheats[lastgame->ncheats - 1].codes = NULL;

	return 0;
}

int addCodeToTree(AST *tree, u32 *code)
{
	GAME *lastgame = &tree->games[tree->ngames - 1];
	CHEAT *lastcheat = &lastgame->cheats[lastgame->ncheats - 1];

	if (lastcheat->ncodes == MAX_CODES_PER_CHEAT) return -1;

	tree->nallcodes++;
	lastcheat->ncodes++;

	lastcheat->codes = (u32*) realloc(lastcheat->codes, lastcheat->ncodes * NUM_BYTES_CHEAT_CODE);
	if (lastcheat->codes == NULL) return -2;

	memcpy(&lastcheat->codes[2 * (lastcheat->ncodes - 1)], code, NUM_BYTES_CHEAT_CODE);

	return 0;
}

int compare(const void *arg1, const void *arg2)
{
	return strcoll(((GAME*)arg1)->s, ((GAME*)arg2)->s);
}

void sortTree(AST *tree)
{
	qsort(tree->games, tree->ngames, sizeof(GAME), compare);
}

u8 *translateTree(AST *tree)
{
	FILE *fp = NULL;
	u16 i, j, k;
	u8 *bin = NULL;
	u32 binlen;

	fp = tmpfile();
	if (fp == NULL) return NULL;

	put_u32(0, fp);
	put_u32(0, fp);

	for (i = 0; i < tree->ngames; i++) {
		putFireStr(toFireStr(tree->games[i].s), fp);
		put_u16(tree->games[i].ncheats, fp);

		for (j = 0; j < tree->games[i].ncheats; j++) {
			putFireStr(toFireStr(tree->games[i].cheats[j].s), fp);
			put_u16(tree->games[i].cheats[j].ncodes, fp);

			for (k = 0; k < tree->games[i].cheats[j].ncodes; k++) {
				put_u32(tree->games[i].cheats[j].codes[2 * k], fp);
				put_u32(tree->games[i].cheats[j].codes[(2 * k) + 1], fp);
			}
		}
	}

	put_u32(BIN_MKR_EOF, fp);

	binlen = ftell(fp);
	bin = (u8*) readFromFile(fp, 0, binlen);
	if (bin == NULL) return NULL;
	fclose(fp); // Delete temporary file

	*(u32*) &bin[0] = binlen - 8;
	*(u32*) &bin[4] = get_crc32(bin + 8, binlen - 8);

	return bin;
}

void freeTree(AST *tree)
{
	u16 i, k;

	i = tree->ngames;
	while (i--) {
		k = tree->games[i].ncheats;
		while (k--) free(tree->games[i].cheats[k].codes); // Free all codes per cheat
		free(tree->games[i].cheats); // Free all cheats per game
	}
	free(tree->games); // Free all games
	free(tree); // Deallocate tree
}

AST *frontend(char *txt, u32 txtlen)
{
	char *s = NULL;
	u32 *code = NULL;
	u32 i = 0;
	u32 slen;
	int tokcur, tokexp = TOK_GAME_TITLE;
	AST *tree = (AST*) malloc(sizeof(AST));

	memset(tree, 0, sizeof(AST));

	// Text MUST contain a new line char at the end
	if (txt[txtlen - 1] != '\n') {
		txt = (char*) realloc(txt, ++txtlen);
		txt[txtlen - 1] = '\n';
	}

	printf("*** Compiler front end ***\n");
	printf("Parsing...\n");

	/*** Scanner ***/
	while (i < txtlen) {
		slen = indexOfChr(&txt[i], '\n');

		if (!_isEmptyStr(&txt[i], slen)) {
			s = &txt[i];
			s[slen] = NUL;
			/*** Screener ***/
			remCmtFromStr(s);
			trimStr(s);

			if (strlen(s) > 0) {
				/*** Parser ***/
				tokcur = getToken(s);

				if (!(tokexp & tokcur)) {
					fprintf(stderr, "ERROR(%i): '%s' : ", nline, s);
					if (tokexp & TOK_CHEAT_DESC) {
						fprintf(stderr, "Cheat description expected\n");
						tokexp = TOK_ALL;
					} else /* if (tokexp & TOK_GAME_TITLE) */ {
						fprintf(stderr, "Game title expected\n");
						tokexp = TOK_GAME_TITLE | TOK_CHEAT_DESC;
					}
					nerr++;
				} else switch (tokcur) {
					case TOK_GAME_TITLE:
						s = getGameTitle(s);
						if (*s == NUL) {
							fprintf(stderr, "ERROR(%i): Empty game title\n", nline);
							nerr++;
						} else {
							checkStr(s);
							if (!nerr && addGameToTree(tree, s) < 0) {
								fprintf(stderr, "ERROR(%i): '%s' : Cannot add more games\n", nline, s);
								nerr++;
							}
						}
						tokexp = TOK_GAME_TITLE | TOK_CHEAT_DESC;
						break;

					case TOK_CHEAT_DESC:
						checkStr(s);
						if (!nerr && addCheatToTree(tree, s) < 0) {
							fprintf(stderr, "ERROR(%i): '%s' : Cannot add more cheats\n", nline, s);
							nerr++;
						}
						tokexp = TOK_ALL;
						break;

					case TOK_CHEAT_CODE:
						code = getCheatCode(s);
						if (!nerr && addCodeToTree(tree, code) < 0) {
							fprintf(stderr, "ERROR(%i): %08X %08X : Cannot add more codes\n", nline, code[0], code[1]);
							nerr++;
						}
						tokexp = TOK_ALL;
						break;
				}
			}
		}
		nline++;
		i += slen + 1;
	}

	printf("Parsing done: %i error(s), %i warning(s)\n", nerr, nwarn);

	if (nerr) {
		fprintf(stderr, "Error(s) occurred during parsing.\n");
		freeTree(tree);
		return NULL;
	} else if (!tree->ngames) {
		fprintf(stderr, "Nothing parsed.\n");
		freeTree(tree);
		return NULL;
	}

	printf("Stats: %u game(s), %u cheat(s), %u code(s)\n", tree->ngames, tree->nallcheats, tree->nallcodes);

	return tree;
}

int createP2mFile(FILE *fp, u8 *bin, u32 binlen, u32 *p2mlen)
{
	P2M_DIR *dir = newP2mDir(P2M_GAMENAME, P2M_ROOTNAME);

	if (dir == NULL) return -1;

	if (addP2mDirFile(dir, "icon.sys", icon_sys, sizeof(icon_sys)) < 0) return -2;
	if (addP2mDirFile(dir, "fcd.ico", fcd_ico, sizeof(fcd_ico)) < 0) return -2;
	if (addP2mDirFile(dir, P2M_FILENAME, bin, binlen) < 0) return -2;

	fwrite(P2M_FILE_ID, sizeof(char), strlen(P2M_FILE_ID), fp);
	scramble(dir);
	writeP2mDir(dir, fp);
	put_u32(0, fp); // No further dirs

	*p2mlen = dir->dirhdr->totalsize + 12;

	freeP2mDir(dir);

	return 0;
}

int backend(AST *tree, char *filename)
{
	FILE *fp = NULL;
	u8 *bin = NULL;
	u32 binlen;
	u32 p2mlen;

	printf("*** Compiler back end ***\n");

	if (mode & MODE_SORT) {
		printf("Sorting by game title... ");
		sortTree(tree);
		printf("Done.\n");
	}

	printf("Translating to BIN... ");

	if ((bin = translateTree(tree)) == NULL) {
		fprintf(stderr, "Failed.\n");
		return -1;
	}

	binlen = *(u32*) bin + 8;

	printf("Done (%s).\n", sizeToStr(binlen));
	printf("%s file '%s'... ", mode & MODE_BIN ? "Writing to" : "Creating P2M", filename);

	if ((fp = fopen(filename, "wb")) == NULL) {
		fprintf(stderr, "Failed.\n");
		return -2;
	}

	if ((mode & MODE_BIN ? writeToFile(fp, bin, 0, binlen) : createP2mFile(fp, bin, binlen, &p2mlen)) < 0) {
		fprintf(stderr, "Failed.\n");
		return -3;
	}

	free(bin);
	fclose(fp);

	if (mode & MODE_BIN) printf("Done.\n");
	else printf("Done (%s).\n", sizeToStr(p2mlen));

	return 0;
}

int compile(char *txt, u32 txtlen, char *filename)
{
	int ret = 0;
	AST *tree = frontend(txt, txtlen);

	if (tree == NULL) return -1;

	if (backend(tree, filename) < 0) ret = -2;

	freeTree(tree);

	return ret;
}
