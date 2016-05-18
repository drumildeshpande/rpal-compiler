#include<iostream>
#include<stack>
#include<list>
#include<string>
#include<cstdlib>
#include<cmath>
#include<fstream>
#include<sstream>
#define SIZE 1000
using namespace std;

/* Token Types in RPAL */

enum tokenTypes {IDENTIFIER,INTEGER,OPERATOR,STRING,DELETE,PUNC,KEYWORD};

enum eleTypes {INT,STR,TUPLE,CS,PF,BOOL_T,BOOL_F,NIL,DUMMY,YSTAR,ENV,ETA};

enum primFunc {PRINT,ORDER,STERN,CONC,STEM,ISTUPLE,ISINTEGER,ISSTRING,ISTRUTHVALUE,ITOS};

/* Token structure */

struct token
{
	string value;
	tokenTypes type;
};

/* N-ary Tree, left child , right sibling Tree node structure */

struct ast_node
{
	string name;
	string value;
	ast_node* child;
	ast_node* sibling;
};

class env;
class controlStruct;

class element		// class to store elements on the execution stack
{
	public:
		bool is_conc = false;
		eleTypes type;
		int int_val;
		string str_val;
		env* env_val;
		controlStruct* cs_val;
		queue<element*> Q;
		primFunc func_val;
};

class env		// class to store the environment variables
{
	public:
		unordered_map<string,element*> binding;
		env* parent;
};

class controlStruct			//class to store the control structures on control stack
{
	public:
		int tauN;
		ast_node* node;
		controlStruct* next;
		controlStruct* delta;
		ast_node* bound_var;
		env* cur_env;
		int lambda_id;
};

stack<ast_node*> ast_stack;
stack<controlStruct*> control_stack;
stack<element*> exe_stack;
env* current_env;

/* Function declarations */

void E();
void Ew();
void T();
void Ta();
void Tc();
void B();
void Bt();
void Bs();
void Bp();
void A();
void At();
void Af();
void Ap();
void R();
void Rn();
void D();
void Da();
void Dr();
void Db();
void Vb();
void Vl();
void read(string);
void read(tokenTypes);
token scanner();
void buildTree(string,string,int);
void printTree(ast_node*,int);

// 2nd part of proj
void AST_to_ST(ast_node*);
void ST_for_let(ast_node*&);
void ST_for_where(ast_node*&);
void ST_for_within(ast_node*&);
void ST_for_rec(ast_node*&);
void ST_for_and(ast_node*&);
void ST_for_at(ast_node*&);
void ST_for_func(ast_node*&);
void ST_for_lambda(ast_node*&);
void getCS(ast_node*,controlStruct*);
void printCS(controlStruct*);
void initCSE(controlStruct*);
void executeCSE();
element* findBinding(string,env*);
void getDelta(controlStruct* delta);
void printEle(element* print_ele);
void printTuple(element*);
void printStr(string);

/* KeyWords in RPAL */

string keyWords[20] =  {"let","where","in","fn","and","rec","within","aug","not","or","gr","ge",
						"ls","le","eq","ne","true","false","nil","dummy"};

string lines[SIZE];
int idx_i = 0;
int idx_j = 0;
int cur_n = 0;
bool debug = false;
bool ast = false;
bool debug2 = false;
bool st = false;
bool l_flag = false;
int lambda_cnt=1;
token Ntoken;

/* Scanner Function : Scans one word at a time and converts it to a token.*/

token scanner()
{
    char buffer[1024];
	int i=0,j=0,k;
    char temp;
    for(k=idx_i;k<cur_n;k++)
    {
		if(idx_j >= lines[k].length() && k<cur_n-1)
		{
			idx_j=0;
			idx_i++;
			k = idx_i;
		}
		if(k>idx_i)
		{
			idx_i=k;
			idx_j = j = 0;
		}
		j=idx_j;
		idx_i = k;
		i=0;
		while(j < lines[k].length())
		{       
			temp = lines[k][j];
			while(temp == ' ' || temp == '\t' || temp == '\n') //spaces
			{
				j++;
				temp = lines[k][j];
			}

			if((temp >= 65 && temp <= 90) || (temp >= 97 && temp <= 122))	//identifier: a-z or A-Z
			{
				buffer[i]=temp;
				i++;j++;
				temp = lines[k][j];
				while((temp >= 65 && temp <= 90) || (temp >= 97 && temp <= 122) || (temp >= 48 && temp <= 57) || temp=='_')	// a-z,A-Z,0-9,_
				{
					buffer[i]=temp;
					i++;j++;
					temp = lines[k][j];
				}
				string word(buffer,i);						
				idx_j = j;
				i=0;
				
				// check if the word is Identifier or a keyword
				token t;
				int pos = -1;
				for(int ii=0;ii<20 && pos==-1;ii++)
				{
					if(keyWords[ii].compare(word)==0)
					{
						pos = ii;
					}
				}				
				t.value = word;
				if(pos>=0)
				{
					t.type = KEYWORD;
				}
				else
				{
					t.type = IDENTIFIER;
				}
				return t;
			} 
		
			if(temp >= 48 && temp <= 57)	//integer: 0-9
			{
				buffer[i]=temp;
				i++;j++;
				temp = lines[k][j];
				while(temp >= 48 && temp <= 57)	//0-9
				{
					buffer[i]=temp;
					i++;j++;
					temp = lines[k][j];
				}
				string word(buffer,i);			
				idx_j=j;
				i=0;
				token t;
				t.value = word;
				t.type = INTEGER;
				return t;
			}

			if(temp == '+' || temp == '&' || temp == '.' || temp == '@' || 
				temp == ':' || temp == '=' || temp == '~' || temp == '|' || temp == '$' || temp == '!' || temp == '#' || temp == '%' || 
				temp == '^' || temp == '_' || temp == '[' || temp == ']' || temp == '{' || temp == '}' || temp == '"' ||
				temp == '`' || temp == '?')	// operator symbols
			{
				buffer[i]=temp;
				i++;j++;
				string word(buffer,i);
				idx_j = j;
				i=0;
				token t;
				t.value = word;
				t.type = OPERATOR;
				return t;
			}
		
			if(temp == '(' || temp == ')' || temp == ';' || temp == ',')	//PUNC	
			{
				buffer[i]=temp;
				i++;j++;
				string word(buffer,i);
				idx_j = j;
				i=0;
				token t;
				t.value = word;
				t.type = PUNC;
				return t;
			}
		
			if(temp == '/')	// '/' operator or comments
			{
				char c = temp;
				j++;
				temp = lines[k][j];
				if(temp == '/')	// implies comment
				{
					while(temp != '\n')
					{
						j++;
						temp = lines[k][j];
					}
				}
				else	// implies '/' operator
				{
					buffer[i]=c;
					i++;
					string word(buffer,i);
					idx_j = j;
					i=0;
					token t;
					t.value = word;
					t.type = OPERATOR;
					return t;
				}
			}
		
			if(temp == '*')	// '*' or '**' operator
			{
				char c = temp;
				j++;
				temp = lines[k][j];
				if(temp == '*')	// '**' operator
				{
					buffer[i] = c;
					i++;
					buffer[i] = temp;
					i++;j++;
					string word(buffer,i);
					i=0;
					idx_j = j;
					token t;
					t.value = word;
					t.type = OPERATOR;
					return t;
				}
				else	// '*' operator
				{
					buffer[i]=c;
					i++;
					string word(buffer,i);
					idx_j = j;
					i=0;
					token t;
					t.value = word;
					t.type = OPERATOR;
					return t;
				}
			}
		
			if(temp == '>' || temp == '<')	// '<' or '>' or '<=' or '>='
			{
				char c = temp;
				j++;
				temp = lines[k][j];
				if(temp == '=')	// '<= or >=' operator
				{
					buffer[i] = c;i++;
					buffer[i] = temp;
					i++;j++;
					string word(buffer,i);
					idx_j = j;
					i=0;
					token t;
					t.value = word;
					t.type = OPERATOR;
					return t;
				}
				else
				{
					buffer[i]=c;
					i++;
					string word(buffer,i);
					i=0;
					token t;
					t.value = word;
					t.type = OPERATOR;				
					return t;
				}
			}

			if(temp == '-')	// -> operator
			{
				char c = temp;
				j++;
				temp = lines[k][j];
				if(temp == '>')
				{
					buffer[i] = c;i++;
					buffer[i] = temp;
					i++;j++;
					string word(buffer,i);
					idx_j = j;
					i=0;
					token t;
					t.value = word;
					t.type = OPERATOR;
					return t;
				}
				else
				{
					buffer[i] = c;i++;
					string word(buffer,i);
					idx_j = j;
					i=0;
					token t;
					t.value = word;
					t.type = OPERATOR;
					return t;
				}
			}

			if(temp == '\'')	//strings - string begins
			{
				j++;
				temp = lines[k][j];
				
				while(temp != '\'')	// Till string ends
				{
					if(temp == '\\')	// can be escape char or '\'
					{
						char c = temp;
						j++;
						temp = lines[k][j];
						if(temp != '\'')	// it was \ char
						{
							buffer[i] = c;
							i++;
							buffer[i] = temp;
						}
						else	// it was escape char
						{
							buffer[i] = temp;
							i++;
							j++;
							temp = lines[k][j];
						}
					}
					else
					{
						buffer[i] = temp;
						i++;j++;
						temp = lines[k][j];
					}
				}
				string word(buffer,i);
				idx_j = j+1;
				i=0;
				token t;
				t.value = word;
				t.type = STRING;
				return t;
			}
		}
    }
	token t;
	t.value = " ";
	t.type = DELETE;
	return t;
}	

