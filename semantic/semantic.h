#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <windows.h>
#include <wingdi.h>
//VC_Compiler是用windows自带图形库实现的词法分析器，程序结果输出函数绘图语言解释器绘出的图像

#include "..\parser\parser.h"

#define yellow RGB(255,255,0)  	//黄色
#define black RGB(0,0,0)  		//黑色

extern double GetExprValue(struct ExprNode* root);			//获得表达式的值

//计算点的坐标
static void CalcCoord(struct ExprNode* Hor_Exp, struct ExprNode* Ver_Exp, double& Hor_x, double& Ver_y);

extern void DrawPixel(unsigned long x, unsigned long y);	//绘制一个点

//绘制图像：循环绘制由参数T指定的所有点
extern void DrawLoop(double Start, double End, double Step, struct ExprNode* HorPtr, struct ExprNode* VerPtr);

extern void DelExprTree(struct ExprNode* root);				//删除一棵树

#endif