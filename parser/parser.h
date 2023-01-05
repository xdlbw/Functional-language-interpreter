//防止头文件多次被引用
#ifndef PARSER_H
#define PARSER_H

#include"..\lexer\scanner.h"

//数据结构区
typedef double(*FuncPtr)(double);

/* 表达式树节点
1.叶节点:常数CONST_ID、参数T

2.有两个孩子的内部节点 也就是运算符节点:加PLUS、减MINUS、乘MUL、除DIV、幂运算POWER  
(有两个特例就是一元加和一元减，比如+5，这个+的左孩子就是NULL，右孩子是5，就将+5转换成5了；
还有-5，这个-的左孩子就是0，右孩子就是5，就将-5转换成0-5了)

3.有一个孩子的内部节点 也就是函数节点:函数FUNC  */

struct ExprNode                                   //语法树节点类型
{
	enum Token_Type OpCode;                       //记号种类
	union
	{
		struct { ExprNode *Left, *Right; }CaseOperator;     //二元运算：只有左右孩子的内部节点
		struct { ExprNode *Child; FuncPtr MathFuncPtr; }CaseFunc;//函数调用：只有一个孩子的内部节点，还有一个指向对应函数名的指针 MathFuncPtr
		double CaseConst;                                   //常数
		double *CaseParmPtr;                                //参数T
	}Content;
};

//函数区
void Parser(char *SrcFilePtr);                 //语法分析主程序

//主函数：产生式(语句级)逻辑区
static void Program();         	//程序
static void Statement();		//语句
static void OriginStatement();	//Origin语句
static void RotStatement();		//Rot语句
static void ScaleStatement();	//Scale语句
static void ForStatement();		//For语句

//主函数：产生式(表达式级)逻辑区
static struct ExprNode* Expression();	//表达式、二元加减运算表达式
static struct ExprNode* Term();			//乘除运算表达式
static struct ExprNode* Factor();		//一元加减运算表达式
static struct ExprNode* Component();	//幂运算表达式
static struct ExprNode* Atom();			//原子表达式

//构建语法树
static struct ExprNode* MakeExprNode(enum Token_Type opcode,...);

//辅助函数
static void FetchToken();
static void MatchToken(enum Token_Type AToken);
static void SyntaxError(int case_of);
static void ErrMsg(unsigned LineNo, char* descrip, char* string);	//打印错误信息

//测试函数
static void PrintSyntaxTree(struct ExprNode* root);     //打印表达式的语法树

#endif
