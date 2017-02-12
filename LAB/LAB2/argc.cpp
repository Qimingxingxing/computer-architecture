#include <iostream>  
  
using namespace std;  
  
int main(int argc,char *argv[])  
{  
    for(int i=0;i<argc;i++)  
    {  
        cout<<"argument["<<i<<"] is: "<<argv[i]<<endl;  
        cout << "ni ma" << endl;
    }  
    cout << "terminate" ; 
    system("pause");  
	return 0;
}
  
