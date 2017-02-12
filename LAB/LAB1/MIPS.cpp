#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include<cmath>
#include <sstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.
/*the sample instruction file is below: 
	0: lw $0, $1, 0
	1: lw $0, $2, 4
	2: j 5
	3: add $1, $2, $3
	4: add $1, $2, $3
	5: add $1, $2, $3
	6: add $1, $2, $3
	7: sw $0, $3, 4
	8: beq $1, $1, 1
	9: or $2, $3, $4
	10: and $2, $3, $5
	11: nor $2, $3, $6
	12: sub $2, $3, $3
	13: or $4, $5, $7
	14: addiu &8,$9, -1
	15: beq $1,$2,-1
	16: beq $2, $2, 3
	17: add $1, $2, $3
	18: add $1, $2, $3
	19: add $1, $2, $3
	20: halt
	
*/
// *********************************************************************************************************\


template <typename T>
  string to_string ( T Number )
  {
     ostringstream ss;
     ss << Number;
     return ss.str();
  }
template<size_t length>
static bitset<32> signedExtention(bitset<length> a);

// *********************************************************************************************************
class RF
{
    public:
        bitset<32> ReadData1, ReadData2; 
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);  
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
            // implement the funciton by you.    
			if(WrtEnable.to_string() =="0" )
			{
				
				ReadData1 = Registers[RdReg1.to_ulong()];
				ReadData2 = Registers[RdReg2.to_ulong()];
				cout << "R"<< RdReg1.to_ulong() <<" is "<<ReadData1<<", and R"<<RdReg2.to_ulong()<<" is " << ReadData2<<endl;
			}
			else{
				Registers[WrtReg.to_ulong()] = WrtData;
				cout << "the Data is written back into R"<< WrtReg.to_ulong()<<endl;
			}
         }
		 
	void OutputRF()
             {
               ofstream rfout;
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
	
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
                 // implement the ALU operations by you. 
				 switch(ALUOP.to_ulong()){
					 
					  case ADDU:
						ALUresult = bitset<32>(-oprand1[31] * pow(2,32) + oprand1.to_ulong() + 
						-oprand2[31]*pow(2,32) + oprand2.to_ulong());
						cout << "oprand1 + oprand2"<<endl;
						break;
					  case SUBU:
						ALUresult = bitset<32>(-oprand1[31] * pow(2,32) + oprand1.to_ulong() -
						(-oprand2[31]*pow(2,32) + oprand2.to_ulong()));
						cout << "oprand1 - oprand2"<<endl;
						break;
					  case AND:
						ALUresult = oprand1 & oprand2;
						cout << "oprand1 & oprand2"<<endl;
						break;
					  case OR:
						ALUresult = oprand1 | oprand2;
						cout << "oprand1 | oprand2"<<endl;
					    break;
					  case NOR:
						ALUresult = ~(oprand1 | oprand2);
						cout << "oprand1 nor oprand2"<<endl;
						break;
					   
				 }
                 return ALUresult;
               }            
};

