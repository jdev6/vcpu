#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#ifdef YDEBUG
#define dprintf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dprintf(...) 1
#endif

uint8_t  BUS, A, B, PC, MAR, IP, RAM[0x100], RAM_addr;
uint16_t IR;  //lo: instr, hi: operand
uint16_t ALU; //hi: B, lo: A; result in lo

#define CYCLE_SPEED 10000

#define b 1 <<

enum microinstrs {
	//data wires
	c0 = b 1,  //PC  -> MAR
	c1 = b 2,  //BUS -> PC
	m0 = b 3,  //MAR -> RAM
	m1 = b 4,  //BUS -> MAR
	r0 = b 5,  //BUS -> RAM
	r1 = b 6,  //RAM -> BUS
	a0 = b 7,  //A   -> BUS
	a1 = b 8,  //BUS -> A
	a2 = b 9,  //A   -> ALU
	a3 = b 10, //ALU -> A
	b0 = b 11, //BUS -> B
	b1 = b 12, //B   -> BUS
	o0 = b 13, //BUS -> OUT
	i0 = b 14, //IR(lo) -> BUS
	i1 = b 15, //RAM -> IR(lo)
	p0 = b 16, //BUS -> IP
	//operations
	nop = b 0,
	hlt = b 17,
	irswp = b 18,
	pcinc = b 19,
	ipexec = b 20,
	stop = b 21,
	//alu ops
	add = b 22,
	sub = b 23,
	and = b 24,
	or  = b 25,
	xor = b 26
};

int opcodes[][4] = {
	{0,       0,0,0},  //NOP
	{a1,      0,0,0},  //LDD (load direct)
	{m1,m0,r1,a1},     //LDI (load indirect)
	{m1,a0|m0,r0, 0},  //STA (store)
	{hlt,     0,0,0},  //HALT
	{a0,o0,     0,0},  //OUT
	{c1,      0,0,0},  //JMP
	{b0,b1|a2,add,a3}, //ADD
	{b0,b1|a2,sub,a3}, //SUB
	{b0,b1|a2,and,a3}, //AND
	{b0,b1|a2,or, a3}, //OR
	{b0,b1|a2,xor,a3}  //XOR
};

#ifdef YDEBUG
char* opcodes_strings[] = {"NOP", "LDD", "LDI", "STA", "HALT", "OUT", "JMP", "ADD", "SUB", "AND", "OR", "XOR"};
#endif

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
		#define alu(bit, o) op(bit) ALU = (ALU & 0x00FF) o ((ALU & 0xFF00) >> 8)
		#define wire(bit, src, dest) op(bit) dest = src
		
		//wires
		wire(c0, PC, MAR);
		wire(c1, BUS, PC);
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
		op(hlt) while(1);
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
		//alu ops
		alu(add, +);
		alu(sub, -);
		alu(and, &);
		alu(or,  |);
		alu(xor, ^);
	}
}
