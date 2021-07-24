/* opcodes have lower 8 bits and operands have upper 24 bits */
#define NUM_OF_OPCODES 18

int A,B;
int PC,SP;
int memory[(1<<25)+1];
int flag=0,addr=0;

void ldc(const void* value){
	// Load accumulator with the value specified 
	B = A;
	A = *(int*)value;
}

void adc(const void* value){
	// Add to the accumulator
	A += *(int*)value;
}

void ldl(const void* offset){
	// Load local 
	B = A;
	A = memory[SP + *(int*)offset];
}

void stl(const void* offset){
	// Store local 
	memory[SP + *(int*)offset] = A;
	A = B;
}

void ldnl(const void* offset){
	// Load non-local
	A = memory[A + *(int*)offset];
}

void stnl(const void* offset){
	// Store non-local 
	memory[A + *(int*)offset] = B;
}

void add(const void* off){
	// Addition 
	A = B+A;
}

void sub(const void* off){
	// Subtraction 
	A = B-A;
}

void shl(const void* off){
	// Shift left 
	A = B<<A;
}

void shr(const void* off){
	// Shift right 
	A = B>>A;
}

void adj(const void* value){
	// Adjust SP 
	SP = SP + *(int*)value;
}

void a2sp(const void* off){
	// Transfer A to SP; 
	SP = A;
	A = B;
}

void sp2a(const void* off){
	// Transfer SP to A 
	B = A;
	A = SP;
}

void call(const void* offset){
	// Call procedure 
	B = A;
	A = PC;
	PC = *(int*)offset - 1;
}

void ret(const void* offset){
	// Return from procedure
	PC = A;
	A = B;
}

void brz(const void* offset){
	// If accumulator is zero, branch to
	// specified offset 
	if(A == 0)
		PC = PC + *(int*)offset;
}

void brlz(const void* offset){
	// If accumulator is less than zero,
	// branch to specified offset 
	if(A < 0)
		PC = PC + *(int*)offset;
}

void br(const void* offset){
	// Branch to specified offset 
	PC = PC + *(int*)offset;
}

void (*asm_func[])(const void*) = {ldc,adc,ldl,stl,ldnl,stnl,add,sub,shl,shr,adj,a2sp,sp2a,call,ret,brz,brlz,br};
char mnemonics[][7] = {"ldc","adc","ldl","stl","ldnl","stnl","add","sub","shl","shr","adj","a2sp","sp2a","call","return","brz","brlz","br","HALT"};