class INSMem
{
      public:
          bitset<32> Instruction;
		  long int ReadAdd;
		  string ins;
          INSMem()
          {       IMem.resize(MemSize); 
                  ifstream imem;
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                        IMem[i] = bitset<8>(line);
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
              {    
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
			   ReadAdd = ReadAddress.to_ulong();
			   ins = IMem[ReadAdd].to_string()+IMem[ReadAdd+1].to_string()+IMem[ReadAdd+2].to_string()
			   +IMem[ReadAdd+3].to_string();
			   Instruction = bitset<32>(ins);
               return Instruction;     
              }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
		  string WriteData_string;
          DataMem()
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {    
               if(Address.to_ulong() > MemSize){
				   cout << "memory overflow !!! Program terminates";
				   throw "memory overflows!";
				}
               // implement by you.
			   if(readmem.to_string() == "1"){
					readdata =bitset<32>(DMem[Address.to_ulong()].to_string()+DMem[Address.to_ulong()+1].to_string()
						+DMem[Address.to_ulong()+2].to_string()+DMem[Address.to_ulong()+3].to_string());
			   }
			   else if(writemem.to_string()=="1"){
				   WriteData_string = WriteData.to_string();
				   DMem[Address.to_ulong()] = bitset<8>(WriteData_string.substr(0,8));
				   DMem[Address.to_ulong()+1] = bitset<8>(WriteData_string.substr(8,8));
				   DMem[Address.to_ulong()+2] = bitset<8>(WriteData_string.substr(16,8));
				   DMem[Address.to_ulong()+3] = bitset<8>(WriteData_string.substr(24,8));
				   cout<<WriteData_string <<" has been written into memory address that starts from "<<Address << endl; 
			   }
               return readdata;     
          }   
                     
          void OutputDataMem()
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
      
};  


// *********************************************************************************************************


template<size_t length>
static bitset<32> signedExtention(bitset<length> a){
	bitset<32> extendedValue;
	string tem;
	if(a.test(a.size()-1) == 1){
		tem = a.to_string();
		for(int i = 0; i<32-length;i++)
			tem = "1"+ tem;
		extendedValue = bitset<32>(tem);
	}
	else{
		tem = a.to_string();
		for(int i = 0; i<32-length;i++)
			tem = "0"+ tem;
		extendedValue = bitset<32>(tem);	
	}	
	return extendedValue;
}



// *********************************************************************************************************
int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;
	bitset<32> instruction,PC(0),oprand1, oprand2, ALUresult, MemoryData,Imm;
	bitset<5> op1,op2,op3;
	bitset<16> op3_16;
	bitset<26> op1_26;
	bitset<3> ALUOP(0);
	string InstructionString, JNewPCString;
	long int op1_lint, op2_lint, op3_lint, op3_16lint,op1_26lint;
	vector<string> instructionSet;
	int isLoad=0, isStore=0, isR_type=0, isI_type=0, isJ_type=0, wrtEnable=0,  isBranch=0, IsEq = 0;
	cout << "welcome to our single-cycle MIPS processor!!!" << endl;
    while (1)
	{
		cout << "*******************************************************************"<<endl;

        // Fetch
        instruction = myInsMem.ReadMemory(PC);
		cout << "current PC is " <<PC<<endl;
		InstructionString = instruction.to_string();
		cout << "the instruction we read is "<<InstructionString<<endl;
		
		// If current insturciton is "11111111111111111111111111111111", then break;
		if(InstructionString=="11111111111111111111111111111111"){
			cout << "this is a halt instruction, the program will terminate!"<<endl;
			break;
		}
// *********************************************************************************************************
		// set signal isLoad, isStore, is R_type, is J_type, isBranch and isI_type based on instruction
		if(InstructionString.substr(0, 6) =="100011" ){
			cout << "this is a load instruction"<<endl;
			isLoad = 1;					
		}
		else if(InstructionString.substr(0, 6) =="101011" ){
			cout << "this is a store instruction" << endl;
			isStore = 1;
		}
		else if(InstructionString.substr(0, 6) =="000000" ){
			cout << "this is a R_type instuction" << endl;
			isR_type = 1;
		}
		else if(InstructionString.substr(0, 6) =="000010" ){
			cout << "this is a J_type instuction" << endl;
			isJ_type = 1;
		}
		else if(InstructionString.substr(0, 6) =="000100" ){
			cout << "this is a branch instuction" << endl;
			isBranch = 1;
		}
		if(InstructionString.substr(0, 6) !="000010" and 
		InstructionString.substr(0, 6) !="000000" )
			cout << "this is a I_type instuction" << endl;		
			isI_type = 1;
						
		// set ALUOP
		if(InstructionString.substr(0, 6) =="100011" or InstructionString.substr(0, 6) 
			=="101011" or InstructionString.substr(0, 6) =="001001"){
			ALUOP = 1; cout << "ALU oprand is set to 1" << endl;}
		else{
			ALUOP = bitset<3>(InstructionString.substr(29, 3)); 
		}

		//set wrtEnable
		if(isStore or isBranch or isJ_type)
			wrtEnable = 0;
		else
			wrtEnable = 1;
// *********************************************************************************************************

/* now we start to execute the instruction
	We divide the code into three situations.
	If it is J_type, we just need to set PC to its new value;
	if it is branch instruction, we first need to read rs and rt, then we set the new PC value
	if it is I_type or R_type,  we set PC = PC + 4;

*/
		if(isJ_type){
			// SetPC 
			JNewPCString = PC.to_string().substr(0,4) + InstructionString.substr(6, 26) + "00";
			PC = bitset<32>(JNewPCString);
			cout << "new PC is set to " <<PC << endl;
			// add to the instruction set we have executed
			op1_26lint = bitset<26>(InstructionString.substr(6, 26)).to_ulong();
			instructionSet.push_back(" J "+to_string(op1_26lint));
		}
		// although branch is one of the I_type instruction, we just deal with it by individual
		else if(isBranch){
			// judge if rs == rt, then PC = PC + Imm + 4
			//read rs and rt
			op1 = bitset<5>(InstructionString.substr(6, 5));
			op2 = bitset<5>(InstructionString.substr(11, 5));
			op3_16 = bitset<16>(InstructionString.substr(16, 16));
			myRF.ReadWrite(op1,
			op2,0,0,0);	
			op1_lint = op1.to_ulong();
			op2_lint = op2.to_ulong();
			op3_16lint = -op3_16[15] * pow(2,16) + op3_16.to_ulong();
			instructionSet.push_back(" Beq $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_16lint));			
			
			if(myRF.ReadData1 == myRF.ReadData2){
				Imm = signedExtention(op3_16);
				Imm <<= 2;
				PC = bitset<32>(-PC[31] * pow(2,32)+PC.to_ulong() + 4 + Imm.to_ulong());
				cout << "two registers are equal, new PC is set to branch PC " << PC << endl;
			}
			else{
				PC = bitset<32>(PC.to_ulong()+4);
				cout << "two registers are not equal, PC = PC + 4, new PC is set to " <<PC << endl;
			}
		}
		else if(isI_type or isR_type){
			//register file store? load? addi?
			//read rs and rt
			//set PC
			PC = bitset<32>(PC.to_ulong() +4 );
			cout << "new PC is set to " <<PC << endl;
			// read two registers
			op1 = bitset<5>(InstructionString.substr(6, 5));
			op2 = bitset<5>(InstructionString.substr(11, 5));
			op1_lint = op1.to_ulong();
			op2_lint = op2.to_ulong();		
			
			myRF.ReadWrite(op1,op2,0,0,0);	
			
			// we need to use imm value for load, store and addi instruction
			if(isLoad or isStore or InstructionString.substr(0, 6) =="001001"){
				oprand1 = myRF.ReadData1; // rs
				op3_16 = bitset<16>(InstructionString.substr(16, 16));
				op3_16lint = -op3_16[15] * pow(2,16) + op3_16.to_ulong();
				oprand2 = signedExtention(op3_16);//Imm
				cout << "oprand1 rs is "<<oprand1<<endl;
				cout << "oprand2 imm is "<<oprand2<<endl;
			}
			//R type 
			else{
				op3 = bitset<5>(InstructionString.substr(16, 5));
				op3_lint =  op3.to_ulong();
				switch(ALUOP.to_ulong() ){
					case ADDU:
						instructionSet.push_back(" add $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_lint));			
						break;
					case SUBU:
						instructionSet.push_back(" sub $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_lint));				
						break;
					case OR:
						instructionSet.push_back(" or $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_lint));	
						break;
					case NOR:
						instructionSet.push_back(" nor $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_lint));	
						break;
					case AND:
						instructionSet.push_back(" and $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_lint));	
						break;
				}
				oprand1 = myRF.ReadData1; //rs
				oprand2 = myRF.ReadData2; //rt
				cout << "oprand1 is "<<oprand1<<endl;
				cout << "oprand2 is "<<oprand2<<endl;
			}
			
			// Execute
			ALUresult = myALU.ALUOperation(ALUOP, oprand1, oprand2); //rs + Imm | rs+rt
			cout << "ALUresult is "<< ALUresult<< endl;
			
			//readwrite memory
			if(isStore){
				myDataMem.MemoryAccess(ALUresult, myRF.ReadData2, isLoad, isStore); // store rt value into M4[rs+Imm]
				instructionSet.push_back(" sw $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_16lint));

			}
						
			else if(isLoad){
				MemoryData = myDataMem.MemoryAccess(ALUresult,0,isLoad,isStore);//load M4[rt+Imm] into MemoryData
				instructionSet.push_back(" lw $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_16lint));
				cout<<"Memory data we get from "<<ALUresult << " is " << MemoryData<<endl;
			}
				
			// Write back to RF
			if(!isStore){   	//don't need to write back for store
				if(isR_type){
					myRF.ReadWrite(0, 0, op3, ALUresult, wrtEnable);
				}
				else if(isI_type and !isLoad){
					instructionSet.push_back(" addi $"+to_string(op1_lint)+", $"+to_string(op2_lint)+", "+to_string(op3_16lint));
					myRF.ReadWrite(0, 0, op2, ALUresult, wrtEnable);
				}
				else if(isLoad){
					myRF.ReadWrite(0, 0, op2, MemoryData, wrtEnable);
				}
			}
		}

// *********************************************************************************************************
// we initial variables for each loop
		ALUOP = 0;
		oprand1 = oprand2 = MemoryData = ALUresult = 0; op3 = 0;
		isLoad = isStore = isR_type = isI_type = isJ_type = wrtEnable = isBranch = IsEq = 0;
		cout << endl;
        myRF.OutputRF(); // dump RF;    
    }
        myDataMem.OutputDataMem(); // dump data mem
		
		cout << "*******************************************************************"<<endl;
		cout << "the codes we have executed are below: (without the instructions we've jumped over)" << endl; 
		for(vector<string>::iterator it = instructionSet.begin(); it!=instructionSet.end();++it )
			cout << *it <<endl;
		cout<<" halt "<<endl;
		cout << "*******************************************************************"<<endl;
        return 0;
        
}
