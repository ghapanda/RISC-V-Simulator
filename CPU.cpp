#include "CPU.h"

#include <iomanip> //remove this
#include <bitset> //For immGen

CPU::CPU()
{
	PC = 0; //set PC to 0
	for (int i = 0; i < 4096; i++) //copy instrMEM
	{
		dmemory[i] = (0);
	}
}


unsigned long CPU::readPC()
{
	return PC;
}
void CPU::incPC(int offset)
{
	PC+=offset;
}


int CPU::readMem(const int&index,bool byteSignal){
	if(index>4092||index<0){
		//cerr<<"index out of range for dmem"<<endl;
		exit(2);
	}
	if(byteSignal){ //for LB
		return dmemory[index];
	}
	else{ //for LW
      return (dmemory[index]&0xff)|((dmemory[index+1]<<8)&0xff00)|((dmemory[index+2]<<16)&0xff0000)|((dmemory[index+3]<<24)&0xff000000);
	}
}
int CPU::writeMem(const int&index,const int&data,bool byteSignal){
	if(index>4092||index<0){
		//cerr<<"index out of range for dmem"<<endl;
		exit(2);
	}
    if(byteSignal){ //for SB
      dmemory[index]=static_cast<char>(data&0xff);
	  
	}
	else{ //for SW
      dmemory[index]=static_cast<char>(data&0xff);
	  dmemory[index+1]=static_cast<char>((data>>8)&0xff);
	  dmemory[index+2]=static_cast<char>((data>>16)&0xff);
	  dmemory[index+3]=static_cast<char>((data>>24)&0xff);
	}
	return 0;
}
regFile::regFile():write_enable(false){
	rs1=nullptr;
	rs2=nullptr;
	rd=nullptr;
	for (int i=0;i<32;i++)
	regs[i]=(0);
}

void regFile::read(const unsigned long&instr){

	rs1=&regs[extract_rs1(instr)];
	rs2=&regs[extract_rs2(instr)];
	rd=&regs[extract_rd(instr)];

}
void regFile::write(const int &data){
	
      if(rd==nullptr||!write_enable){
		//cerr<<"cannot write to regfile"<<endl;
		exit(2);
	  }
	  if(rd!=&regs[0])
	  *rd=data;
	    
}

int regFile::read_rs1() const{
	if(rs1==nullptr){
		//cerr<<"rs1 has not been read properly"<<endl;
		exit(2);
	}
	return *rs1;
}
int regFile::read_rs2() const{
		if(rs2==nullptr){
		//cerr<<"rs2 has not been read properly"<<endl;
		exit(2);
	}
	return *rs2;
}

int regFile::read_rd() const{
		if(rd==nullptr){
		//cerr<<"rd has not been read properly"<<endl;
		exit(2);
	}
	return *rd;
}

int regFile:: read_reg(const int&index) const{
	if(index>31||index<0){
		//cerr<<"invalid index"<<endl;
		exit(2);
	}
	return regs[index];
}
// Add other functions here ... 
unsigned long fetch(const unsigned long& PC, const unsigned char* instMem){
	unsigned long index=PC;
	unsigned long instruction=instMem[index]|(instMem[index+1]<<8)|(instMem[index+2]<<16)|(instMem[index+3]<<24);
    return instruction;
}

unsigned char extractOpcode(const unsigned long&instr){
	unsigned char opCode=static_cast<unsigned char>(instr &0x7f);

	return opCode;
}




unsigned char extract_rs1(const unsigned long&instr){
	unsigned char rs1=(instr>>15)&0x1F;
		bitset<32>opcode_bin(rs1);

	return rs1;
}

unsigned char extract_rs2(const unsigned long&instr){
	unsigned char rs2=(instr>>20)&0x1F;

	return rs2;
}

unsigned char extract_rd(const unsigned long&instr){
	unsigned char rd=(instr>>7)&0x1F;
	return rd;
}

Controller::Controller(){
	RegWrite=false;
	ALUsrc=false;
	Branch=false;
	MemRe=false;
	MemWr=false;
	MemtoReg=false;
	Jmp=false;
	LdUImm=false;
	ALUOp=-1;
}
void Controller::set_signals(const unsigned char& opCode){
		switch (opCode){
		case 0b0110011:
		//Rtype
		     RegWrite=true;
			 ALUOp=2;
		     return;
		case 0b0010011:
		     
		//I type
		     RegWrite=true;
			 ALUsrc=true;
			 ALUOp=3;
			 return;

		
		case 0b0100011:
		//S type
		     ALUsrc=true;
			 MemWr=true;
			 ALUOp=0;//ADD
		     return;
		case 0b1100011:
		//Btype
			Branch=true;
			ALUOp=1;//SUBTRACT
		    return;
		case 0b1101111:
		//J type
            Jmp=true;
			RegWrite=true;
		    return;
		case 0b0110111:
		//U type LUI
		    RegWrite=true;
			LdUImm=true;
		    return;
		case 0b0000011:
		//load instructions

		    RegWrite=true;
			MemtoReg=true;
			ALUOp=0;//ADD
			ALUsrc=true;
			MemRe=true;
			return;
		default:
		exit(2);

		return;
	}
}



unsigned char extract_func3(const unsigned long&instr){
	unsigned char func3=(instr>>12)&0x7;
	return func3;
}

unsigned char extract_func7(const unsigned long&instr){
	unsigned char func7=(instr>>25)&0x7f;
	return func7;
}


