#define b 1 <<

#if 0
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
	carry = b 22, //A->BUS if carry set
	//alu ops
	add = b 23,
	sub = b 24,
	and = b 25,
	or  = b 26,
	xor = b 27
};
#else
#define GEN_MICRO_INSTRS \
X(nop)\
/*WIRES*/\
X(c0)  /*PC  -> MAR*/\
X(c1)  /*BUS -> PC*/\
X(c2)  /*PC  -> BUS*/\
X(m0)  /*MAR -> RAM*/\
X(m1)  /*BUS -> MAR*/\
X(r0)  /*BUS -> RAM*/\
X(r1)  /*RAM -> BUS*/\
X(a0)  /*A   -> BUS*/\
X(a1)  /*BUS -> A*/\
X(a2)  /*A   -> ALU*/\
X(a3)  /*ALU -> A*/\
X(b0)  /*BUS -> B*/\
X(b1)  /*B   -> BUS*/\
X(o0)  /*BUS -> OUT*/\
X(i0)  /*IR(lo) -> BUS*/\
X(i1)  /*RAM -> IR(lo)*/\
X(p0)  /*BUS -> IP*/\
/*OPERATIONS*/\
X(hlt)\
X(irswp)\
X(pcinc)\
X(ipexec)\
X(stop)\
X(carry) /*A->BUS if carry set*/\
X(zero)  /*A->BUS if zero set*/\
/*ALU OPS*/\
X(add)\
X(sub)\
X(and)\
X(or)\
X(xor)\

//WORKaroud maybe conter not beign 0
enum {COUNTER_BASE = (__COUNTER__+1)};

enum {
#define X(w) w = 1<<(__COUNTER__-COUNTER_BASE),
GEN_MICRO_INSTRS
_m_i_count_
};

#endif


int opcodes[][4] = {
	{0,       0,0,0},  //NOP
	{a1,      0,0,0},  //LDD (load direct)
	{m1,m0,r1,a1},     //LDI (load indirect)
	{m1,a0|m0,r0, 0},  //STA (store)
	{b0,      0,0,0},  //LDBD (load direct B)
	{m1,m0,r1,b0},     //LDBI (load indirect B)
	{hlt,     0,0,0},  //HALT
	{a0,o0,     0,0},  //OUT
	{c1,      0,0,0},  //JMP
	{0,b1|a2,add,a3},  //ADD
	{0,b1|a2,sub,a3},  //SUB
	{0,b1|a2,and,a3},  //AND
	{0,b1|a2,or, a3},  //OR
	{0,b1|a2,xor,a3},  //XOR
	{a1,c2,carry,c1}, //JC (jump, if CARRY)
	{a1,c2,zero,c1}   //JZ (jump, if ZERo)
};

char* opcodes_strings[] = {"NOP", "LDD", "LDI", "STA", "LDBD", "LDBI", "HALT", "OUT", "JMP", "ADD", "SUB", "AND", "OR", "XOR", "JC", "JZ"};
#define OPCODE_COUNT (sizeof(opcodes_strings)/sizeof(*opcodes_strings))
