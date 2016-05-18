#include<iostream>
#include<stack>
#include<list>
#include<string>
#include<cstdlib>
#include<cmath>
#include<algorithm>
#include<unordered_map>
#include<queue>
#include"parser.cpp"
using namespace std;

int main(int argc, char *argv[])
{
    string filename;
    if(argc < 2)
    {
        cout<<"Error in input format"<<endl;
    }
    else if(argc == 2)
    {
        filename = (string)argv[1];
        parser(filename,false,false,false,false);
    }
    else if(argc == 3)
    {
		string str(argv[1]);
		filename = (string)argv[2];
		if(str == "-ast")		// Flag to print AST tree and output
		{
			parser(filename,false,true,false,false);
		}
		else if(str == "-debug")	// Flag to print debug tree
		{
			parser(filename,true,false,true,false);
		}
		else if(str == "-l")	// Flag to print program source code and output
		{
			parser(filename,false,false,false,true);
		}
		else if(str == "-st")	// Flag to print ST tree and output
		{
			parser(filename,false,false,true,false);
		}
		else
		{
			cout<<"Error in input format"<<endl;
		}
    }
    return 0;
}

