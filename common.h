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

char* opcodes_strings[] = {"NOP", "LDD", "LDI", "STA", "HALT", "OUT", "JMP", "ADD", "SUB", "AND", "OR", "XOR"};
#define OPCODE_COUNT (sizeof(opcodes_strings)/sizeof(*opcodes_strings))