void parser(string fname,bool d,bool a,bool stb,bool l)
{
	int i=0;
	//debug = d;
	debug2 = d;
	ast = a;
	st = stb;
	l_flag= l;
	ifstream fin(fname.c_str());
	while(!fin.eof() && i<SIZE)
	{
		if(i!=0 && l)
			cout<<endl;
		getline(fin,lines[i]);
		if(debug || l)		// if -l flag on print the program
		{
			cout<<lines[i];
		}
		i++;
	}
	cur_n = i-1;
	Ntoken = scanner(); 
	E();
	if(debug)
	{
		cout<<"stack size:"<<ast_stack.size()<<endl;
	}
	if(ast)
	{
		printTree(ast_stack.top(),0);
	}
	ast_node* root = ast_stack.top();
	if(debug2)
		printTree(root,0);
	AST_to_ST(root);
	//cout<<"Standardised tree is :"<<endl;
	if(st)
		printTree(root,0);
	controlStruct CS;
	getCS(root,&CS);
	if(debug2)
		printCS(CS.next);
	initCSE(CS.next);
	cout<<endl;
}

/* Read Function: Reads Next token */

void read(string cur_t_val)
{	
	if(debug)
	{
		cout<<"in read str"<<endl;
		cout<<Ntoken.value;	
	}
	if(Ntoken.value != cur_t_val && debug)
	{
		cout<<"Error: Expected token->"<<cur_t_val<<", but found->"<<Ntoken.value<<endl;
		exit(0);
	}	
	if(Ntoken.type == INTEGER || Ntoken.type == IDENTIFIER || Ntoken.type == STRING)
	{
		if(debug)
		{
			cout<<"###########buildTree(Ntoken,0)"<<endl;
		}
		if(Ntoken.type == INTEGER)
		{
			buildTree("INT",Ntoken.value,0);
		}
		if(Ntoken.type == IDENTIFIER)
		{
			buildTree("ID",Ntoken.value,0);
		}
		if(Ntoken.type == STRING)
		{
			buildTree("STR",Ntoken.value,0);
		}
	}
	Ntoken = scanner();	
	if(debug)
	{
		cout<<"scanner out: "<<Ntoken.value<<" , "<<Ntoken.type<<endl;
	}
}

void read(tokenTypes cur_t_type)	// overloaded read()
{	
	if(debug)
	{
		cout<<Ntoken.value;
		cout<<"in read type"<<endl;
	}
	if(Ntoken.type != cur_t_type && debug)
	{
		cout<<"Error: Expected token->"<<cur_t_type<<", but found->"<<Ntoken.type<<endl;
		exit(0);
	}	
	if(Ntoken.type == INTEGER || Ntoken.type == IDENTIFIER || Ntoken.type == STRING)
	{
		if(debug)
		{
			cout<<"###########buildTree(Ntoken,0)"<<endl;
		}
		if(Ntoken.type == INTEGER)
		{
			string s="INT";
			buildTree(s,Ntoken.value,0);
		}
		if(Ntoken.type == IDENTIFIER)
		{
			string s="ID";
			buildTree(s,Ntoken.value,0);
		}
		if(Ntoken.type == STRING)
		{
			string s="STR";
			if(Ntoken.value == "")
			{
				//cout<<"val:"<<Ntoken.value<<":";
				string temp = "\'\'";
				buildTree(s,temp,0);
			}
			else
			{
				buildTree(s,Ntoken.value,0);
			}
		}
	}	
	Ntoken = scanner();
	if(debug)
	{	
		cout<<"scanner out: "<<Ntoken.value<<" , "<<Ntoken.type<<endl;
	}
}


/* Start of Functions for Non teminals in the grammer */

void E()
{
	if(debug)
	{
		cout<<"in E()"<<endl;
		cout<<Ntoken.value;
	}
	int n=0;
	if(Ntoken.value == "let" && Ntoken.type == KEYWORD)
	{
		read("let");
		D();
		read("in");
		E();
		if(debug)
		{
			cout<<"###########buildTree(let,2)"<<endl;
		}
		buildTree("let","let",2);
	}
	else if(Ntoken.value == "fn" && Ntoken.type == KEYWORD)
	{
		read("fn");
		while((Ntoken.value == "(" && Ntoken.type == PUNC) || (Ntoken.type == IDENTIFIER))
		{
			n++;
			Vb();
		}
		read(".");
		E();
		if(debug)
		{
			cout<<"###########buildTree(lambda,n+1)"<<n+1<<endl;
		}
		buildTree("lambda","lambda",n+1);
	}
	else
		Ew();
}

void Ew()
{
	if(debug)
	{
		cout<<"in Ew()"<<endl;
		cout<<Ntoken.value;
	}
	T();
	if(Ntoken.value == "where" && Ntoken.type == KEYWORD)
	{
		read("where");
		Dr();
		if(debug)
		{
			cout<<"###########buildTree(where,2)"<<endl;
		}
		buildTree("where","where",2);
	}
}

void T()
{
	if(debug)
	{
		cout<<"in T()"<<endl;
		cout<<Ntoken.value;
	}
	int n=0;
	Ta();
	while(Ntoken.value == "," && Ntoken.type == PUNC)
	{
		n++;
		read(",");
		Ta();
	}
	if(n>0)
	{
		if(debug)
		{
			cout<<"###########buildTree(tau,n+1)"<<n+1<<endl;
		}
		buildTree("tau","tau",n+1);
	}
}

void Ta()
{
	if(debug)
	{
		cout<<"in Ta()"<<endl;
		cout<<Ntoken.value;
	}
	Tc();
	while(Ntoken.value == "aug" && Ntoken.type == KEYWORD)
	{
		read("aug");
		Tc();
		if(debug)
		{
			cout<<"###########buildTree(aug,2)"<<endl;
		}
		buildTree("aug","aug",2);	
	}
}

void Tc()
{
	if(debug)
	{
		cout<<"in Tc()"<<endl;
	}
	int n=0;
	B();
	while(Ntoken.value == "->" && Ntoken.type == OPERATOR)
	{
		read("->");
		Tc();
		read("|");
		Tc();
		if(debug)
		{
			cout<<"###########buildTree(->,3)"<<endl;
		}
		buildTree("->","->",3);
	}
}

void B()
{
	if(debug)
	{
		cout<<"in B()"<<endl;
	}
	Bt();
	while(Ntoken.value == "or" && Ntoken.type == KEYWORD)
	{
		read("or");
		Bt();
		if(debug)
		{
			cout<<"###########buildTree(or,2)"<<endl;
		}
		buildTree("or","or",2);
	}
}

void Bt()
{
	if(debug)
	{
		cout<<"in Bt()"<<endl;
		cout<<Ntoken.value;
	}
	Bs();
	while(Ntoken.value == "&" && Ntoken.type == OPERATOR)
	{
		read("&");
		Bs();
		if(debug)
		{
			cout<<"###########buildTree(&,2)"<<endl;
		}
		buildTree("&","&",2);	
	}
}

void Bs()
{
	if(debug)
	{
		cout<<"in Bs()"<<endl;
		cout<<Ntoken.value;
	}
	if(Ntoken.value == "not" && Ntoken.type == KEYWORD)	
	{
		read("not");
		Bp();
		if(debug)
		{
			cout<<"###########buildTree(not,1)"<<endl;
		}
		buildTree("not","not",1);
	}
	else
	{	
		Bp();
	}
}

