//防止头文件多次被引用
#ifndef SCANNER_H
#define SCANNER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <stdarg.h>
#include <math.h>

enum Token_Type                                                  //记号种类
{
    ORIGIN, SCALE, ROT, IS, TO,STEP, DRAW, FOR, FROM,            //保留字
    T,                                                           //参数
    SEMICO, L_BRACKET, R_BRACKET, COMMA,                         //分隔符号
    PLUS, MINUS, MUL, DIV, POWER,                                //运算符
    FUNC,                                                        //函数
    CONST_ID,                                                    //常数
    NONTOKEN,                                                    //空记号
    ERRTOKEN                                                     //出错记号
};

//函数指针，指针指向参数和返回类型都为double的函数
typedef double(*MathFuncPtr)(double);

struct Token                              //记号的数据结构
{
    Token_Type type;                      //记号的种类
    char* lexeme;                         //构成记号的字符串
    double value;                         //若为常数，则是常数的值
    double(*FuncPtr)(double);             //若为函数，则是函数的指针
};

static Token TokenTable[] = 				  //符号表内容
{
	{ CONST_ID,	(char*)"PI", 		3.1415926,		NULL },
	{ CONST_ID,	(char*)"E",			2.71828,		NULL },
	{ T,		(char*)"T",			0.0,			NULL },
	{ FUNC,		(char*)"SIN",		0.0,			sin },
	{ FUNC,		(char*)"COS",		0.0,			cos },
	{ FUNC,		(char*)"TAN",		0.0,			tan },
	{ FUNC,		(char*)"LN",		0.0,			log },
	{ FUNC,		(char*)"EXP",		0.0,			exp },
	{ FUNC,		(char*)"SQRT",		0.0,			sqrt },
	{ ORIGIN,	(char*)"ORIGIN",	0.0,			NULL },
	{ SCALE,	(char*)"SCALE",		0.0,			NULL },
	{ ROT,		(char*)"ROT",		0.0,			NULL },
	{ IS,		(char*)"IS",		0.0,			NULL },
	{ FOR,		(char*)"FOR",		0.0,			NULL },
	{ FROM,		(char*)"FROM",		0.0,			NULL },
	{ TO,		(char*)"TO",		0.0,			NULL },
	{ STEP,		(char*)"STEP",		0.0,			NULL },
	{ DRAW,		(char*)"DRAW",		0.0,			NULL },
};

//extern表示声明，定义在cpp文件
extern unsigned int LineNo;				//跟踪记号所在源文件行号
extern int InitScanner(const char *);	//初始化词法分析器
extern Token GetToken(void);			//获取记号函数
extern void CloseScanner(void);			//关闭词法分析器

#endif
