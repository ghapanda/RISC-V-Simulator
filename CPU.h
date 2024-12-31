#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
using namespace std;


class CPU {
private:
	char dmemory[4096]; //data memory byte addressable in little endian fashion;
	unsigned long PC; //pc 

public:
	CPU();
	unsigned long readPC();
	void incPC(int offset);
	int readMem(const int&index,bool byteSignal);
	int writeMem(const int& index,const int&data,bool byteSignal);
	
};


class regFile{
private:
	int regs[32];
	int* rs1;
	int* rs2;
	int write_data;
	int* rd;
public:
    regFile();
	void read(const unsigned long&instr);
	void write(const int&data);
	int read_rs1() const;
	int read_rs2() const;
	int read_rd() const;
	int read_reg(const int&index) const;
	bool write_enable;
};

class Controller{
	public:
	Controller();
	bool RegWrite;
	bool ALUsrc;
	bool Branch;
	bool MemRe;
	bool MemWr;
	bool MemtoReg;
	bool Jmp;
	bool LdUImm;
	char ALUOp;
	void set_signals(const unsigned char& opCode);

};


class ALU_control{
    
    char ALUOp;
	public:
	  ALU_control();
	  bool Byte_signal;
	  void set_ALUop(const char& op){ALUOp=op;}
	  string generate_control(const unsigned long&instr);

};


class ALU{
	int input_1;
	int input_2;
	public:
	ALU();
	bool zero_flag;
	void set_inputs(const int &in_1,const int &in_2);
	int execute(const string control_input);
};



unsigned long fetch(const unsigned long &PC, const unsigned char* instMem);
unsigned char extractOpcode(const unsigned long&instr);
unsigned char extract_rs1(const unsigned long&instr);
unsigned char extract_rs2(const unsigned long&instr);
unsigned char extract_rd(const unsigned long&instr);
unsigned char extract_func3(const unsigned long&instr);
unsigned char extract_func7(const unsigned long&instr);
int immGenerator(const unsigned long&instr);



//wrapper functions
void write_regFile(regFile* rf,const Controller* cntr,const ALU_control* myALU_control,CPU* myCPU,const int & ALU_output,const int& imm);
void write_memory(const regFile* rf, Controller* cntr,CPU* myCPU,const ALU_control* myALUcntr, const int& address);

void set_ALU_inputs(ALU* alu, const Controller* controller,const int& rs1, const int &rs2, const int& imm);
void updatePC(regFile*rf, Controller* cntr, CPU* myCPU,const ALU* myALU,const int& imm); //supporting jmp, and branch 