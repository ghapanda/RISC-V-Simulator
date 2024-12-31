#include "CPU.h"

#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include<fstream>
#include <sstream>


#include <bitset> 
using namespace std;


int main(int argc, char* argv[])
{
	/* This is the front end of your project.
	You need to first read the instructions that are stored in a file and load them into an instruction memory.
	*/

	/* Each cell should store 1 byte. You can define the memory either dynamically, or define it as a fixed size with size 4KB (i.e., 4096 lines). Each instruction is 32 bits (i.e., 4 lines, saved in little-endian mode).
	Each line in the input file is stored as an hex and is 1 byte (each four lines are one instruction). You need to read the file line by line and store it into the memory. You may need a mechanism to convert these values to bits so that you can read opcodes, operands, etc.
	*/

	unsigned char instMem[4096];


	if (argc < 2) {
		//cout << "No file name entered. Exiting...";
		return -1;
	}

	ifstream infile(argv[1]); //open the file
	if (!(infile.is_open() && infile.good())) {
		cout<<"error opening file\n";
		return 0; 
	}
	string line; 
	int i = 0;
	while (infile) {
			infile>>line;	
			unsigned int byteValue = stoi(line, nullptr, 16);
			instMem[i] = static_cast<char>(byteValue);
			i++;
		}
	int maxPC= i-3; 


	/* Instantiate your CPU object here.  CPU class is the main class in this project that defines different components of the processor.
	CPU class also has different functions for each stage (e.g., fetching an instruction, decoding, etc.).
	*/

	CPU *myCPU=new CPU; 
	regFile* myRegFile=new regFile;

	

	bool done = true;
	while (done == true) // processor's main loop. Each iteration is equal to one clock cycle.  
	{   Controller* myController=new Controller;
	    ALU_control* myALU_control=new ALU_control;
		ALU* myALU=new ALU;
		//fetch
		unsigned long PC=myCPU->readPC();
		unsigned long instr=fetch(PC,instMem);
		

		// decode
		unsigned char opCode=extractOpcode(instr);
		if (!opCode){
			delete myALU;
			delete myALU_control;
			delete myController;
			break;
		}
		myController->set_signals(opCode);
	
		//regfile read
		myRegFile->write_enable=myController->RegWrite;
        myRegFile->read(instr);
		//immGen
		int imm=immGenerator(instr);
		//ALU control
		myALU_control->set_ALUop(myController->ALUOp);
		string ALU_control_input=myALU_control->generate_control(instr);
		//ALU
		set_ALU_inputs(myALU,myController,myRegFile->read_rs1(),myRegFile->read_rs2(),imm);
		int ALU_output=myALU->execute(ALU_control_input);
		//Mem stage and WB stage
		write_memory(myRegFile,myController,myCPU,myALU_control,ALU_output);
		write_regFile(myRegFile,myController,myALU_control,myCPU,ALU_output,imm);
		updatePC(myRegFile,myController,myCPU,myALU,imm);
	
		if (myCPU->readPC() > maxPC){
			delete myALU;
			delete myALU_control;
			delete myController;
			break;
		}

	}
	
	int a0 =myRegFile->read_reg(10);
	int a1 =myRegFile->read_reg(11);  
	delete myCPU;
	delete myRegFile;
	// print the results (you should replace a0 and a1 with your own variables that point to a0 and a1)
	cout <<"("<<a0<<","<< a1 <<")"<<endl;



	return 0;

}




