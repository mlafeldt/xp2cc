#include <stdio.h>

#include "chksum.h"
#include "extract.h"
#include "firestr.h"
#include "main.h"
#include "myutil.h"
#include "p2m.h"

int isValidBinFile(u8 *bin, u32 binlen)
{
	return (binlen >= BIN_MIN_LEN &&
		*(u32*) &bin[0] == binlen - 8 &&
		*(u32*) &bin[4] == get_crc32(bin + 8, binlen - 8) &&
		*(u32*) &bin[binlen - 4] == BIN_MKR_EOF
	);
}

int extractCodes(u8 *bin, u32 binlen, char *filename)
{
	FILE *fp = NULL;
	u8 *binend = NULL;
	u16 ngames = 0, ncheats, ncodes;
	u32 nallcheats = 0, nallcodes = 0;

	printf("BIN file integrity check... ");

	if (!isValidBinFile(bin, binlen)) {
		fprintf(stderr, "Failed.\n");
		return -1;
	}

	printf("Passed.\nExtracting to TXT file '%s'... ", filename);

	if ((fp = fopen(filename, "wt")) == NULL) {
		fprintf(stderr, "Failed.\n");
		return -2;
	}

	fprintf(fp, "\n//--------\n\n");

	binend = bin + binlen - 4;
	bin += 8;

	while (bin < binend) {
		// Print game title w/ quotes
		fprintf(fp, "\"%s\"\n", toSBCS((FIRECHAR*) bin));
		bin += getFireStrSize((FIRECHAR*) bin);

		ncheats = *(u16*) bin;
		bin += sizeof(u16);
		nallcheats += ncheats;

		while (ncheats--) {
			// Print cheat description w/o quotes
			fprintf(fp, "%s\n", toSBCS((FIRECHAR*) bin));
			bin += getFireStrSize((FIRECHAR*) bin);

			ncodes = *(u16*) bin;
			bin += sizeof(u16);
			nallcodes += ncodes;

			while (ncodes--) {
				// Print cheat code
				fprintf(fp, "%08X %08X\n", *(u32*) bin, *(u32*) &bin[4]);
				bin += 8;
			}
		}

		fprintf(fp, "\n//--------\n\n");
		ngames++;
	}

	fprintf(fp, "// Total Games..: %u\n", ngames);
	fprintf(fp, "// Total Cheats.: %u\n", nallcheats);
	fprintf(fp, "// Total Codes..: %u\n", nallcodes);

	fclose(fp);

	printf("Done.\nStats: %u game(s), %u cheat(s), %u code(s)\n", ngames, nallcheats, nallcodes);

	return 0;
}

int extract(u8 *buf, u32 buflen, char *filename)
{
	if (!(mode & MODE_BIN)) {
		printf("P2M file integrity check... ");

		if (!isValidP2mFile(buf, buflen)) {
			fprintf(stderr, "Failed.\n");
			return -1;
		}

		printf("Passed.\nSearching for BIN file '%s'... ", P2M_FILENAME);
		unscramble(buf);
		buf = findP2mDirFile(buf, P2M_ROOTNAME, P2M_FILENAME, &buflen);

		if (buf == NULL) {
			fprintf(stderr, "Not found.\n");
			return -2;
		}

		printf("Found (%s).\n", sizeToStr(buflen));
	}

	return extractCodes(buf, buflen, filename);
}
