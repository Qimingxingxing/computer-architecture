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


int main(int argc, char* argv[]){
    ifstream config;
	int m, index= 0;
	ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    traces.open(argv[2]);
    tracesout.open(outname.c_str());
	string line;    	
	int takenOrNot, prediction = 0;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;  
	vector<vector<int> > predictor;
	vector<int> array;
	int count= 0, correct= 0, wrong = 0;
	double missRate;
	array.resize(2);
    config.open(argv[1]);
    while(!config.eof())  // read config file
    {
		config >> m;
	}
	if(m>32 or m<0){
		cout << "the value of m is not valid!" << endl;
		exit(0);
	}
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
			predictor.resize(pow(2, m));
			for(int i=0; i< pow(2,m); i++){
				predictor[i].resize(2);
				predictor[i][0] = 1;
				predictor[i][1] = 1;
			}
	
    if (traces.is_open()&&tracesout.is_open()){    
        while (getline (traces,line)){   // read mem access file and access Cache
            count++;
            istringstream iss(line); 
            if (!(iss >> xaddr >> takenOrNot)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);

			index = bitset<32>(accessaddr.to_string().substr(32-m, m)).to_ulong(); 
			array = predictor[index];
			if(array[0] == 1 and array[1] == 1){
				if(takenOrNot == 0){
					predictor[index][0] = 1;
					predictor[index][1] = 0;
				}
				prediction = 1;
			}
			else if(array[0] == 1 and array[1] == 0){
				if(takenOrNot == 0){
					predictor[index][0] = 0;
					predictor[index][1] = 0;
				}
				else{
					predictor[index][0] = 1;
					predictor[index][1] = 1;			
				}
				prediction = 1;
			}	
			else if(array[0] == 0 and array[1] == 0){
				if(takenOrNot == 1){
					predictor[index][0] = 0;
					predictor[index][1] = 1;
				}
				prediction = 0;
			}						
			else if(array[0] == 0 and array[1] == 1){
				if(takenOrNot == 0){
					predictor[index][0] = 0;
					predictor[index][1] = 0;
				}
				else{
					predictor[index][0] = 1;
					predictor[index][1] = 1;				
				}
				prediction = 0;
			}	
			if(takenOrNot == prediction)
				correct++;
			else
				wrong++;
			tracesout<< prediction << endl;  

		}
		missRate = (double)wrong/(double)count; 
		traces.close();
        tracesout.close(); 

    }
    else cout<< "Unable to open trace or traceout file ";           

	return 0;
}