void Bp()
{
	if(debug)
	{
		cout<<"in Bp()"<<endl;
		cout<<Ntoken.value;
	}
	A();
	if((Ntoken.value == "gr" && Ntoken.type == KEYWORD) || (Ntoken.value == ">" && Ntoken.type == OPERATOR))
	{
		read(Ntoken.value);
		A();
		if(debug)
		{
			cout<<"###########buildTree(gr,2)"<<endl;
		}
		buildTree("gr","gr",2);
	}
	else if((Ntoken.value == "ge" && Ntoken.type == KEYWORD) || (Ntoken.value == ">=" && Ntoken.type == OPERATOR))
	{
		read(Ntoken.value);
		A();
		if(debug)
		{
			cout<<"###########buildTree(ge,2)"<<endl;
		}
		buildTree("ge","ge",2);
	}
	else if((Ntoken.value == "ls" && Ntoken.type == KEYWORD) || (Ntoken.value == "<" && Ntoken.type == OPERATOR))
	{
		read(Ntoken.value);
		A();
		if(debug)
		{
			cout<<"###########buildTree(ls,2)"<<endl;
		}		
		buildTree("ls","ls",2);
	}
	else if((Ntoken.value == "le" && Ntoken.type == KEYWORD) || (Ntoken.value == "<=" && Ntoken.type == OPERATOR))
	{
		read(Ntoken.value);
		A();
		if(debug)
		{
			cout<<"###########buildTree(le,2)"<<endl;
		}
		buildTree("le","le",2);
	}
	else if(Ntoken.value == "eq" && Ntoken.type == KEYWORD)
	{
		read("eq");
		A();
		if(debug)
		{
			cout<<"###########buildTree(eq,2)"<<endl;
		}
		buildTree("eq","eq",2);
	}
	else if(Ntoken.value == "ne" && Ntoken.type == KEYWORD)
	{
		read("ne");
		A();
		if(debug)
		{
			cout<<"###########buildTree(ne,2)"<<endl;
		}
		buildTree("ne","ne",2);
	}
}

void A()
{
	if(debug)
	{
		cout<<"in A()"<<endl;
		cout<<Ntoken.value;
	}
	if(Ntoken.value == "+" && Ntoken.type == OPERATOR)
	{
		read("+");
		At();
	}
	else if(Ntoken.value == "-" && Ntoken.type == OPERATOR)
	{
		read("-");
		At();
		if(debug)
		{
			cout<<"###########buildTree(neg,1)"<<endl;
		}
		buildTree("neg","neg",1);
	}
	else
	{
		At();
		while((Ntoken.value == "+" && Ntoken.type == OPERATOR) || (Ntoken.value == "-" && Ntoken.type == OPERATOR))
		{
			string tempToken = Ntoken.value;
			read(Ntoken.value);
			At();		
			if(debug)
			{
				cout<<"###########buildTree(Ntoken.value,2)"<<endl;
			}
			if(tempToken == "+")
			{
				buildTree("+","+",2);
			}
			else
			{
				buildTree("-","-",2);
			}
			
		}
	}
	
}

void At()
{
	if(debug)
	{
		cout<<"in At()"<<endl;
		cout<<Ntoken.value;
	}
	Af();
	while((Ntoken.value == "*" && Ntoken.type == OPERATOR) || (Ntoken.value == "/" && Ntoken.type == OPERATOR))
	{
		string tempToken = Ntoken.value;
		read(Ntoken.value);
		Af();
		if(debug)
		{
			cout<<"###########buildTree(Ntoken.value,2)"<<endl;
			//cout<<"yyy:"<<Ntoken.value;
		}
		if(tempToken == "*")
		{
			//cout<<"dfdfdf";
			buildTree("*","*",2);
		}
		else
		{
			buildTree("/","/",2);
		}
	}
}

void Af()
{
	if(debug)
	{
		cout<<"in Af()"<<endl;
		cout<<Ntoken.value;
	}
	Ap();
	if(Ntoken.value == "**" && Ntoken.type == OPERATOR)
	{
		read("**");
		Af();
		if(debug)
		{
			cout<<"###########buildTree(**,2)"<<endl;
		}
		buildTree("**","**",2);
	}
}

void Ap()
{
	if(debug)
	{
		cout<<"in Ap()"<<endl;
		cout<<Ntoken.value;
	}
	R();
	while(Ntoken.value == "@" && Ntoken.type == OPERATOR)
	{
		read("@");
		read(IDENTIFIER);
		R();
		if(debug)
		{
			cout<<"###########buildTree(@,3)"<<endl;
		}
		buildTree("@","@",3);
	}
}

void R()
{
	if(debug)
	{
		cout<<"in R()"<<endl;
		cout<<Ntoken.value;
	}
	Rn();
	while((Ntoken.type == IDENTIFIER) ||
		  (Ntoken.type == INTEGER) ||
		  (Ntoken.type == STRING) ||
		  (Ntoken.value == "true" && Ntoken.type == KEYWORD) ||
		  (Ntoken.value == "false" && Ntoken.type == KEYWORD) ||
		  (Ntoken.value == "nil" && Ntoken.type == KEYWORD) ||
		  (Ntoken.value == "(" && Ntoken.type == PUNC) ||
		  (Ntoken.value == "dummy" && Ntoken.type == KEYWORD))
	{
		Rn();
		if(debug)
		{
			cout<<"###########buildTree(gamma,2)"<<endl;
		}
		buildTree("gamma","gamma",2);
	}
}

void Rn()
{
	if(debug)
	{
		cout<<"in Rn()"<<endl;
		cout<<Ntoken.value;
	}
	if(Ntoken.type == IDENTIFIER)
	{
		read(IDENTIFIER);
	}
	else if(Ntoken.type == INTEGER)
	{
		read(INTEGER);
	}
	else if(Ntoken.type == STRING)
	{
		read(STRING);
	}
	else if(Ntoken.value == "true" && Ntoken.type == KEYWORD)
	{
		read("true");
		if(debug)
		{
			cout<<"###########buildTree(true,0)"<<endl;
		}
		buildTree("true","true",0);
	}
	else if(Ntoken.value == "false" && Ntoken.type == KEYWORD)
	{
		read("false");
		if(debug)
		{
			cout<<"###########buildTree(false,0)"<<endl;
		}
		buildTree("false","false",0);
	}
	else if(Ntoken.value == "nil" && Ntoken.type == KEYWORD)
	{
		read("nil");
		if(debug)
		{
			cout<<"###########buildTree(nil,0)"<<endl;
		}
		buildTree("nil","nil",0);
	}
	else if(Ntoken.value == "(" && Ntoken.type == PUNC)
	{
		read("(");
		E();
		read(")");
	}
	else if(Ntoken.value == "dummy" && Ntoken.type == KEYWORD)
	{
		read("dummy");
		if(debug)
		{
			cout<<"###########buildTree(dummy,0)"<<endl;
		}
		buildTree("dummy","dummy",0);
	}
}

void D()
{
	if(debug)
	{
		cout<<"in D()"<<endl;
		cout<<Ntoken.value;
	}
	Da();
	if(Ntoken.value == "within" && Ntoken.type == KEYWORD)
	{
		read("within");
		D();
		if(debug)
		{
			cout<<"###########buildTree(within,2)"<<endl;
		}
		buildTree("within","within",2);
	}
}

void Da()
{
	if(debug)
	{
		cout<<"in Da()"<<endl;
		cout<<Ntoken.value;
	}
	int n=0;
	Dr();
	while(Ntoken.value == "and" && Ntoken.type == KEYWORD)
	{
		n++;
		read("and");
		Dr();
	}
	if(n>0)
	{
		if(debug)
		{
			cout<<"###########buildTree(and,n+1)"<<n+1<<endl;
		}
		buildTree("and","and",n+1);
	}
}

void Dr()
{
	if(debug)
	{
		cout<<"in Dr()"<<endl;
		cout<<Ntoken.value;
	}
	if(Ntoken.value == "rec" && Ntoken.type == KEYWORD)
	{
		read("rec");
		Db();
		if(debug)
		{
			cout<<"###########buildTree(rec,1)"<<endl;
		}
		buildTree("rec","rec",1);
	}
	else
	{
		Db();
	}
}

void Db()
{
	if(debug)
	{
		cout<<"in Db()"<<endl;
		cout<<Ntoken.value;
	}
	if(Ntoken.value == "(" && Ntoken.type == PUNC)	// thrid rule
	{
		read("(");
		D();
		read(")");
	}
	else if(Ntoken.type == IDENTIFIER)	// either first or second rule
	{
		if(debug)
		{
			cout<<Ntoken.value;
		}
		read(IDENTIFIER);
		if((Ntoken.value == "," && Ntoken.type == PUNC) || (Ntoken.value == "=" && Ntoken.type == OPERATOR))	// first rule
		{
			Vl();
			read("=");
			E();
			if(debug)
			{
				cout<<"###########buildTree(=,2)"<<endl;
			}
			buildTree("=","=",2);
		}
		else
		{
			int n=0;
			while((Ntoken.type == IDENTIFIER) || (Ntoken.value == "(" && Ntoken.type == PUNC))
			{
				n++;
				Vb();
			}
			read("=");
			E();
			if(debug)
			{
				cout<<"###########buildTree(fcn_form,n+2)"<<n+2<<endl;
			}
			buildTree("function_form","fcn_form",n+2);
		}
	}
}

void Vb()
{
	if(debug)
	{
		cout<<"in Vb()"<<endl;
		cout<<Ntoken.value;
	}
	if(Ntoken.type == IDENTIFIER)
	{
		read(IDENTIFIER);
	}
	else
	{
		read("(");
		if(Ntoken.value == ")" && Ntoken.type == PUNC)
		{
			read(")");
			if(debug)
			{
				cout<<"###########buildTree((),2)"<<endl;
			}
			buildTree("()","()",2);
		}
		else
		{
			read(IDENTIFIER);
			Vl();
			read(")");
		}
	}
}

