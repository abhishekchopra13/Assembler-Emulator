#include <bits/stdc++.h>
#include "mnemonics.h"
using namespace std;
vector<int> Code; // Code
int count_instr = 0;
int read_flag = 0, write_flag = 0, trace_flag = 0;
string str, file_name;

void Segmentation_fault()
{
	printf("Segmentation Fault\n");
	exit(0);
}

void help()
{
	printf("trace current instruction only: -t\n");
	printf("See all read operations: -r\n");
	printf("See all write opereration: -w\n");
	printf("See the code segment: -before\n");
	printf("Trace all instructions at once and save trace to file_name.txt: -all\n");
	printf("See next instruction to be executed: -next\n");
	printf("See memory: -dump\n");
	printf("To view instructions: -help\n");
}

/*

Features I am thinking off--

-trace  show instruction trace
-read   show memory reads
-write  show memory writes
-before show memory dump before execution
-after  show memory dump after execution
-wipe   wipe written flags before execution
-isa    display ISA

*/

pair<long int, bool> read_operand (string &operand)
{
    if ((int)operand.size() == 0) {
        return {0, false};
    }
    int len = (int)operand.size();
    char *str = (char *)malloc(len *  sizeof(char));
    for (int i = 0; i < (int)operand.size(); i++) {
        str[i] = operand[i];
    }
    for (int i = (int)operand.size(); i < strlen(str); i++) {
        str[i] = '\0';
    }
    char *end;
    long int num;
    num = strtol(str, &end, 10);
    if (!*end) {
        return {num, true};
    }
    num = strtol(str, &end, 16);
    if (!*end) {
        return {num, true};
    }
    return {-1, false};
}

void trace()
{
	printf("PC = %08X, SP = %08X, A = %08X, B = %08X\n",PC,SP,A,B);
}

int execute()
{
	if (PC >= (int)Code.size()) {
		Segmentation_fault();
	}
	int op = (0xFF & Code[PC]);
	int val = Code[PC]>>8;
	if (str != "-r" and str != "-w")
	{
		printf("%s ", mnemonics[op]);
	    if(op < 6 || (op == 10 || op == 13) || (op >= 15 && op <= 17)) {
	    	printf("%08X\n", val);
	    } else {
	    	printf("\n");
	    }		
	}
	if (op == 18) {
		count_instr++;
		return 0;
	}
	(asm_func[op])(&val); // Execute functions
	count_instr++;
	PC++;
	if (trace_flag) {
		trace();
	}
	if((val==2 || val ==4 )&&(str == "-r")) {
	    printf("Reading Memory[%08X] it has value %08X\n",addr,A); 
	}
	if((val==3 || val == 5 )&&(str == "-w")) {
		printf("Writing Memory[%08X] value was %08X and now is %08X\n",addr,flag,memory[addr]);
	}
	return 1;
}

void before()
{
	for(int i=0;i<Code.size();i=i+4)
	{
		printf("%08X %08X %08X %08X %08X\n", i, Code[i], Code[i+1], Code[i+2],Code[i+3]);
	}
}
void dump()
{
	int base;
	read_again:
	cout << "Base address: ";
	string operand;
	cin >> operand;
	pair<long int, bool> val = read_operand(operand);
	if (!val.second) {
		cout << "Enter address in correct format hex or decimal\n";
		goto read_again;
	}
	base = val.first;
	read_again1:
	cout << "Offset: ";
	cin >> operand;
	val = read_operand(operand);
	if (!val.second) {
		cout << "Enter offset in correct format hex or decimal\n";
		goto read_again1;
	}
	for (int i = base; i < base + val.first; i+=4) {
		printf("%08X %08X %08X %08X %08X\n", i, memory[i], memory[i+1], memory[i+2], memory[i+3]);	
	}
}

void next()
{
	int op = (0xFF & Code[PC]);
	int val = Code[PC]>>8;
	printf("%s ", mnemonics[op]);
    if(op < 6 || (op == 10 || op == 13) || (op >= 15 && op <= 17)) {
    	printf("%08X\n", val);
    } else {
    	printf("\n");
    }
}

void all(){
	// ofstream outList(x);
	FILE *fptr;

    // opening file in writing mode
    fptr = fopen(file_name.c_str(), "w");
	while(execute()) {
		int op = (0xFF & Code[PC]);
		int val = Code[PC]>>8;
		fprintf(fptr,"%s %08X\n", mnemonics[op], val);
		fprintf(fptr,"A = %08X , B = %08X , PC = %08X , SP = %08X\n", A, B, PC, SP);
	}
}

int interact()
{
	printf("Operation: ");
	
	cin >> str;
	if (str == "-t") {
		trace_flag = 1;
		int run = execute();
		trace();
		trace_flag = 0;
		if (!run)	return 0;
	} else if (str == "-r") {
		while(execute()){
		}
		return 0;
	} else if (str == "-w") {
		while(execute()){
		}return 0;
	} else if (str == "-before") {
		before();
		return 0;
	} else if(str == "-next"){
		next();
		return 1;
	} else if (str == "-all") {
		trace_flag = 1;
		all();
		return 0;
	}else if(str == "dump") {
		dump();
	} else if (str == "-help") {
		help();
	}
	else {
		printf("Invalid operation\n");
		return 1;
	}
	return 1;
}

int main(int argc, char*argv[])
{
	PC = 0, SP = 0, A = 0, B = 0;
	int val;
	ifstream file (argv[1], ios::in | ios::binary);
    while(file.read((char*)&val,sizeof(int)))
    { 
    	Code.push_back(val);
    }
    for (int i = 0; i < strlen(argv[1]); i++) {
    	if (argv[1][i] == '.')
    		break;
    	file_name.push_back(argv[1][i]);
    }
    file_name += ".txt";
    // (asm_func[op])(&val); how to call function
   	for (int i = 0; i < (int)Code.size(); i++)
		memory[i] = Code[i];	
   	while (1) {
   		int run = interact();
   		if (!run)	break;
   	}
   	printf("Do you want to check memory 'y' or 'n'\n");
   	char reply; cin >> reply;
   	if (reply == 'y') {
   		dump();
   	}
   	printf("Instructions executed are %d\n", count_instr);
	return 0;
}