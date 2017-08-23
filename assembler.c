#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"

struct label {
	char label[32];
	uint8_t addr;
};

#define fputc(c,s) do {fprintf(stderr, "WRITE %X\n", c); putc(c,s);}while(0)

#define MAX_LABELS 64

struct label labels[MAX_LABELS];

int main(int argc, char** argv){
	FILE* in = NULL;
	FILE* out = NULL;

	if (argc > 1) in = fopen(argv[1], "r");
	if (argc > 2) out = fopen(argv[2], "wb");

	in = in ? in : stdin;
	out = out ? out : stdout;

	uint8_t addr = 0;
	uint8_t label_count = 0;

	//first loop, TO get labels
	while (!feof(in) && !ferror(in)) {
		char* line = NULL;
		size_t n = 0;
		getline(&line, &n, in);

		if (*line == ';' || *line == '\n' || !*line) {
			free(line);
			continue;
		}
		
		char label[32];
		char ignore[32];

		if (sscanf(line, "%[^:]%1[:] %s", label, ignore, ignore) >= 3) {
			//labvel; + nmemonic foudn
			fprintf(stderr, "NEW label '%s' at %X\n", label, addr);
			strcpy(labels[label_count].label, label);
			labels[label_count].addr = addr;
			
			label_count++;
			addr += 2;

		} else if (sscanf(line, "%1[^;]%s", ignore, ignore) >= 2) addr += 2;

		free(line);
	}

	rewind(in);

	while (!feof(in) && !ferror(in)) {
		char* line = NULL;
		size_t n = 0;
		getline(&line, &n, in);

		if (*line == ';' || *line == '\n' || !*line) {
			free(line);
			goto success;
		}

		//get mnemonic and operand
		char mnemonic[10] = "";
		int operand = 0;
		char label[32] = "";

		//remove label, delclaration if there is
		char* colon;
		colon = strchr(line, ':');
		line = colon ? colon+1 : line;

		//remove spaces;
		while (*line++ == ' ');
		line--;

		fprintf(stderr, "%s", line);
		
		if (sscanf(line, "%s %i", mnemonic, &operand) < 2)    //FIRST try to scan: mnemonic nn (diretc number)
			if (sscanf(line, "%s %%%s", mnemonic, label) < 2) //IF not : mnemonic %label
				sscanf(line, "%s", mnemonic);                 //FINALLY: maybe has no operand
				
		if (*label) {
			//now get label ADDRESS if there was
			for (int i = 0; i < MAX_LABELS; i++) {
				if (strcmp(labels[i].label, label) == 0) {
					operand = labels[i].addr;
				}
			}
		}

		if (strcmp(mnemonic, ".db") == 0) {
			//.db nnnn
			fputc(operand >> 8, out);
			fputc(operand & 0xFF, out);
			goto success;
		}
	
		for (int i = 0; i <= strlen(mnemonic); i++) mnemonic[i] = toupper(mnemonic[i]);

		for (int i = 0; i < OPCODE_COUNT; i++) {
			if (strcmp(opcodes_strings[i], mnemonic) == 0) {
				//eqals
				fputc((uint8_t) i, out);
				fputc((uint8_t) operand, out);
				goto success;
			}
		}

		fprintf(stderr, "INVALID MNEMONIC '%s'\n", mnemonic);
		return 1;

		success:;
	}

	fclose(in);
	fclose(out);
}