void Vl()
{
	if(debug)
	{
		cout<<"in Vl()"<<endl;
		cout<<Ntoken.value;
	}
	int n=0;
	while(Ntoken.value == "," && Ntoken.type == PUNC)	// start from , as 1st ID was read in Db()
	{
		n++;
		read(",");
		read(IDENTIFIER);
	}
	if(n>0)	// more than one ID
	{
		if(debug)
		{
			cout<<"###########buildTree(,,n+1):"<<n+1<<endl;	// +1 as one ID was read earlier
		}
		buildTree(",",",",n+1);
	}
}

/* --- End of Functions for Non teminals in the grammer --- */


/* Function to Build Tree Nodes */

void buildTree(string n,string value,int numChild)
{
	ast_node* new_node = new ast_node();
	new_node->name = n;
	new_node->value = value;
	//cout<<"node val:"<<value<<":"<<endl;
	new_node->child = NULL;
	new_node->sibling = NULL;
	ast_node* child = new ast_node();
	for(int i=1;i<=numChild;i++)
	{
		ast_node* tmp = ast_stack.top();
		//cout<<"child val:"<<tmp->value<<":"<<endl;
		ast_stack.pop();
		if(child == NULL)	// get first child
		{
			child = tmp;
		}				
		else	// create a list of siblings
		{
			tmp->sibling = child;
			child = tmp;
		}
	}
	new_node->child = child;
	ast_stack.push(new_node);
}


/* Function to print AST */

void printTree(ast_node* root,int numDots)
{
	//cout<<"Printing tree"<<endl;
	if(root->value!="")
	{
		for(int i=0;i<numDots;i++)
		{
			cout<<".";
		}
		if(root->name == "STR")
		{
			if(root->value == "\'\'")
				cout<<"<STR:"<<root->value<<">"<<endl;
			else
				cout<<"<STR:'"<<root->value<<"'>"<<endl;
		}
		else if(root->name == "ID")
		{
			cout<<"<ID:"<<root->value<<">"<<endl;
		}
		else if(root->name == "INT")
		{
			cout<<"<INT:"<<root->value<<">"<<endl;
		}
		else if(root->name == "function_form")
		{
			cout<<"function_form"<<endl;
		}
		else if(root->name == "true")
		{
			cout<<"<true>"<<endl;
		}
		else if(root->name == "false")
		{
			cout<<"<false>"<<endl;
		}
		else if(root->name == "dummy")
		{
			cout<<"<dummy>"<<endl;
		}
		else if(root->name == "nil")
		{
			cout<<"<nil>"<<endl;
		}
		else if(root->name == "Y*")
		{
			cout<<"<Y*>"<<endl;
		}
		else
		{
			cout<<root->value<<endl;
		}
		if(root->child != NULL)
		{
			//cout<<"xxx"<<endl;
			printTree(root->child,numDots+1);
		}
		if(root->sibling != NULL)
		{
			printTree(root->sibling,numDots);	
		}
	}
}

/* Function wrapper to standardize the AST tree */

void AST_to_ST(ast_node* root)
{
	if(root->child != NULL)
	{
		AST_to_ST(root->child);
	}
	if(root->sibling!=NULL)
	{
		AST_to_ST(root->sibling);
	}
	if(root->name == "let")
	{
		ST_for_let(root);
	}
	if(root->name == "where")
	{
		ST_for_where(root);
	}
	if(root->name == "within")
	{
		ST_for_within(root);
	}
	if(root->name == "and")
	{
		ST_for_and(root);
	}
	if(root->name == "rec")
	{
		ST_for_rec(root);
	}
	if(root->name == "@")
	{
		ST_for_at(root);
	}
	if(root->name == "function_form")
	{
		ST_for_func(root);
	}
	if(root->name == "lambda")
	{
		ST_for_lambda(root);
	}
}

/* Standardize the LET node */
/* 	LET				gamma
	/ \				/	\
	=  p	=>	lambda	E
   /\  			/	\
   X E		    X   P
*/

void ST_for_let(ast_node*& root)
{
	if(debug2)
		cout<<"in st for let"<<endl;
	if(root->child != NULL && root->child->value!= "" && root->child->sibling != NULL && root->child->sibling->value!="")
	{
		ast_node* P = root->child->sibling;
		root->name = "gamma";
		root->value = "gamma";
		ast_node* eq = root->child;
		if(P!= NULL)
		{
			if(root->child->name == "=")
			{
				eq->name = "lambda";
				eq->value = "lambda";
				ast_node* X = eq->child;
				ast_node* E = X->sibling;
				if(X != NULL && E!=NULL)
				{
					root->child->sibling = E;
					X->sibling = P;
					E->sibling = NULL;
				}
			}
		}
	}
}

/* Standardize the WHERE node */
/* 	WHERE			gamma
	/ \				/	\
	P  =	=>	lambda	E
   	  /\ 		/	\
      X E		X   P
*/

void ST_for_where(ast_node*& root)
{
	if(debug2)
		cout<<"in st for where"<<endl;
	if(root->child !=NULL && root->child->value!= "" && root->child->sibling!=NULL && root->child->sibling->value!="")
	{
		ast_node* P = root->child;
		root->name = "gamma";
		root->value = "gamma";
		ast_node* eq = root->child->sibling;
		if(eq->name == "=")
		{
			eq->name = "lambda";
			eq->value = "lambda";
			if(eq->child!=NULL && eq->child->sibling!=NULL)
			{
				ast_node* E = eq->child->sibling;
				ast_node* X = eq->child;
				root->child = eq;
				eq->sibling = E;
				X->sibling = P;
				P->sibling = NULL;
			}
		}
	}
}

/* Standardize the FCN_FORM node */
/* 	FCN_FORM			  =
	/ |	 \				/	\
	p V+  E		=>		P lambda
    					   /   \
  			    		   V   .E
*/


void ST_for_func(ast_node*& root)
{
	if(debug2)
		cout<<"in st for func"<<endl;
	ast_node* P = root->child;
	ast_node* V = P->sibling;
	root->name = "=";
	root->value = "=";
	while(V->sibling != NULL && V->sibling->value != "")
	{
		ast_node* temp = new ast_node;
		temp->name = "lambda";
		temp->value = "lambda";
		temp->sibling = NULL;	//
		temp->child = V;
		P->sibling = temp;
		P=V;
		V = V->sibling;
	}
	P->sibling = V;
}

/* Standardize the WITHIN node */
/* 	WITHIN			=
	/    \		   /  \
   =     =	=>	   X gamma
  / \	/ \ 		 /	 \
 X1 E1 X2 E2	  lambda  E1
				   /  \
				  X1  E2
*/

void ST_for_within(ast_node*& root)
{
	if(debug2)
		cout<<"in st for within"<<endl;
	if(root->child!= NULL && root->child->sibling!=NULL)
	{
		if(root->child->name == "=" && root->child->sibling->name == "=")
		{
			ast_node* X1 = root->child->child;
			ast_node* X2 = root->child->sibling->child;
			if(X1 !=NULL && X2!=NULL)
			{
				ast_node* E1 = X1->sibling;
				ast_node* E2 = X2->sibling;
				root->name = "=";
				root->value = "=";
				root->child = X2;
				ast_node* gamma = new ast_node;
				ast_node* lambda = new ast_node;				
				gamma->name = "gamma";
				gamma->value = "gamma";
				lambda->name = "lambda";
				lambda->value = "lambda";
				X2->sibling = gamma;
				gamma->child = lambda;
				lambda->sibling = E1;
				lambda->child = X1;
				X1->sibling = E2;
			}
		} 
	}
}

/* Standardize the @ node */
/* 	  @				gamma
	/ | \			/	\
   E1 N E2	=>	gamma	E2
   	  	 		/	\
      			N   E1
*/

void ST_for_at(ast_node*& root)
{
	if(debug2)
		cout<<"in st for at"<<endl;
	if(root->child!=NULL && root->child->sibling!=NULL)
	{
		ast_node* E1 = root->child;
		ast_node* N = E1->sibling;
		ast_node* E2 = N->sibling;
		if(E2!=NULL)
		{
			root->name = "gamma";
			root->value = "gamma";
			ast_node* gamma = new ast_node;
			gamma->name = "gamma";
			gamma->value = "gamma";
			root->child = gamma;
			gamma->sibling = E2;
			gamma->child = N;
			N->sibling = E1;
			E1->sibling = NULL;
		}
	}
}

/* Standardize the AND node */
/* 	AND			  =
	 |			/	\
	=++	=>		,	TAU
   	/ \ 	   	|	 |
    X E		    X++  E++
*/