int immGenerator(const unsigned long&instr){
	unsigned char opCode=extractOpcode(instr);
	unsigned char func3;
	int i;
	int j;


    bitset<32> instr_bits(instr);
	bitset <32> imm_bits;

	bitset<32> instr_bits_j(instr);
	bitset <32> imm_bits_j;

	long instr_signed=static_cast<long>(instr); //This is crucial so that the right shift is arithmetic
	int imm;
	switch (opCode){
	     
		case 0b0010011:
		     
		//I type //pay attention to SRAI
		   imm=instr_signed>>20;
		   func3=extract_func3(instr);
		   if(func3==0x5){
              return imm&0x1f;
		   }
		   
		   return imm; 
			 

		
		case 0b0100011:
		//S type => this is tricky be careful!
           imm=((instr>>7)&0x1F)|((instr_signed>>25)<<5);
		   return imm;
		
		case 0b1100011:
		//Btype
		   

		   imm_bits[11]=instr_bits[7];
		   imm_bits[12]=instr_bits[31];
		   for(i=1;i<5;i++)
		      imm_bits[i]=instr_bits[7+i]; //imm[4:1]
		   for(i=5,j=25;i<11;i++,j++)
			  imm_bits[i]=instr_bits[j];
			imm=((static_cast<int>(imm_bits.to_ullong()))<<19)>>19;
			return imm;
		   

		
		case 0b1101111:
		//J type
		   

		   imm_bits_j[20]=instr_bits_j[31];
		   imm_bits_j[11]=instr_bits_j[20];
		   for(i=1;i<11;i++)
		      imm_bits_j[i]=instr_bits_j[20+i]; //imm[4:1]
           for(i=12;i<20;i++)
		      imm_bits_j[i]=instr_bits_j[i];
			imm=((static_cast<int>(imm_bits_j.to_ullong()))<<11)>>11;
			return imm;
			
		case 0b0110111:
		//U type LUI
		   imm=(instr_signed>>12)<<12;
		   return imm;
		case 0b0000011:
		//load instructions
		   imm=instr_signed>>20;
		   return imm; 

		   
		
		default:		

	    return -1;
	}
}


ALU_control::ALU_control(){
	ALUOp=0;
	Byte_signal=false;
}
string ALU_control::generate_control(const unsigned long&instr){
	unsigned char func3;
	unsigned char opCode;
	switch(ALUOp){
		case 0:
		  opCode=extractOpcode(instr);
		  func3=extract_func3(instr);
		  if(opCode==0b0000011||opCode==0b0100011){
			if(func3==0x0){Byte_signal=true;} //for SB and LB
		  }
		  return "add";
		case 1:
		  return "subtract";
		case 2:
		 //R_type logic => for XOR and add

		  func3=extract_func3(instr);
		  if(func3==0x4){
			return "xor";
		  }
		  else if(func3==0x0){
			return "add";
		  }


		case 3:
		 //I type logic => for SRAI and ORI
		  func3=extract_func3(instr);
		  if(func3==0x6){
			return "ori";
		  }
		  else if(func3==0x5){
			return "srai";
		  }
		  else{
			exit(2);
		  }
		  default:
		  return "disabled";
	}	       
}







  ALU::ALU(){
	input_1=0;
	input_2=0;
  }

  void ALU::set_inputs(const int &in_1,const int &in_2){
	input_1=in_1;
	input_2=in_2;
  }

  int ALU::execute(const string control_input){
	int result=0;
	if(control_input=="add"){
		result=input_1+input_2;
	}
	else if(control_input=="subtract"){
		result=input_1-input_2;
		zero_flag=result==0;
	}
	else if(control_input=="xor"){
		result=input_1^input_2;
	}
	else if(control_input=="ori"){
		result=input_1|input_2;
	}
	else if(control_input=="srai"){
		result=input_1>>input_2;
	}
	else if(control_input=="disabled"){
		result=0;
	}
	else{
		exit(2);
	}
	return result;
  }




//wrapper functions
//write back stage
void write_regFile(regFile* rf,const Controller* cntr,const ALU_control* myALU_control,CPU* myCPU,const int & ALU_output,const int& imm){
	if(cntr->RegWrite){ //RegWrite signal must be set
		if(cntr->MemRe&&cntr->MemtoReg){ 
			rf->write(myCPU->readMem(ALU_output,myALU_control->Byte_signal)); //LB and LW already handled
		}

		else if(cntr->LdUImm){
			rf->write(imm);
		}
		else{
			rf->write(ALU_output);
			return;
		}
	}
} 


//exec stage
void set_ALU_inputs(ALU* alu, const Controller* controller,const int& rs1, const int &rs2, const int& imm){
	if (controller->ALUsrc){
	  alu->set_inputs(rs1,imm);
	}
	else{
		alu->set_inputs(rs1,rs2);
	}
}


//write back stage
void updatePC(regFile*rf, Controller* cntr, CPU* myCPU,const ALU* myALU,const int& imm){
     if(cntr->Branch&&myALU->zero_flag){ //for BEQ
        myCPU->incPC(imm);
		return;
	 }
	 else if(cntr->Jmp&&cntr->RegWrite){ //for jal
	    rf->write(myCPU->readPC()+4);
		myCPU->incPC(imm);
		
	 }
	 else{
        myCPU->incPC(4); //for PC+4
	 }
} //supporting jmp, and branch 


//write to memory (mem stage)
void write_memory(const regFile* rf, Controller* cntr,CPU* myCPU,const ALU_control* myALUcntr, const int& address){
	if(cntr->MemWr){
		int data=rf->read_rs2();
        myCPU->writeMem(address,data,myALUcntr->Byte_signal);
	}
}
