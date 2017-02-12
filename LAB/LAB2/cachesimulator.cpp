/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss
struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };
static int isPowerOfTwo (unsigned int x)
{
 while (((x & 1) == 0) && x > 1) /* While x is even and > 1 */
   x >>= 1;
 return (x == 1);
}
class cache {
	  private: 
		int blockSize, associativity, cacheSize;
      public:
		int numOfBlocksPerSet, numOfWays, numOfTagBits, numOfIndexBits, numOfOffsetBits, replaceWay;
		string AddtagBits, indexBits, offsetBits;
		long int indexBits_int;
		//int counter; 
		vector<int> counter;
		struct block{
			int validBit;
			int dirtyBit;
			string tagBits;
		} b, *block, blockFound, replaceBlock;
		vector<vector<struct block> > setWay;
		//constructor
		cache(int blockSize, int associativity, int cacheSize)
		{
			if(!isPowerOfTwo(blockSize) or (associativity!=0 and !isPowerOfTwo(associativity))){
				cout << "blockSize and associativity must be the power of 2!!!" << endl;
				exit(0);
			}
			this->blockSize = blockSize;
			this->associativity = associativity;
			this->cacheSize = cacheSize;
			if(associativity){
				numOfBlocksPerSet = 1024 * cacheSize / associativity / blockSize;
				numOfWays = associativity;
			}
			else{
				numOfBlocksPerSet = 1; 
				numOfWays =  1024 * cacheSize / blockSize;
			}
			numOfTagBits = 32 - log2(numOfBlocksPerSet) - log2(blockSize);
			numOfIndexBits = log2(numOfBlocksPerSet);
			numOfOffsetBits = log2(blockSize);
			setWay.resize(numOfBlocksPerSet);
			for (int i = 0; i < numOfBlocksPerSet; ++i)
    		setWay[i].resize(numOfWays);
    		counter.resize(pow(2,numOfIndexBits));
    		for(int i=0; i<numOfIndexBits; i++){
    			counter[i] = 0;
			}
		}
		
		int ReadAddress(bitset<32> address){
			AddtagBits = address.to_string().substr(0,numOfTagBits);
			indexBits = address.to_string().substr(numOfTagBits, numOfIndexBits);
			offsetBits = address.to_string().substr(numOfTagBits + numOfIndexBits, numOfOffsetBits);
			indexBits_int = bitset<32>(indexBits).to_ulong();
			for(int i = 0; i < numOfWays; i++){
				blockFound = setWay[indexBits_int][i];
				if(blockFound.tagBits == AddtagBits and blockFound.validBit)
					return RH;
			}
			return RM;
		}
		int WriteAddress(bitset<32> address){
			AddtagBits = address.to_string().substr(0,numOfTagBits);
			indexBits = address.to_string().substr(numOfTagBits, numOfIndexBits);
			offsetBits = address.to_string().substr(numOfTagBits + numOfIndexBits, numOfOffsetBits);
			indexBits_int = bitset<32>(indexBits).to_ulong();
			for(int i = 0; i < numOfWays; i++){
				blockFound = setWay[indexBits_int][i];
				if(blockFound.tagBits == AddtagBits and blockFound.validBit)
					return WH;
			}
			return WM;			
		}
		string replace(bitset<32> address){
			AddtagBits = address.to_string().substr(0,numOfTagBits);
			indexBits = address.to_string().substr(numOfTagBits, numOfIndexBits);
			offsetBits = address.to_string().substr(numOfTagBits + numOfIndexBits, numOfOffsetBits);
			indexBits_int = bitset<32>(indexBits).to_ulong();
			replaceWay = counter[indexBits_int];
			if(replaceWay == numOfWays -1)
				counter[indexBits_int] = 0;
			else 
				counter[indexBits_int]++;
			replaceBlock = setWay[indexBits_int][replaceWay];
			AddtagBits = replaceBlock.tagBits;
			setWay[indexBits_int][replaceWay].tagBits = address.to_string().substr(0,numOfTagBits);
			setWay[indexBits_int][replaceWay].validBit = 1;
			if(replaceBlock.dirtyBit == 0){
				return "";
			}
			else{
				setWay[indexBits_int][replaceWay].dirtyBit = 0;
				return (AddtagBits + indexBits + offsetBits);
			}
				
		}
		};

			
int main(int argc, char* argv[]){
    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open(argv[1]);
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;              
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;              
      cache_params>>cacheconfig.L2blocksize;           
      cache_params>>cacheconfig.L2setsize;        
      cache_params>>cacheconfig.L2size;
      }
	  
	cache L1(cacheconfig.L1blocksize,cacheconfig.L1setsize,cacheconfig.L1size); 
	cache L2(cacheconfig.L2blocksize,cacheconfig.L2setsize,cacheconfig.L2size);
	
  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
   
   
    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
    
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;        
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
    
    string writeBackAdd;
    if (traces.is_open()&&tracesout.is_open()){    
        while (getline (traces,line)){   // read mem access file and access Cache
            
            istringstream iss(line); 
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
           
           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0)
             {   
				L1AcceState = L1.ReadAddress(accessaddr);
				if(L1AcceState == RH){
					//L1 hit
					L2AcceState = NA;
				}
				else{
					//L1 miss
					writeBackAdd =L1.replace(accessaddr);
					// if dirtybit = 0, no writeBack Data
					if(writeBackAdd == ""){
						L2AcceState = L2.ReadAddress(accessaddr);
					}
					// if dirtybit = 1, have writeBack Data
					else{
						L2AcceState = L2.ReadAddress(accessaddr);
						L2.WriteAddress(bitset<32>(writeBackAdd));
					}
					if(L2AcceState == RM){
						L2.replace(accessaddr);
					}
				}
             }
             else 
             {   
				L1AcceState = L1.WriteAddress(accessaddr);
				if(L1AcceState == WH){
					//write hit
					L2AcceState = NA;
				}
				else{
					//write miss
					L2AcceState = L2.WriteAddress(accessaddr);
				}
				
             }	
              
                           
            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
             
             
        }
        traces.close();
        tracesout.close(); 
    }
    else cout<< "Unable to open trace or traceout file ";

    return 0;
}
