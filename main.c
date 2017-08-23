#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include "common.h"

#ifdef YDEBUG
#define dprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dprintf(...) 1
#endif

uint8_t  BUS, A, B, PC, MAR, IP, RAM[0x100], RAM_addr;
uint16_t IR;  //lo: instr, hi: operand
uint16_t ALU; //hi: B, lo: A; result in lo

#define CYCLE_SPEED 5000

void reset(void);
void exec_microcode(int* mcodes, uint8_t count);

int main(int argc, char** argv) {
	assert(argc > 1);

	FILE* f = fopen(argv[1], "rb");
	assert(f);
	int c = 0, i = 0;
	while ((c = fgetc(f)) != EOF)
		RAM[i++] = c;
	fclose(f);

	reset();

	while (1) {
		//cycle
		dprintf("CYCLE\n");
		
		//transfer to IR instr; and operand asfast, as posisble,
		//leave operand in BUS and exec instr
		int cycler[] = {c0, m0|pcinc, i1|c0, irswp|m0, i1|pcinc, irswp, i0, p0|irswp, i0, ipexec};
		exec_microcode(cycler, 10);

		usleep(CYCLE_SPEED);
	}
	return 0;
}

void reset(void) {
	PC = 0;
	MAR = 0;
	RAM_addr = 0;
	ALU = 0;
	IP = 0;
	IR = 0;
}

void exec_microcode(int* mcodes, uint8_t count) {
	for (int i = 0; i < count; i++) {
		int mcode = mcodes[i];
		
		#define op(bit) if (mcode & (bit))
		#define alu(bit, o) op(bit) {\
			uint8_t n1 = ALU&0x00FF, n2 = (ALU&0xFF00) >> 8;\
			ALU = 0;\
			switch(*#o) {\
				case '+': if (n1+n2 > 0xFF) ALU = 0x100; break;\
				case '-': if (n2>n1) ALU = 0x100; if(n2 == n1) ALU |= 0x200; break;\
			}\
			ALU |= (n1 o n2) & 0xFF;\
		}
		#define wire(bit, src, dest) op(bit) dest = src
		
		//wires
		wire(c0, PC, MAR);
		wire(c1, BUS, PC);
		wire(c2, PC, BUS);
		wire(m0, MAR, RAM_addr);
		wire(m1, BUS, MAR);
		op(r0) RAM[RAM_addr] = BUS;
		op(r1) BUS = RAM[RAM_addr];
		wire(a0, A, BUS);
		wire(a1, BUS, A);
		op(a2) ALU = (ALU & 0xFF00) | A;
		wire(a3, ALU & 0xFF, A);
		wire(b0, BUS, B);
		op(b1) ALU = (ALU & 0x00FF) | (B<<8);
		op(o0) printf("%.2X\n",BUS);
		wire(i0, IR & 0xFF, BUS);
		op(i1) IR = (IR & 0xFF00) | RAM[RAM_addr];
		wire(p0, BUS, IP);
		//op
		op(nop);
		op(hlt) {printf("HALT\n");while(1);}
		op(irswp) IR = (IR >> 8) | (IR << 8);
		op(pcinc) PC++;
		op(ipexec) {
			//execute, opcode (THIS CAn: end in recursion, for infinte.. ((GIVen theres opcode WITH ipexec micro,instrunction)))
			#define status dprintf("\nBUS: %X;A: %X; B: %X; ALU: %X; MAR: %X; RAM: %X\n", BUS,A,B,ALU,MAR,RAM[RAM_addr]);

			status;
			dprintf("EXEC opcode %.2X (PC: %.2X): %s %.2X", IP, PC, opcodes_strings[IP], BUS);
			exec_microcode(opcodes[IP], 4);
			status;
		}
		op(carry) if (ALU & (1<<8)) BUS = A;
		op(zero) if (ALU & (1<<9)) BUS = A;
		//alu ops
		alu(add, +);
		alu(sub, -);
		alu(and, &);
		alu(or,  |);
		alu(xor, ^);
	}
}
