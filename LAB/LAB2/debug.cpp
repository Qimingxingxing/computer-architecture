#include<iostream>
#include<cmath>
#include<vector>
using namespace std;
class cache {
      public:
      	int *p; 
		cache(int blockSize, int associativity, int cacheSize)
		{
			int a[2][2] = {{1,1},{2,2}}; 
			this->blockSize = blockSize;
			this->associativity = associativity;
			this->cacheSize = cacheSize;
			p = &a[0][0];
		}
		int blockSize, associativity, cacheSize;
		
      };
int main()
{
	vector<vector<int> > sss;
	sss.resize(5);
for (int i = 0; i < 5; ++i)
    sss[i].resize(4);
	cache c(1,2,3); 
	cout << sss[1].size();
	sss[4][4] = 100;
	cout << sss[1][1];
}
