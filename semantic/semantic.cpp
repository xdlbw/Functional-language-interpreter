#include <windef.h>
#include "semantic.h"

HDC hDC;                            
extern double Parameter;      		//参数T的存储空间：记录t每次加一点的变化
extern double Origin_x, Origin_y;	//横纵坐标平移距离
extern double Scale_x, Scale_y;		//横纵比例因子
extern double Rot_angle;			//旋转角度

//计算表达式的值
//参数是表达式的根,后续遍历语法树,根据不同的节点类型计算当前根节点的值
double GetExprValue(struct ExprNode *root){		
	if(root == NULL) return 0.0;
	switch (root->OpCode)
	{
		case PLUS:
			return GetExprValue(root->Content.CaseOperator.Left) +
				GetExprValue(root->Content.CaseOperator.Right);
		case MINUS:
			return GetExprValue(root->Content.CaseOperator.Left) -
				GetExprValue(root->Content.CaseOperator.Right);
		case MUL:
			return GetExprValue(root->Content.CaseOperator.Left) *
				GetExprValue(root->Content.CaseOperator.Right);
		case DIV:
			return GetExprValue(root->Content.CaseOperator.Left) /
				GetExprValue(root->Content.CaseOperator.Right);
		case POWER:
			return pow(GetExprValue(root->Content.CaseOperator.Left),
				GetExprValue(root->Content.CaseOperator.Right));
		case FUNC:
			return(*root->Content.CaseFunc.MathFuncPtr)
				(GetExprValue(root->Content.CaseFunc.Child));
		case CONST_ID:
			return root->Content.CaseConst;
		case T:
			return *(root->Content.CaseParmPtr);
		default:
			return 0.0;
	}
}

//计算被绘制点的坐标
//参数分别表示：横坐标表达式语法树的根节点、纵坐标表达式语法树的根节点
//点横坐标值，起返回值的作用、点纵坐标值，起返回值的作用
static void CalcCoord(struct ExprNode* Hor_Exp, struct ExprNode* Ver_Exp, double& Hor_x, double& Ver_y){
	double HorCord, VerCord, Hor_tmp;
	HorCord = GetExprValue(Hor_Exp);
	VerCord = GetExprValue(Ver_Exp);		//根据表达式的语法树计算原始坐标
	HorCord *= Scale_x;
	VerCord *= Scale_y;						//进行比例变换
	Hor_tmp = HorCord * cos(Rot_angle) + VerCord * sin(Rot_angle);
	VerCord = VerCord * cos(Rot_angle) - HorCord * sin(Rot_angle);
	HorCord = Hor_tmp;						//进行旋转变换
	HorCord += Origin_x;
	VerCord += Origin_y;					//进行平移变换
	Hor_x = HorCord;
	Ver_y = VerCord;						//返回变换后点的坐标
}

//绘制一个点
void DrawPixel(unsigned long x, unsigned long y){
	SetPixel(hDC, x, y, yellow);
}

//循环绘制点的坐标
void DrawLoop(double Start, double End, double Step, struct ExprNode *HorPtr, struct ExprNode *VerPtr){
	extern double Parameter;
	double x, y;
	//把t在范围内的每一个值带入计算
	for(Parameter = Start; Parameter <= End; Parameter += Step){
		CalcCoord(HorPtr, VerPtr, x, y);				//计算要绘制店的实际坐标
		DrawPixel((unsigned long)x, (unsigned long)y);	//绘制这个点
	}
}

//删除一颗语法树
void DelExprTree(struct ExprNode* root){
	if(root == NULL) return;
	switch(root->OpCode)
	{
		case PLUS:
		case MINUS:
		case MUL:
		case DIV:
		case POWER:
			DelExprTree(root->Content.CaseOperator.Left);
			DelExprTree(root->Content.CaseOperator.Right);
			break;
		case FUNC:
			DelExprTree(root->Content.CaseFunc.Child);
			break;
		default:
			break;
	}
	delete(root);
}