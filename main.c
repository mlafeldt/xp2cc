#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "compile.h"
#include "extract.h"
#include "main.h"
#include "mystring.h"
#include "myutil.h"

#define XP2CC_VERS	"1.31"

#define MAX_TEXT_SIZE	2*1024*1024


void showBanner(void)
{
	printf("\nXploder2 Code Compiler v%s\n", XP2CC_VERS);
	printf("Copyright (C) 2004 misfire\n");
//	printf("For XFX internal use only!\n");
	printf("\n");
}

void showUsage(void)
{
	printf("Usage:\txp2cc.exe <input file> <output file> [-absx]\n\n");
	printf("\t-x Extract cheat codes\n");
	printf("\t-b Use binary file instead of P2M\n");
	printf("\t-a Set system-default ANSI code page\n");
	printf("\t-s Sort by game title\n");
	printf("\nNote:\t-a and -s affect compiling only.\n");
}

int mode = MODE_DEFAULT;

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	void *buf = NULL;
	long filelen;
	int i;
	char *s = NULL;

#ifdef _DEBUG
	clock_t t = clock();
#endif

	showBanner();
	sanityChecks();

	if (argc > 2) {
		for (i = 3; i < argc; i++) {
			s = argv[i];
			if (*s == '-') {
				while (*++s) switch (tolower((u8)*s)) {
					case 'a':
						mode |= MODE_ANSI;
						break;
					case 'b':
						mode |= MODE_BIN;
						break;
					case 's':
						mode |= MODE_SORT;
						break;
					case 'x':
						mode |= MODE_EXTRACT;
						break;
				}
			}
		}
	} else {
		showUsage();
		return -1;
	}

	if (mode & MODE_EXTRACT) {
		printf("Mode: Extract cheat codes from %s file\n", mode & MODE_BIN ? "BIN" : "P2M");
		printf("Reading from %s file '%s'... ", mode & MODE_BIN ? "BIN" : "P2M", argv[1]);

		if ((fp = fopen(argv[1], "rb")) == NULL) {
			fprintf(stderr, "Failed.\n");
			return -2;
		}

		filelen = getBinFileLen(fp);

		if ((buf = readFromFile(fp, 0, filelen)) == NULL) {
			fprintf(stderr, "Failed.\n");
			return -3;
		}

		fclose(fp);
		printf("Done (%s).\n", sizeToStr(filelen));
		printf("Extraction %s.\n", !extract((u8*)buf, filelen, argv[2]) ? "succeeded" : "failed");
	} else {
		printf("Mode: Compile %s file\n", mode & MODE_BIN ? "BIN" : "P2M");

		if (mode & MODE_ANSI) {
			printf("Setting system-default ANSI code page... ");
			printf("%s.\n", setlocale(LC_ALL, ".ACP") == NULL ? "Failed, using ASCII" : "Done");
		}

		printf("Reading from TXT file '%s'... ", argv[1]);

		if ((fp = fopen(argv[1], "rt")) == NULL) {
			fprintf(stderr, "Failed.\n");
			return -2;
		}

		filelen = getTxtFileLen(fp);
//		if (filelen > MAX_TEXT_SIZE) filelen = MAX_TEXT_SIZE;

		if ((buf = readFromFile(fp, 0, filelen)) == NULL) {
			fprintf(stderr, "Failed.\n");
			return -3;
		}

		fclose(fp);
		printf("Done.\n");
		printf("Compilation %s.\n", !compile((char*)buf, filelen, argv[2]) ? "succeeded" : "failed");
	}

	free(buf);

#ifdef _DEBUG
	printf("*** All done in %.f ms ***\n", (double)(clock() - t) / CLOCKS_PER_SEC * 1000);
#endif

	return 0;
}
