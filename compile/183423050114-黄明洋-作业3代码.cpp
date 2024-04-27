/*
词法分析器



*/

#include<iostream>
#include <stdio.h>
#include<map>
#include<string>
//#include<sstream>
#include<set>
#include<vector>
using namespace std;

vector<string> result;
set<int> lex_line;

int cnt=1;
map<string, int> Table;
map<int, int> isTerminal;//当前状态是否是终态；
map<int, int> needRetract;//当前终态是否需要回退；
string s;
//map<string, int> reservedCode;//存放保留字、关键字码；


int stateTrans[1000][1000];//ASCII码值最大1000;


void initStateTrans() {
	//下面是第一行（状态1的所有弧）的定义
	//读入一个字母，2状态表示正在识别标识符，保留字，关键字
	for (int i = int('a'); i <= int('z');i++) {
		stateTrans[1][i] = 2;
	}
	for (int i = int('A'); i <= int('Z'); i++) {
		stateTrans[1][i] = 2;
	}
	//3状态表示正在识别整数
	for (int i = int('0'); i <= int('9'); i++) {
		stateTrans[1][i] = 3;
	}
	stateTrans[1][18] = 18;//换行符号占用了

	stateTrans[1][int('+')] = int('+');
	stateTrans[1][int('-')] = int('-');
	stateTrans[1][int('*')] = int('*');
	stateTrans[1][int('/')] = int('/');
	stateTrans[1][int('%')] = int('%');
	stateTrans[1][int('<')] = int('<');
	stateTrans[1][int('>')] = int('>');
	stateTrans[1][int('=')] = int('=');
	stateTrans[1][int('!')] = int('!');
	stateTrans[1][int('&')] = int('&');
	stateTrans[1][int('|')] = int('|');
	stateTrans[1][int('(')] = int('(');
	stateTrans[1][int(')')] = int(')');
	stateTrans[1][int('[')] = int('[');
	stateTrans[1][int(']')] = int(']');
	stateTrans[1][int('{')] = int('{');
	stateTrans[1][int('}')] = int('}');
	stateTrans[1][int(';')] = int(';');
	stateTrans[1][int(',')] = int(',');
	stateTrans[1][int('#')] = int('#');

	for (int i = 0; i < 1000; i++) {
		if (stateTrans[1][i] == 0) {
			//将无定义的状态都定义为错误状态14
			stateTrans[1][i] = 14;
		}
	}

	//下面是状态2的定义

	for (int i = int('a'); i <= int('z'); i++) {
		stateTrans[2][i] = 2;
	}
	for (int i = int('A'); i <= int('Z'); i++) {
		stateTrans[2][i] = 2;
	}
	
	for (int i = int('0'); i <= int('9'); i++) {
		stateTrans[2][i] = 2;
	}
	//除了字母数字读入其他字符都定义为状态4（终态）
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[2][i] == 2)continue;
		stateTrans[2][i] = 4;
	}

	//下面是状态3的定义，识别数字
	for (int i = int('0'); i <= int('9'); i++) {
		stateTrans[3][i] = 3;
	}
	

	for (int i = 0; i < 1000; i++) {
		if (stateTrans[3][i] == 3)continue;
		stateTrans[3][i] = 5;
	}
	for (int i = int('a'); i <= int('z'); i++) {
		stateTrans[3][i] = 26;
	}
	for (int i = int('A'); i <= int('Z'); i++) {
		stateTrans[3][i] = 26;
	}
	for (int i = int('a'); i <= int('z'); i++) {
		stateTrans[26][i] = 26;
	}
	for (int i = int('A'); i <= int('Z'); i++) {
		stateTrans[26][i] = 26;
	}
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[26][i] ==26)continue;
		//状态23为识别"123asd",需要回退
		stateTrans[26][i] = 23;
	}


	//其他直接抵达终态的状态号定义为这个字符的ascii码，因为我懒得记了
	//还有这几个字符没有到达终态：/,<,>,=,!,&,|

	//状态6为识别"/*",状态19识别为“//”
	stateTrans[int('/')][int('*')] = 6;
	stateTrans[int('/')][int('/')] = 19;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('/')][i] == 6|| stateTrans[int('/')][i] == 19)continue;
		//状态17为识别"/",需要回退
		stateTrans[int('/')][i] = 17;
	}

	//状态7为识别"<=",不用回退
	stateTrans[int('<')][int('=')] = 7;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('<')][i] == 7)continue;
		//状态8为识别"<",需要回退
		stateTrans[int('<')][i] = 8;
	}

	//状态9为识别">=",不用回退
	stateTrans[int('>')][int('=')] = 9;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('>')][i] == 9)continue;
		//状态10为识别"<",需要回退
		stateTrans[int('>')][i] = 10;
	}
	//状态11为识别"==",不用回退
	stateTrans[int('=')][int('=')] = 11;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('=')][i] == 11)continue;
		//状态12为识别"=",需要回退
		stateTrans[int('=')][i] = 12;
	}

	//状态13为识别"!=",不用回退
	stateTrans[int('!')][int('=')] = 13;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('!')][i] == 13)continue;
		//状态14为错误，需要回退
		stateTrans[int('!')][i] = 14;
	}
	//状态15为识别"&&",不用回退
	stateTrans[int('&')][int('&')] = 15;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('&')][i] == 15)continue;
		//状态14为错误，需要回退
		stateTrans[int('!')][i] = 14;
	}
	
	//状态16为识别"||",不用回退
	stateTrans[int('|')][int('|')] = 16;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('|')][i] == 16)continue;
		//状态14为错误，需要回退
		stateTrans[int('|')][i] = 14;
	}

	
	
	//状态25为识别"*/",不用回退
	stateTrans[int('*')][int('/')] = 25;
	for (int i = 0; i < 1000; i++) {
		if (stateTrans[int('*')][i] == 25)continue;
		//状态21为识别"*"，需要回退
		stateTrans[int('*')][i] = 21;
	}
}
void initTerminal() {
	isTerminal[4] = 1;
	isTerminal[5] = 1;
	isTerminal[6] = 1;
	isTerminal[7] = 1;
	isTerminal[8] = 1;
	isTerminal[9] = 1;
	isTerminal[10] = 1;
	isTerminal[11] = 1;
	isTerminal[12] = 1;
	isTerminal[13] = 1;
	isTerminal[14] = 1;
	isTerminal[15] = 1;
	isTerminal[16] = 1;
	isTerminal[17] = 1;
	isTerminal[int('+')] = 1;
	isTerminal[int('-')] = 1;
	isTerminal[int('%')] = 1;
	isTerminal[int('(')] = 1;
	isTerminal[int(')')] = 1;
	isTerminal[int('[')] = 1;
	isTerminal[int(']')] = 1;
	isTerminal[int('{')] = 1;
	isTerminal[int('}')] = 1;
	isTerminal[int(';')] = 1;
	isTerminal[int(',')] = 1;
	isTerminal[int('#')] = 1;
	isTerminal[18] = 1;
	isTerminal[19] = 1;
	isTerminal[25] = 1;
	isTerminal[21] = 1;
	isTerminal[23] = 1;

	


	needRetract[4] = 1;
	needRetract[5] = 1;
	needRetract[8] = 1;
	needRetract[10] = 1;
	needRetract[12] = 1;
	needRetract[14] = 1;
	needRetract[17] = 1;
	needRetract[21] = 1;
	needRetract[23] = 1;
}
void initTable() {
	Table["void"] = 3;
	Table["int"] = 4;
	Table["float"] = 5;
	Table["char"] = 6;
	Table["if"] = 7;
	Table["else"] = 8;
	Table["while"] = 9;
	Table["do"] = 10;
	Table["for"] = 11;
	Table["return"] = 12;
	Table["main"] = 13;
	Table["+"] = 14;
	Table["-"] = 15;
	Table["*"] = 16;
	Table["/"] = 17;
	Table["%"] = 18;
	Table["<"] = 19;
	Table["<="] = 20;
	Table[">"] = 21;
	Table[">="] = 22;
	Table["=="] = 23;
	Table["!="] = 24;
	Table["&&"] = 25;
	Table["||"] = 26;
	Table["="] = 27;
	Table["("] = 28;
	Table[")"] = 29;
	Table["["] = 30;
	Table["]"] = 31;
	Table["{"] = 32;
	Table["}"] = 33;
	Table[";"] = 34;
	Table[","] = 35;
}
void init() {
	initTable();//初始化定义表
	initStateTrans();//初始化状态转移矩阵
	initTerminal();//初始化终态属性
	result.resize(100);
}	
string getWord() {
	int curState = 1;
	char tempChar;
	string tempString="";
	do {
		tempChar = cin.get();
	} while (tempChar == ' ');//忽略多余的空格
	while (tempChar == '\n') {//忽略多余的空行
		char c=cin.get();
		if (c != '\n') {
			cin.unget();
			break;
		}
		else {
			tempChar = c;
		}
	}
	while (stateTrans[curState][int(tempChar)]) { //在状态转移中有定义
		//cout << int(tempChar) << endl;
		tempString = tempString.append(1, tempChar);
		if (tempChar == '\n') {
			curState = stateTrans[curState][18];
		}
		else {
			curState = stateTrans[curState][int(tempChar)];
		}
		if (isTerminal[curState]) {
			if (needRetract[curState]) {
				cin.unget();
				//ungetc(tempChar, sss);
				tempString.pop_back();
			}
			if (curState == 4) {
				if (Table.find(tempString) == Table.end()) {
					Table[tempString] = 1;
				}
			}
			if(curState == 5) {
				if (Table.find(tempString) == Table.end()) {
					Table[tempString] = 2;
				}
			}
			if (curState == 23) {
				//cout << "-----------------------------" << endl;
				Table[tempString] = 0;
			}
			return tempString;
		}
		tempChar = cin.get();
	}
}
void compose(string tp) {
	
	if (Table[tp] >= 3 && Table[tp] <= 35) {

		string s = "<" + to_string(Table[tp]) + ",->,";
		//cout <<endl<<"cnt:" << cnt << endl;


		result[cnt].append(s);

		//cout << "<" << Table[tp] << ',' <<'-' << '>' << ',';
	}

	else if (Table[tp] >= 1 && Table[tp] <= 2) {
		string s = "<" + to_string(Table[tp]) + ","+tp+">,";
		//cout << endl << "cnt:" << cnt << endl;
		result[cnt].append(s);


		//cout << "<" << Table[tp] << ',' << tp << '>' << ',';
	
	}
	else if (Table[tp] == 0) {
		//cout << "LexicalError,";
		string s = "LexicalError,";
		result[cnt].append(s);
		lex_line.insert(cnt);

	}




}
int main()
{
	init();
	string tempWord;

	tempWord = getWord();
	while (tempWord != "#") {
		if (tempWord == "\n") {
			cnt++;
			//cout << cnt;
			tempWord = getWord();
			continue;
		}
		if (tempWord == "//") {//杠杠注释
			string a;
			getline(cin,a);
			cin.unget();
			cnt--;
			tempWord = getWord();
			continue;
		}
		if (tempWord == "/*") {//杠星注释
			cnt--;
			do {
				tempWord = getWord();
			} while (tempWord != "*/");
			tempWord = getWord();
			continue;

		}
		compose(tempWord);//格式化
		tempWord = getWord();
	}

	for (int i = 1; i <= cnt; i++) {//输出词法分析结果
		if (i != cnt)
			cout << result[i] << endl;
		else
			cout << result[i];

	}
	if (!lex_line.empty()) {//输出错误的语句
		printf("LexicalError(s) on line(s) ");
		for (auto it = lex_line.begin(); it != lex_line.end(); it++) {
			cout << *it << ",";

		}
	
	}
	
}
