#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

int main(int argc, char** argv){
	FILE* in = NULL;
	FILE* out = NULL;

	if (argc > 1) in = fopen(argv[1], "r");
	if (argc > 2) out = fopen(argv[2], "wb");

	in = in ? in : stdin;
	out = out ? out : stdout;

	while (!feof(in) && !ferror(in)) {
		char* line = NULL;
		size_t n = 0;
		getline(&line, &n, in);

		//get mnemonic and operand
		char mnemonic[10] = "";
		int operand = 0;

		sscanf(line, "%s %i", mnemonic, &operand);

		free(line);

		if (*mnemonic == ';' || !*mnemonic) continue;

		if (strcmp(mnemonic, ".db") == 0) {
			//.db nnnn
			fputc(operand >> 8, out);
			fputc(operand & 0xFF, out);
			continue;
		}
	
		for (int i = 0; i <= strlen(mnemonic); i++) mnemonic[i] = toupper(mnemonic[i]);

		for (int i = 0; i < OPCODE_COUNT; i++) {
			if (strcmp(opcodes_strings[i], mnemonic) == 0) {
				//eqals
				fputc((uint8_t) i, out);
				fputc((uint8_t) operand, out);
				continue;
			}
		}

		fprintf(stderr, "INVALID MNEMONIC '%s'\n", mnemonic);
	}

	fclose(in);
	fclose(out);
}
