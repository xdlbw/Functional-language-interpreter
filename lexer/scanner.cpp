#include "scanner.h"

#define MAX_TOKEN_LEN 50                           //记号最大长度

unsigned int LineNo;
static char TokenBuffer[MAX_TOKEN_LEN];            //记号字符缓冲
static FILE* File;                                 //指向扫描文件

//初始化词法分析器
extern int InitScanner(const char* FileName){
	LineNo = 1;						//行号
	File = fopen(FileName, "r");	//只读形式打开文件
	if(File != NULL) return 1;		//打开成功
	else return 0;					//打开失败
}

//关闭词法分析器
extern void CloseScanner(void){
	if(File != NULL)
		fclose(File);				//关闭文件
}

//从输入源程序（流）读字符
static char GetChar(void){
	char Char = getc(File);			//逐个字符读
	return toupper(Char);			//全部转换为大写，然后return
}

//将预读字符退回源程序（流）
//若前面记号缓冲区中已构成一个合法记号，且加入这个字符之后变得不合法了，就将其重新写回文件等待下一轮识别
static void BackChar(char Char){
	if(Char != EOF)
		ungetc(Char, File);
}

//加入字符到记号缓冲区
static void AddCharTokenString(char Char){
	int TokenLength = strlen(TokenBuffer);
	if(TokenLength + 1 >= sizeof(TokenBuffer)) return;		//发生数组越界
	TokenBuffer[TokenLength] = Char;						//写入字符到缓冲区
	TokenBuffer[TokenLength + 1] = '\0';					//加入字符串结束标志\0
}

//请空记号缓冲区
static void EmptyTokenString(){
	memset(TokenBuffer, 0, MAX_TOKEN_LEN);
}

//判断记号是否在记号表中
static Token JudgeKeyToken(const char *IDString){
	int i;         
    //遍历TokenTab表   
    for(i = 0; i<sizeof(TokenTable) / sizeof(TokenTable[0]); i++){
        //判断所给的字符是否与TokenTable中的项存在匹配
        if (strcmp(TokenTable[i].lexeme, IDString) == 0)  return TokenTable[i]; 
    }
    Token errortoken;
    memset(&errortoken, 0, sizeof(Token));
    //如果没有，默认为出错记号
    errortoken.type = ERRTOKEN;
    return errortoken;
}

//获得一个记号
extern Token GetToken(void){
	Token token;
	char Char;

	memset(&token, 0, sizeof(Token));		//token置空
	EmptyTokenString();						//清空记号缓冲区
	token.lexeme = TokenBuffer;

	//此循环用来过滤掉源程序中的空格、TAB、回车等分隔符，文件结束返回空记号
	for (;;){
		Char = GetChar();					//从源程序读字符
		if (Char == EOF){					//读出错
			token.type = NONTOKEN;
			return token;
		}
		if (Char == '\n')	LineNo++;		//行号+1
		if (!isspace(Char))	break;			//遇到空格该记号肯定已经完成，退出循环
	}

	//若不是空格、TAB、回车、文件结束符等，则将其加入到记号的字符缓冲区中
	AddCharTokenString(Char);

	//判断Char是不是英文字母
    //若char是A-Za-z，则一定是函数、关键字、PI、E等。
	if (isalpha(Char)){
		for (;;){
			Char = GetChar();
			//继续判断下一个字符变量Char是否为字母或者数字
			if (isalnum(Char))	AddCharTokenString(Char);//加入缓冲区
			else break;
		}
		//如果Char是字母或者数字，则此记号已经结束，应该将使其出错的字符回退到文件中等待下一轮读入
		BackChar(Char);
		//判断token是不是符号表里的
		token = JudgeKeyToken(TokenBuffer);
		token.lexeme = TokenBuffer;
		return token;
	}

	//若是一个数字，则一定是常量，处理方式跟上述字母差不多
    //但是判断下一个字符只需要考虑数字和小数点即可，因为数字后面跟字母不符合语法规范
	else if (isdigit(Char)){
		for (;;){
			Char = GetChar();
			if (isdigit(Char))	AddCharTokenString(Char);
			else break;
		}
		//小数
		if (Char == '.'){
			AddCharTokenString(Char);
			for (;;){
				Char = GetChar();
				if (isdigit(Char))	AddCharTokenString(Char);
				else break;
			}
		}
		BackChar(Char);
		token.type = CONST_ID;
		token.value = atof(TokenBuffer);   		//把字符串转换成浮点数
		return token;
	}

	//不是字母和数字，则一定是运算符或分隔符
	else{
		switch (Char){
			case ';': token.type = SEMICO; break;
			case '(': token.type = L_BRACKET; break;
			case ')': token.type = R_BRACKET; break;
			case ',': token.type = COMMA; break;
			case '+': token.type = PLUS; break;
			// -号特殊处理，因为--是注释符
			case '-':
				Char = GetChar();
				if (Char == '-'){
					while (Char != '\n' && Char != EOF) Char = GetChar();
					BackChar(Char);
					return GetToken();
				}
				else{
					BackChar(Char);
					token.type = MINUS;
					break;
				}
			// /号特殊处理，因为//是注释符
			case '/':
				Char = GetChar();
				if (Char == '/'){
					while (Char != '\n' && Char != EOF) Char = GetChar();
					BackChar(Char);
					return GetToken();
				}
				else{
					BackChar(Char);
					token.type = DIV;
					break;
				}
			// *号特殊处理，**表示幂运算
			case '*':
				Char = GetChar();
				if (Char == '*'){
					token.type = POWER;
					break;
				}
				else{
					BackChar(Char);
					token.type = MUL;
					break;
				}
			//缺省情况，出错
			default:token.type = ERRTOKEN; break;
		}
	}
	return token;
}