void ST_for_and(ast_node*& root)
{
	if(debug2)
		cout<<"in st for and"<<endl;
	if(root->child!=NULL)
	{
		ast_node* eq = root->child;
		ast_node* comma = new ast_node;
		ast_node* tau = new ast_node;		
		comma->name = ",";
		comma->value = ",";
		tau->name = "tau";
		tau->value = "tau";
		root->name = "=";
		root->value = "=";
		
		root->child = comma;
		comma->sibling = tau;
		comma->child = eq->child;
		tau->child = comma->child->sibling;
		
		comma = comma->child;
		tau = tau->child;
		eq = eq->sibling;
		while(eq!=NULL && eq->value!="")
		{
			if(debug2)
				cout<<"eq name val"<<eq->name<<","<<eq->value<<endl;
			comma->sibling = eq->child;
			tau->sibling = eq->child->sibling;
			
			comma = comma->sibling;
			tau = tau->sibling;
			eq = eq->sibling;
		}	
		comma->sibling = NULL;
		tau->sibling = NULL;
	}
}

/* Standardize the REC node */
/* 	REC			  =
	 |			/	\
	 =	=>		X  gamma
   	/ \ 	   	  /     \
    X E		     Ystar lambda
						/  \
					    X  E
*/


void ST_for_rec(ast_node*& root)
{
	if(debug2)
		cout<<"in st for rec"<<endl;
	if(root->child!=NULL)
	{
		root->name = "=";
		root->value = "=";
		ast_node* eq = root->child;
		if(eq!=NULL )
		{
			ast_node* X = eq->child;
			ast_node* E = X->sibling;
			root->child = X;
			ast_node* gamma = new ast_node;
			ast_node* Ystar = new ast_node;
			ast_node* lambda = new ast_node;
			ast_node* X1 = new ast_node;

			gamma->name = "gamma";
			gamma->value = "gamma";
			Ystar->name = "Y*";
			Ystar->value = "Y*";
			lambda->name = "lambda";
			lambda->value = "lambda";
			X1->name = X->name;
			X1->value = X->value;
			X1->child = X->child;

			X->sibling = gamma;
			gamma->child = Ystar;
			Ystar->sibling = lambda;
			lambda->child = X1;
			X1->sibling = E;			
		}
	}
}

/* Standardize the Multi-fucn param (lambda) node */
/* 	lambda			  ++lambda
	/ 	\			    /  \
	V++ E		=>	   V   .E		    		 
*/


void ST_for_lambda(ast_node*& root)
{
	if(root->child!=NULL)
	{
		ast_node* V = root->child;
		ast_node* temp = V->sibling;
		while(temp->sibling!=NULL && temp->sibling->value!="")
		{
			ast_node* lambda = new ast_node;
			lambda->name = "lambda";
			lambda->value = "lambda";
			V->sibling = lambda;
			lambda->child = temp;
			V = temp;
			temp = temp->sibling;
		}
	}
}

/* Function to generate all the control structures (deltas) */

void getCS(ast_node* root,controlStruct* CS)
{
	//cout<<"hello"<<endl;
	if(root->name == "lambda")
	{
		controlStruct* nextCS = new controlStruct;
		nextCS->node = root;
		ast_node* sib = root->child->sibling;
		if(root->child->name != ",")
		{
			nextCS->bound_var = root->child;
			nextCS->bound_var->sibling = NULL;
		}
		else
		{
			nextCS->bound_var = root->child->child;
		}
		CS->next = nextCS;
		CS = CS->next;
		CS->lambda_id = lambda_cnt;
		lambda_cnt++;
		controlStruct* newCS = new controlStruct;
		getCS(sib,newCS);
		CS->delta = newCS->next;
		if(root->sibling!=NULL && root->sibling->value!="")
		{
			getCS(root->sibling,CS);
		}
	}
	else if(root->name == "->")
	{
		ast_node* B = root->child;
		ast_node* E1 = B->sibling;
		ast_node* E2 = E1->sibling;
		B->sibling=NULL;
		E1->sibling = NULL;
		
		ast_node* delta_then = new ast_node;
		delta_then->name = "delta_then";
		delta_then->value = "delta_then";
		controlStruct* nextCS = new controlStruct;
		nextCS->node = delta_then;
		CS->next = nextCS;
		CS = CS->next;		
		controlStruct* temp = new controlStruct;
		getCS(E1,temp);
		CS->delta = temp->next;				
				
		ast_node* delta_else = new ast_node;
		delta_else->name = "delta_else";
		delta_else->value = "delta_else";
		controlStruct* nextCS1 = new controlStruct;
		nextCS1->node = delta_else;
		CS->next = nextCS1;
		CS = CS->next;
		controlStruct* temp1 = new controlStruct;
		getCS(E2,temp1);
		CS->delta = temp1->next;

		ast_node* beta = new ast_node;
		beta->name = "beta";
		beta->value = "beta";
		controlStruct* betaCS = new controlStruct;
		betaCS->node = beta;
		CS->next = betaCS;
		CS = CS->next;

		controlStruct* CSforB = new controlStruct;
		getCS(B,CSforB);
		CS->next = CSforB->next;
		while(CS->next!=NULL)
		{
			CS = CS->next;
		}
		if(root->sibling!=NULL && root->sibling->value!="")
		{
			getCS(root->sibling,CS);
		}
	}
	else if(root->name == "tau")
	{
		int numChildren = 0;
		controlStruct* nextCS = new controlStruct;
		nextCS->node = root;
		CS->next = nextCS;
		CS = CS->next;
		ast_node* temp = root->child;
		getCS(temp,CS);
		while(temp!=NULL && temp->value!="")
		{
			temp = temp->sibling;
			numChildren++;
		}		
		CS->tauN = numChildren;
		while(CS->next!=NULL)
		{
			CS = CS->next;
		}
		if(root->sibling!=NULL && root->sibling->value!="")
		{
			getCS(root->sibling,CS);
		}
	}
	else
	{
		controlStruct* nextCS = new controlStruct;
		nextCS->node = root;
		CS->next = nextCS;
		CS = CS->next;
		if(debug2)
			cout<<root->name<<":"<<root->value<<endl;
		if(root->child!=NULL && root->child->value!="")
		{
			getCS(root->child,CS);
		}
		while(CS->next!=NULL)
		{
			CS = CS->next;
		}
		if(root->sibling!=NULL && root->sibling->value!="")
		{
			getCS(root->sibling,CS);
		}	
	}
}

void printCS(controlStruct* CS)
{
	while(CS!=NULL)
	{
		cout<<CS->node->name<<":"<<CS->node->value<<endl;
		ast_node* it = CS->bound_var;
		while(it!=NULL && it->value!="")
		{
			cout<<it->name<<":"<<it->value<<",";
			it=it->sibling;
		}
		cout<<endl;
		if(CS->delta!=NULL)
		{
			printCS(CS->delta);
		}
		CS = CS->next;
	}
}

/* Initiate the CSE machine before Execution */

void initCSE(controlStruct* CS)
{
	current_env = new env;
	//env* new_env = new env;
	current_env->parent = NULL;
	
	element* exe_ele = new element;
	exe_ele->type = ENV;
	exe_ele->env_val = current_env;
	exe_stack.push(exe_ele);
	
	ast_node* env_node = new ast_node;
	env_node->name = "env";
	env_node->value = "env";
	controlStruct* newCS = new controlStruct;
	newCS->node = env_node;
	newCS->cur_env = current_env;
	control_stack.push(newCS);
	lambda_cnt=1;
	while(CS!=NULL)
	{
		if(debug2)
			cout<<"pushing->"<<CS->node->name<<":"<<CS->node->value<<endl;
		control_stack.push(CS);
		CS = CS->next;
	}
	executeCSE();
}

/* Function to execute the CSE machine */

void executeCSE()
{
	while(!control_stack.empty())
	{
		controlStruct* topCS = control_stack.top();
		control_stack.pop();
		if(debug2)
			cout<<"cs on top: "<<topCS->node->name<<":"<<topCS->node->value<<endl;
		
		if(topCS->node->name == "INT")
		{
			element* new_ele = new element;
			new_ele->type = INT;
			int val = atoi(topCS->node->value.c_str());
			//cout<<"int value in execse:int"<<val<<endl;
			new_ele->int_val = val;
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "STR")
		{
			element* new_ele = new element;
			new_ele->type = STR;
			new_ele->str_val = topCS->node->value;
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "nil")
		{
			element* new_ele = new element;
			new_ele->type = NIL;
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "true")
		{
			element* new_ele = new element;
			new_ele->type = BOOL_T;
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "false")
		{
			element* new_ele = new element;
			new_ele->type = BOOL_F;
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "dummy")
		{
			element* new_ele = new element;
			new_ele->type = DUMMY;
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "ID")		/* RULE 1 : Name of top of Control stack */
		{
			if(debug2)
				cout<<"in ID"<<endl;
			element* new_ele = findBinding(topCS->node->value,current_env);
			if(new_ele != NULL)
			{
				exe_stack.push(new_ele);
			}
			else
			{
				element* new_ele = new element;
				new_ele->type = PF;
				if(topCS->node->value == "Print")
				{
					new_ele->func_val = PRINT;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Isstring")
				{
					new_ele->func_val = ISSTRING;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Isinteger")
				{
					new_ele->func_val = ISINTEGER;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Istruthvalue")
				{
					new_ele->func_val = ISTRUTHVALUE;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Istuple")
				{
					new_ele->func_val = ISTUPLE;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Order")
				{
					new_ele->func_val = ORDER;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Stern")
				{
					new_ele->func_val = STERN;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Stem")
				{
					new_ele->func_val = STEM;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "Conc")
				{
					new_ele->func_val = CONC;
					exe_stack.push(new_ele);
				}
				else if(topCS->node->value == "ItoS")
				{
					new_ele->func_val = ITOS;
					exe_stack.push(new_ele);
				}
				else
				{
					cout<<"error: ID read is not a primitive function"<<endl;
				}
			}
			
		}
		else if(topCS->node->name == "lambda")		/* RULE 2: Lambda on top of control stack */
		{
			topCS->cur_env = current_env;
			element* new_ele = new element;
			new_ele->type = CS;
			new_ele->cs_val = topCS;
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "gamma")		/* Rule 3,4,10,11,12,13: Gamma on top of control stack */
		{
			if(exe_stack.top()->type == CS)			/* Rule 4,11: Gamma on top of control stack and lambda on exe stack */
			{
				element* top_ele = exe_stack.top();
				exe_stack.pop();
				env* new_env = new env;
				new_env->parent = top_ele->cs_val->cur_env;
				if(top_ele->cs_val->bound_var!=NULL && top_ele->cs_val->bound_var->value!="")
				{
					element* bind_ele = exe_stack.top();
					exe_stack.pop();
					if(bind_ele->type == TUPLE)		/* Rule 11: Gamma on top of control stack and lambda(n-ary) on exe stack */
					{
						//cout<<"dsfdfdsfdfs"<<endl;
						ast_node* temp = top_ele->cs_val->bound_var;
						if(temp->sibling!=NULL && temp->sibling->value!="")
						{
							queue<element*> tempQ;
							while(temp!=NULL && temp->value!="")
							{
								element* temp_ele = bind_ele->Q.front();
								if(temp_ele==NULL)
								{
									cout<<"error in gamma tuple binding to lambda"<<endl;
								}
								tempQ.push(temp_ele);
								bind_ele->Q.pop();
								if(debug2)
									cout<<"var: "<<temp->value<<", value: "<<temp_ele->int_val<<endl;
								new_env->binding[temp->value] = temp_ele;
								temp = temp->sibling;
							}
							while(tempQ.size()!=0)
							{
								bind_ele->Q.push(tempQ.front());
								tempQ.pop();
							}
						}
						else
						{
							new_env->binding[temp->value] = bind_ele;
						}
					}
					else			/* Rule 4: Gamma on top of control stack and lambda on exe stack */
					{
						new_env->binding[top_ele->cs_val->bound_var->value] = bind_ele;
					}
					current_env = new_env;
					element* new_ele1 = new element;
					new_ele1->type = ENV;
					new_ele1->env_val = new_env;
					exe_stack.push(new_ele1);
					
					ast_node* new_node = new ast_node;
					new_node->name = "env";
					new_node->value = "env";
					controlStruct* new_cs = new controlStruct;
					new_cs->node = new_node;
					new_cs->cur_env = current_env;
					control_stack.push(new_cs);
					getDelta(top_ele->cs_val->delta);
				}
			}			// -- CS
			else if(exe_stack.top()->type == YSTAR)		/* Rule 12: Gamma on top of control stack and Y on exe stack */
			{
				exe_stack.pop();
				element* temp_ele = exe_stack.top();
				if(temp_ele->type != CS)
				{
					cout<<"Error: ystar wasnt followd by lambda in exe stack"<<endl;
				}
				//temp_ele->type = ETA;
				exe_stack.top()->type = ETA;
			}		// -- Ystar
			else if(exe_stack.top()->type == ETA)		/* Rule 13: Gamma on top of control stack and ETA on exe stack */
			{
				element* new_cs_ele = new element;
				new_cs_ele->type = CS;
				new_cs_ele->cs_val = exe_stack.top()->cs_val;
				exe_stack.push(new_cs_ele);
				
				ast_node* gamma = new ast_node;
				gamma->name = "gamma";
				gamma->value = "gamma";
				controlStruct* gamma_cs = new controlStruct;
				gamma_cs->node = gamma;
				control_stack.push(gamma_cs);
				
				ast_node* gamma2 = new ast_node;
				gamma2->name = "gamma";
				gamma2->value = "gamma";
				controlStruct* gamma2_cs = new controlStruct;
				gamma2_cs->node = gamma2;
				control_stack.push(gamma2_cs);
			}		// -- ETA
			else if(exe_stack.top()->type == PF)		// Primitive Function
			{
				element* topPF = exe_stack.top();
				exe_stack.pop();
				
				if(topPF->func_val == PRINT)
				{
					element* print_ele = exe_stack.top();
					if(print_ele==NULL)
					{
						cout<<"Error: nothing to print"<<endl;
					}
					exe_stack.pop();
					printEle(print_ele);
					element* new_ele1 = new element;
					new_ele1->type = DUMMY;
					exe_stack.push(new_ele1); 
				}
				else if(topPF->func_val == ISINTEGER)
				{
					element* isint_ele = exe_stack.top();
					if(isint_ele == NULL)
					{
						cout<<"Error: no element found to check for isint"<<endl;
					}
					exe_stack.pop();
					element* new_ele = new element;
					if(isint_ele->type == INT)
					{
						new_ele->type = BOOL_T;
					}
					else
					{
						new_ele->type = BOOL_F;
					}
					exe_stack.push(new_ele);
				}
				else if(topPF->func_val == ISSTRING)
				{
					element* isstr_ele = exe_stack.top();
					if(isstr_ele==NULL)
					{
						cout<<"Error: no element found to check for isstring"<<endl;
					}
					exe_stack.pop();
					element* new_ele = new element;
					if(isstr_ele->type==STR)
					{
						new_ele->type = BOOL_T;
					} 
					else
					{
						new_ele->type = BOOL_F;
					}
					exe_stack.push(new_ele);
				}
				else if(topPF->func_val == ISTRUTHVALUE)
				{
					element* isbool_ele = exe_stack.top();
					//printEle(isbool_ele);cout<<endl;					
					if(isbool_ele==NULL)
					{
						cout<<"Error: no element found to check for istruthval"<<endl;
					}
					exe_stack.pop();
					element* new_ele = new element;
					if(isbool_ele->type==BOOL_T || isbool_ele->type==BOOL_F)
					{
						new_ele->type = BOOL_T;
					} 
					else
					{
						new_ele->type = BOOL_F;
					}
					exe_stack.push(new_ele);
				}
				else if(topPF->func_val == ISTUPLE)
				{
					element* istup_ele = exe_stack.top();
					if(istup_ele==NULL)
					{
						cout<<"Error: no element found to check for istuple"<<endl;
					}
					exe_stack.pop();
					element* new_ele = new element;
					if(istup_ele->type==TUPLE || istup_ele->type == NIL)
					{
						new_ele->type = BOOL_T;
					} 
					else
					{
						new_ele->type = BOOL_F;
					}
					exe_stack.push(new_ele);
				}
				else if(topPF->func_val == ORDER)
				{
					element* tup_ele = exe_stack.top();
					exe_stack.pop();
					int n=0;
					if(tup_ele->type==TUPLE)
					{
						n = tup_ele->Q.size();	
					}
					element* new_ele = new element;
					new_ele->type = INT;
					new_ele->int_val = n;
					exe_stack.push(new_ele);
				}
				else if(topPF->func_val == STEM)
				{
					element* stem_ele = exe_stack.top();
					exe_stack.pop();
					string str = stem_ele->str_val;
					//string s = "";
					//if(str!="")
					//{
						//cout<<"string val in (b4) stem is:"<<stem_ele->str_val<<":"<<endl;
						if(str!="''")						
							str = str.substr(0,1);
						else
							str = "";
					//}
					//cout<<"stem string is:"<<str<<endl;
					element* new_ele = new element;
					new_ele->type = STR;
					new_ele->str_val = str;
					exe_stack.push(new_ele);
				}
				else if(topPF->func_val == STERN)
				{
					element* stern_ele = exe_stack.top();
					exe_stack.pop();
					string str = stern_ele->str_val;
					string s = "";
					//if(str!="")
					//{
						//cout<<"string val in (b4) stern is:"<<stern_ele->str_val<<":"<<endl;
						if(str!="''")
							s = str.substr(1,str.size());
						//else
							//s = "''";
					//}
					//cout<<"stem string is:"<<str<<endl;
					element* new_ele = new element;
					new_ele->type = STR;
					new_ele->str_val = s;
					exe_stack.push(new_ele);
				}
				else if(topPF->func_val == CONC)
				{
					element* ele1 = exe_stack.top();
					exe_stack.pop();
					string s="";
					if(ele1->type == STR)
					{
						if(ele1->str_val != "''")
						{
							s = ele1->str_val;				
						}
						if(topPF->is_conc==false)
						{
							topPF->str_val = s;
							topPF->is_conc=true;
							exe_stack.push(topPF);
						}
						else
						{
							element* new_ele = new element;	
							new_ele->type = STR;
							new_ele->str_val = topPF->str_val+s;
							//cout<<":"<<topPF->str_val<<":"<<ele1->str_val<<":"<<endl;
							exe_stack.push(new_ele);
						}
					}
				}
				else if(topPF->func_val == ITOS)
				{
					element* itos_ele = exe_stack.top();
					if(itos_ele == NULL)
					{
						cout<<"no element to use itos on"<<endl;
					}
					exe_stack.pop();
					string str = to_string(itos_ele->int_val);
					element* temp = new element;
					temp->type = STR;
					temp->str_val = str;
					exe_stack.push(temp);
				}
			}	//--PF
			else if(exe_stack.top()->type == TUPLE)		/* Rule 10: Gamma on top of control stack and Tuple on exe stack */
			{
				element* tup_ele = exe_stack.top();
				exe_stack.pop();
				element* tup_index = exe_stack.top();
				//cout<<"tup index:"<<tup_index->int_val<<":"<<endl;
				exe_stack.pop();
				if(tup_index!=NULL && tup_index->type==INT)
				{
					queue<element*> tempQ;
					for(int i=1;i<tup_index->int_val;i++)
					{
						tempQ.push(tup_ele->Q.front());
						tup_ele->Q.pop();
					}
					element* new_ele = new element;
					element* temp = tup_ele->Q.front();
					while(tup_ele->Q.size()!=0)
					{
						tempQ.push(tup_ele->Q.front());
						tup_ele->Q.pop();
					}
					new_ele->type = temp->type;
					new_ele->int_val = temp->int_val;
					new_ele->str_val = temp->str_val;
					new_ele->Q = temp->Q;
					new_ele->cs_val = temp->cs_val;
					new_ele->env_val = temp->env_val;
					new_ele->func_val = temp->func_val;
					
					while(tempQ.size()!=0)
					{
						tup_ele->Q.push(tempQ.front());
						tempQ.pop();
					}
					exe_stack.push(new_ele);
				}
				else
				{
					cout<<"Error: in gamma , tuple"<<endl;
				}

			}	//--TUPLE
			else
			{
				cout<<"Error: unidentified element on stack when gamma was encountered -> of type: "<<exe_stack.top()->type<<endl;
				exit(0);
			}
			
		}
		else if(topCS->node->name == "env")		/* Rule 5: Environment variable on top of control stack */
		{
			element* temp = exe_stack.top();
			exe_stack.pop();
			
			//cout<<temp->type<<":"<<temp->int_val<<":"<<endl;			
			element* env_ele = exe_stack.top();
			//cout<<env_ele->type<<":"<<env_ele->int_val<<":"<<endl;
			//exe_stack.pop();
			if(env_ele->type==ENV && (env_ele->env_val==topCS->cur_env))
			{
				exe_stack.pop();
				//cout<<exe_stack.top()->type<<":"<<exe_stack.top()->int_val<<":"<<endl;
				exe_stack.push(temp);
				
				stack<element*> tempStack;
				while(exe_stack.size()!=0 && (exe_stack.top()->type!=ENV))
				{
					tempStack.push(exe_stack.top());
					exe_stack.pop();
				}
				if(exe_stack.size()!=0)
				{
					current_env = exe_stack.top()->env_val;	
				}
				while(tempStack.size()!=0)
				{
					exe_stack.push(tempStack.top());
					tempStack.pop();
				}
			}
			else
			{
				cout<<"Error: in env"<<endl;
				cout<<"stack size in env:"<<exe_stack.size()<<endl;
				printEle(env_ele);cout<<endl;
				cout<<":"<<env_ele->type<<":"<<endl;
				exit(0);
			}
		}
		else if(topCS->node->name == "tau")		/* Rule 9: TAU on top of control stack */
		{
			element* new_ele = new element;
			new_ele->type = TUPLE;
			int num = topCS->tauN;
			if(debug2)
				cout<<"num of children tau has: "<<num<<endl;
			for(int i=0;i<num;i++)
			{
				element* temp_ele = exe_stack.top();
				if(temp_ele!=NULL)
				{
					new_ele->Q.push(temp_ele);
					exe_stack.pop();
				}
				else
					cout<<"Error: for tau couldnt find all children"<<endl;
			}
			exe_stack.push(new_ele);
		}
		else if(topCS->node->name == "Y*")
		{
			element* ystar_ele = new element;
			ystar_ele->type = YSTAR;
			exe_stack.push(ystar_ele);
		}
		else if(topCS->node->name == "beta")		/* Rule 8: Beta on top of control stack */
		{
			element* bool_ele = exe_stack.top();
			exe_stack.pop();
			if(bool_ele->type==BOOL_T || bool_ele->type==BOOL_F)
			{
				controlStruct* deltaCS = new controlStruct;
				if(bool_ele->type==BOOL_T)			/* Rule 8: Beta on top of control stack and true on exe stack*/
				{
					//deltaCS = control_stack.top();
					control_stack.pop();	// pop delta else
					deltaCS = control_stack.top();	// save delta then
					control_stack.pop();
				}
				else								/* Rule 8: Beta on top of control stack and false on exe stack*/
				{
					deltaCS = control_stack.top();	//save delta else
					control_stack.pop();
					control_stack.pop();	// pop delta then
				}
				getDelta(deltaCS->delta);
			}
			else
			{
				cout<<"Error: beta should have bool element on exe stack"<<endl;
			}
		}
		else if(topCS->node->name == "eq" || topCS->node->name == "ne")		/* Rule 6: Binop on top of control stack (eq,ne)*/
		{
			element* ele1 = exe_stack.top();
			exe_stack.pop();
			element* ele2 = exe_stack.top();
			exe_stack.pop();
			bool result = false;
			if(ele1->type==STR && ele2->type==STR)	//comparing strings
			{
				if(ele1->str_val=="")
					ele1->str_val= "''";
				if(ele2->str_val=="")
					ele2->str_val= "''";
				if(topCS->node->name == "eq")
				{
					if(debug2)
						cout<<":"<<ele1->str_val<<":"<<ele2->str_val<<":"<<endl;
					if(ele1->str_val == ele2->str_val)
					{
						result = true;
					}
					else
					{
						result = false;
					}
				}
				else
				{
					if(debug2)
						cout<<":"<<ele1->str_val<<":"<<ele2->str_val<<":"<<endl;
					if(ele1->str_val != ele2->str_val)
					{
						result = true;
					}
					else
					{
						result = false;
					}
				}
			}
			else if((ele1->type==BOOL_T || ele1->type==BOOL_F) && (ele2->type==BOOL_T || ele2->type==BOOL_F))	// comparing bools
			{
				if(topCS->node->name == "eq")
				{
					if(ele1->type == ele2->type)
					{
						result = true;
					}
					else
					{
						result = false;
					}
				}
				else
				{
					if(ele1->type != ele2->type)
					{
						result = true;
					}
					else
					{
						result = false;
					}
				}
			}
			else if(ele1->type==INT && ele2->type==INT)		//comparing ints
			{
				if(topCS->node->name == "eq")
				{
					if(ele1->int_val == ele2->int_val)
					{
						result = true;
					}
					else
					{
						result = false;
					}
				}
				else
				{
					if(ele1->int_val != ele2->int_val)
					{
						result = true;
					}
					else
					{
						result = false;
					}
				}
			}
			else
			{
				cout<<"Error: in eq/ne wrong type of elements being compared"<<endl;
			}
			element* result_ele = new element;
			if(result)
			{
				result_ele->type = BOOL_T;
			}
			else
			{
				result_ele->type = BOOL_F;
			}
			exe_stack.push(result_ele);
		}
		else if(topCS->node->name == "aug")		/* Rule 6: Binop on top of control stack (aug)*/
		{
			element* ele1 = exe_stack.top();	// first tuple
			exe_stack.pop();
			element* ele2 = exe_stack.top();	// second tuple
			exe_stack.pop();
			element* new_ele = new element;
			new_ele->type = TUPLE;
			//if((ele1->type==TUPLE || ele1->type==NIL) && (ele2->type==TUPLE || ele2->type==NIL))
			{
				queue<element*> tempQ;
				if(ele1->type!=NIL)		// push elements of first tuple
				{
					if(ele1->type==TUPLE)
					{
						while(ele1->Q.size()!=0)
						{
							new_ele->Q.push(ele1->Q.front());
							tempQ.push(ele1->Q.front());
							ele1->Q.pop();
						}
						while(tempQ.size()!=0)
						{
							ele1->Q.push(tempQ.front());
							tempQ.pop();
						}
					}
					else
					{
						new_ele->Q.push(ele1);
					}
				}
				

				if(ele2->type!=NIL)
				{	
					if(ele2->type==TUPLE)
					{
						while(ele2->Q.size()!=0)	// push elements of second tuple
						{
							new_ele->Q.push(ele2->Q.front());
							tempQ.push(ele2->Q.front());
							ele2->Q.pop();
						}
						while(tempQ.size()!=0)
						{
							ele2->Q.push(tempQ.front());
							tempQ.pop();
						}
					}
					else
					{
						new_ele->Q.push(ele2);
					}
				}
				exe_stack.push(new_ele);
			}
			/*else
			{
				if(ele1->type!=NIL)
				{
					new_ele->Q.push(ele1);
					exe_stack.push(new_ele);
				}
				if(ele2->type!=NIL)
				{
					new_ele->Q.push(ele2);
					exe_stack.push(new_ele);
				}
				cout<<"Error: aug didnt find 2 tuples on exe stack: insted found:"<<endl;
				printEle(ele1);cout<<endl;
				printEle(ele2);cout<<endl;
			}*/
		}
		else if(topCS->node->name == "&" || topCS->node->name == "or")	/* Rule 6: Binop on top of control stack (&,or)*/
		{
			element* ele1 = exe_stack.top();
			exe_stack.pop();
			element* ele2 = exe_stack.top();
			exe_stack.pop();
			if((ele1->type==BOOL_T || ele1->type==BOOL_F) && (ele2->type==BOOL_T || ele2->type==BOOL_F))
			{
				element* new_ele = new element;
				if(topCS->node->name == "&")
				{
					if(ele1->type==BOOL_T && ele2->type==BOOL_T)
					{
						new_ele->type = BOOL_T;
					}
					else
					{
						new_ele->type = BOOL_F;
					}
				}
				else
				{
					if(ele1->type==BOOL_F && ele2->type==BOOL_F)
					{
						new_ele->type = BOOL_F;
					}
					else
					{
						new_ele->type = BOOL_T;
					}
				}
				exe_stack.push(new_ele);
			}
			else
			{
				cout<<"Error: for &/or did not find two bools to operate on: instead found: "<<ele1->type<<":"<<ele2->type<<endl;
				printEle(ele1);cout<<endl;
				printEle(ele2);cout<<endl;
			}
		}
		else if(topCS->node->name == "neg")			/* Rule 7: Unop on top of control stack (neg)*/
		{
			element* ele1 = exe_stack.top();
			exe_stack.pop();
			if(ele1->type == INT)
			{
				element* new_ele = new element;
				new_ele->type = INT;
				new_ele->int_val = -(ele1->int_val);
				exe_stack.push(new_ele);
			}
			else
			{
				cout<<"Error: neg did not find integer to operate on"<<endl;
			}
		}
		else if(topCS->node->name == "not")			/* Rule 7: Unop on top of control stack (not)*/
		{
			element* ele1 = exe_stack.top();
			exe_stack.pop();
			if(ele1->type == BOOL_T || ele1->type==BOOL_F)
			{
				element* new_ele = new element;
				if(ele1->type == BOOL_T)
				{
					new_ele->type = BOOL_F;
				}
				else
				{
					new_ele->type = BOOL_T;
				}
				exe_stack.push(new_ele);
			}
			else
			{
				cout<<"Error: NOT - did not find bool to operate on"<<endl;
			}
		}
		else if(topCS->node->name == "/" || topCS->node->name == "*" || topCS->node->name == "+" ||
				topCS->node->name == "-" || topCS->node->name == "ls" || topCS->node->name == "gr" ||
				topCS->node->name == "le" || topCS->node->name == "**")		/* Rule 6: Binop on top of control stack (multiple)*/
		{
			element* ele1 = exe_stack.top();
			exe_stack.pop();
			element* ele2 = exe_stack.top();
			exe_stack.pop();
			int result;
			if(ele1->type==INT && ele2->type==INT)
			{
				element * new_ele = new element;
				if(topCS->node->name == "/")
				{
					if(ele2->int_val!=0)
					{
						result = (ele1->int_val)/(ele2->int_val);
						new_ele->type = INT;
						new_ele->int_val = result;
					}
					else
						cout<<"Error: cannot divide by zero"<<endl;
				}
				else if(topCS->node->name == "*")
				{
					result = (ele1->int_val)*(ele2->int_val);
					new_ele->type = INT;
					new_ele->int_val = result;
				}
				else if(topCS->node->name == "+")
				{
					result = (ele1->int_val)+(ele2->int_val);
					new_ele->type = INT;
					new_ele->int_val = result;
				}
				else if(topCS->node->name == "-")
				{
					result = (ele1->int_val)-(ele2->int_val);
					new_ele->type = INT;
					new_ele->int_val = result;
				}
				else if(topCS->node->name == "**")
				{
					result = pow((ele1->int_val),(ele2->int_val));
					new_ele->type = INT;
					new_ele->int_val = result;
				}
				else if(topCS->node->name == "ls")
				{
					if(ele1->int_val < ele2->int_val)
					{
						new_ele->type = BOOL_T;
					}
					else
						new_ele->type = BOOL_F;
				}
				else if(topCS->node->name == "gr")
				{
					if(ele1->int_val > ele2->int_val)
					{
						new_ele->type = BOOL_T;
					}
					else
						new_ele->type = BOOL_F;
				}
				else if(topCS->node->name == "le")
				{
					if(ele1->int_val <= ele2->int_val)
					{
						new_ele->type = BOOL_T;
					}
					else
						new_ele->type = BOOL_F;
				}
				exe_stack.push(new_ele);
			}
			else
			{
				cout<<"Error: did nto get two integers to *,/,+,-,**,ls,gr,le"<<endl;
				printEle(ele1);cout<<endl;
				printEle(ele2);cout<<endl;
			}
		}
		if(debug2)
		{
			cout<<"exe stack size after: "<<exe_stack.size()<<endl;
			if(exe_stack.size()>0)
			{
				cout<<"exe stack top after: "<<exe_stack.top()->type<<endl;
				cout<<"exe stack top: ";printEle(exe_stack.top());cout<<endl;
			}
			cout<<"----------------------------------------------------"<<endl;
		}
	}
}

/* Function to find binding of element to values in the environment */

element* findBinding(string str,env* cur_env)
{
	//cout<<"str who's binding to find:"<<str;
	if(cur_env->binding[str]!= NULL)
	{
		return cur_env->binding[str];
	}
	else if(cur_env->parent!=NULL)
	{
		return findBinding(str,cur_env->parent);
	}
	else
	{
		return NULL;
	}
}

/* function to load new delta on control stack */

void getDelta(controlStruct* delta)
{
	if(debug2)
		cout<<"in get delta"<<endl;
	if(delta==NULL)
	{
		cout<<"error:delta is NULL"<<endl;
	}
	else
	{
		while(delta!=NULL)
		{
			if(debug2)
				cout<<"pushing delta cs:"<<delta->node->name<<":"<<delta->node->value<<endl;
			control_stack.push(delta);
			delta = delta->next;
		}
	}
}

/* function to print elements */

void printEle(element* print_ele)
{
	switch(print_ele->type)
	{
		case INT:
			cout<<print_ele->int_val;
			break;
		
		case STR:
			//cout<<print_ele->str_val;
			printStr(print_ele->str_val);
			break;	

		case ENV:
			cout<<"Env"<<endl;
			break;
		
		case TUPLE:
			printTuple(print_ele);			
			break;

		case CS:
			cout<<"[lambda closure: "<<print_ele->cs_val->bound_var->value<<": "<<print_ele->cs_val->lambda_id<<"]";
			break;
		
		case YSTAR:
			cout<<"Y*"<<endl;
			break;
		
		case ETA:
			cout<<"eta : "<<print_ele->cs_val->bound_var->value<<endl;
			break;

		case BOOL_T:
			cout<<"true";
			break;

		case BOOL_F:
			cout<<"false";
			break;
		
		case DUMMY:
			cout<<"dummy";
			break;
		
		case NIL:
			cout<<"nil";
			break;
		
		default:
			cout<<"Error: element type wrong"<<endl;
			break;
	}
}

/* function to print Tuples */

void printTuple(element* print_ele)
{
	queue<element*> tempQ;
	cout<<"(";
	while(print_ele->Q.size()!=0)
	{
		element* temp_ele = print_ele->Q.front();
		if(temp_ele == NULL)
		{
			cout<<"Error:tupple element is null (int print)"<<endl;
		}
		else
		{
			printEle(temp_ele);
			if(print_ele->Q.size()>1)
			{
				cout<<", ";
			}
		}
		tempQ.push(temp_ele);
		print_ele->Q.pop();
	}
	cout<<")";
	while(tempQ.size()!=0)
	{
		print_ele->Q.push(tempQ.front());
		tempQ.pop();
	}
}

/* Function to print strings */
/* string when printed directly printed out /t /n instead of actual tab and new lines
  so need to be printed char by char */

void printStr(string str)
{
	for(int i=0;i<str.size();i++)
	{
		if(str[i]=='\\')
		{
			i++;
			if(str[i]=='n')
			{
				cout<<endl;
			}
			else
			{
				cout<<"\t";
			}
		}
		else
		{
			cout<<str[i];
		}
	}
}
