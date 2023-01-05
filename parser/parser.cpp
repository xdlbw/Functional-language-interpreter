#include "parser.h"
#include "..\semantic\semantic.h"

//全局变量区
double Parameter = 0, Origin_x = 0, Origin_y = 0, Scale_x = 1, Scale_y = 1, Rot_angle = 0;
static Token token;

/*语法分析主程序:
	调用词法分析器的GetToken函数(封装在FetchToken中)返回记号，然后使用核心产生式program()
    对记号流进行递归下降分析，判断记号流的结构是否符合文法规则*/

void Parser(char *SrcFilePtr){		//语法分析器的入口
	if (!InitScanner(SrcFilePtr)){	//初始化词法分析器
		printf("Open Source File Error !\n"); return;
	}
	printf("\nenter in Parser\n");
	FetchToken();					//先获得一个记号
	Program();						//然后进入Program递归子程序，递归下降分析
	printf("exit from Parser\n");
	CloseScanner();					//关闭词法分析器
	return;
}

//通过词法分析器接口GetToken获得一个记号
static void FetchToken(){
	token = GetToken();
	if (token.type == ERRTOKEN) 
		SyntaxError(1); 		//如果得到的记号是非法输入errtoken，则指出一个语法错误
}

//递归下降子程序：仅进行语法分析，不需要构造语法树
static void Program(){
	printf("enter in Program\n");
	while(token.type != NONTOKEN){			//词法分析器输出NONTOKEN表示已达记号流末尾
		Statement();						//程序有多个语句
		MatchToken(SEMICO);					//直到匹配到分隔符
		printf("matchtoken ;\n");
	}
	printf("exit from Program\n");
}

//语句Statment的递归子程序
//根据前面所得的记号流依次取记号，由于对应的保留字有对应的语法，例如ORIGIN后面要跟IS
//如果匹配不了语法，那就是语法错误了
static void Statement(){
	printf("enter in Statement\n");
	switch (token.type)
	{
		//根据匹配到的保留字进入对应匹配语句函数
		case ORIGIN: OriginStatement(); break;
		case SCALE: ScaleStatement(); break;
		case ROT:  RotStatement(); break;
		case FOR: ForStatement(); break;
		default: SyntaxError(2); 	//否则报错
	}
	printf("exit from Statement\n");
}

//语句OriginStatement的递归子程序
static void OriginStatement(){
	struct ExprNode *tmp;		//语法树节点的类型
	printf("enter in OriginStatement\n");

	MatchToken(ORIGIN);
	printf("matchtoken ORIGIN\n");
	MatchToken(IS);
	printf("matchtoken IS\n");
	MatchToken(L_BRACKET);
	printf("matchtoken (\n");

	printf("enter in Expression\n");
	tmp = Expression();  			//Tree_trace(tmp);
	PrintSyntaxTree(tmp);
	Origin_x = GetExprValue(tmp);	//获取横坐标点平移距离
	DelExprTree(tmp);				//删除一棵树
	printf("exit from Expression\n");

	MatchToken(COMMA);
	printf("matchtoken ,\n");

	printf("enter in Expression\n");
	tmp = Expression();   			//Tree_trace(tmp);
	PrintSyntaxTree(tmp);
	Origin_y = GetExprValue(tmp);	//获取纵坐标点平移距离
	DelExprTree(tmp);				//删除一棵树
	printf("exit from Expression\n");

	MatchToken(R_BRACKET);
	printf("matchtoken )\n");

    printf("exit from OriginStatement\n");
}

//语句ScaleStatement的递归子程序
static void ScaleStatement(void){
	struct ExprNode *tmp;
	printf("enter in ScaleStatement\n");

	MatchToken(SCALE);
	printf("matchtoken SCALE\n");
	MatchToken(IS);
	printf("matchtoken IS\n");
	MatchToken(L_BRACKET);
	printf("matchtoken (\n");

	printf("enter in Expression\n");
	tmp = Expression();  			//Tree_trace(tmp);
	PrintSyntaxTree(tmp);
	Scale_x = GetExprValue(tmp);	//获取横坐标的比例因子
	DelExprTree(tmp);
	printf("exit from Expression\n");

	MatchToken(COMMA);
	printf("matchtoken ,\n");

    printf("enter in Expression\n");
	tmp = Expression();   		 	//Tree_trace(tmp);
	PrintSyntaxTree(tmp);
	Scale_y = GetExprValue(tmp);	//获取纵坐标的比例因子
	DelExprTree(tmp);
	printf("exit from Expression\n");

	MatchToken(R_BRACKET);
	printf("matchtoken )\n");

    printf("exit from ScaleStatement\n");
}

//语句RotStatement的递归子程序
static void RotStatement(void){
	struct ExprNode *tmp;
	printf("enter in RotStatement\n");

	MatchToken(ROT);
	printf("matchtoken ROT\n");
	MatchToken(IS);
	printf("matchtoken IS\n");

	printf("enter in Expression\n");
	tmp = Expression();  			//Tree_trace(tmp);
	PrintSyntaxTree(tmp);
	Rot_angle = GetExprValue(tmp);	//获得旋转角度
	DelExprTree(tmp);
	printf("exit from Expression\n");

    printf("exit from RotStatement\n");
}

//语句ForStatement的递归子程序
//对右部文法符号的展开->遇到终结符号直接匹配，遇到非终结符就调用相应子程序
//ForStatement中唯一的非终结符是Expression，他出现在5个不同位置，分别代表循环的起始、终止、步长、横坐标、纵坐标，需要5个树节点指针保存这5棵语法树
static void ForStatement(void){
	//eg:for T from 0 to 2*pi step pi/50 draw (t, -sin(t));
	double Start, End, Step;		//绘图起点、终点、步长
	struct ExprNode *start_ptr, *end_ptr, *step_ptr, *x_ptr, *y_ptr;

	printf("enter in ForStatement\n");
	MatchToken(FOR);
	printf("matchtoken FOR\n");
	MatchToken(T);
	printf("matchtoken T\n");
	MatchToken(FROM);
	printf("matchtoken FROM\n");

	printf("enter in Expression\n");    
	start_ptr = Expression(); 			//Tree_trace(start_ptr),获得参数起点表达式的语法树
	PrintSyntaxTree(start_ptr);
	Start = GetExprValue(start_ptr);	//计算参数起点表达式的值
	DelExprTree(start_ptr);				//释放参数起点语法树所占空间
	printf("exit from Expression\n");

	MatchToken(TO);
	printf("matchtoken TO\n");

	printf("enter in Expression\n");    
	end_ptr = Expression(); 			//Tree_trace(end_ptr),构造参数终点表达式语法树
	PrintSyntaxTree(end_ptr);
	End = GetExprValue(end_ptr);		//计算参数终点表达式的值
	DelExprTree(end_ptr);				//释放参数终点语法树所占空间
	printf("exit from Expression\n");

	MatchToken(STEP);
	printf("matchtoken STEP\n");

	printf("enter in Expression\n");    
	step_ptr = Expression(); 			//Tree_trace(step_ptr),构造参数步长表达式语法树
	PrintSyntaxTree(step_ptr);
	Step = GetExprValue(step_ptr);		//计算参数步长表达式的值
	DelExprTree(step_ptr);				//释放参数步长语法树所占空间
	printf("exit from Expression\n");

	MatchToken(DRAW);
	printf("matchtoken DRAW\n");
	MatchToken(L_BRACKET);
	printf("matchtoken (\n");

	printf("enter in Expression\n");  
	x_ptr = Expression(); 				//Tree_trace(x_ptr);
	PrintSyntaxTree(x_ptr);
	printf("exit from Expression\n");

	MatchToken(COMMA);
	printf("matchtoken ,\n");

	printf("enter in Expression\n");  
	y_ptr = Expression();				//Tree_trace(y_ptr);
	PrintSyntaxTree(y_ptr);
	printf("exit from Expression\n");

	MatchToken(R_BRACKET);
	printf("matchtoken )\n");

    printf("exit from ForStatement\n");

	DrawLoop(Start, End, Step, x_ptr, y_ptr); 	//绘制图形
	DelExprTree(x_ptr);							//释放横坐标语法树所占空间
	DelExprTree(y_ptr);							//释放纵坐标语法树所占空间
}

//（二元加减运算）表达式Expression的递归子程序，与上边不太相同的是，表达式需要为其构造语法树
//把函数设计为语法树节点的指针，在函数内引进2个语法树节点的指针变量，分别作为Expression左右操作数（Term）的语法树节点指针
//表达式应该是由正负号或无符号开头、由若干个项以加减号连接而成。

static struct ExprNode *Expression(){		//展开右部，并且构造语法树
	struct ExprNode *left, *right;			//左右子树节点指针
	Token_Type token_tmp;					//当前记号

	left = Term();							//分析左操作数得到其语法树，左操作数是一个乘除运算表达式
	while (token.type == PLUS || token.type == MINUS){
		token_tmp = token.type;				//当前记号是加/减
		MatchToken(token_tmp);				//匹配记号
		right = Term();						//分析右操作数得到其语法树，右操作数是一个乘除运算表达式
		left = MakeExprNode(token_tmp, left, right);	//构造运算的语法树，结果为左子树
	}
	return left;							//返回的是当前记号节点
}

//乘除运算表达式Term的递归子程序
//项是由若干个因子以乘除号连接而成
static struct ExprNode *Term(){
	struct ExprNode *left, *right;
	Token_Type token_tmp;

	left = Factor();
	while (token.type == MUL || token.type == DIV){
		token_tmp = token.type;
		MatchToken(token_tmp);
		right = Factor();
		left = MakeExprNode(token_tmp, left, right);
	}
	return left;
}

//一元加减运算Factor的递归子程序
//因子则可能是一个标识符或一个数字，或是一个以括号括起来的子表达式
static struct ExprNode *Factor(){
	struct ExprNode *left, *right;
	if (token.type == PLUS){			//匹配一元加运算
		MatchToken(PLUS);
		right = Factor();
	}
	else if (token.type == MINUS){		//表达式退化为仅有右操作数的表达式
		MatchToken(MINUS);
		right = Factor();
		left = new ExprNode;
		left->OpCode = CONST_ID;
		left->Content.CaseConst = 0.0;
		right = MakeExprNode(MINUS, left, right);
	}
	else right = Component();//匹配非终结符Component
	return right;
}

//幂运算表达式Component的递归子程序
static struct ExprNode *Component(){
	struct ExprNode *left, *right;
	left = Atom();
	if (token.type == POWER){
		MatchToken(POWER);
		right = Component();		//递归调用Component以实现POWER的右结合
		left = MakeExprNode(POWER, left, right);
	}
	return left;
}

//原子表达式Atom的递归子程序，包括分隔符 函数 常数 参数
static struct ExprNode *Atom(){
	struct Token t = token;
	struct ExprNode *address, *tmp;
	switch (token.type)
	{
		case CONST_ID:
			MatchToken(CONST_ID);
			address = MakeExprNode(CONST_ID, t.value);
			break;
		case T:
			MatchToken(T);
			address = MakeExprNode(T);
			break;
		case FUNC:
			MatchToken(FUNC);
			MatchToken(L_BRACKET);
			tmp = Expression();  	//Tree_trace(tmp);
			address = MakeExprNode(FUNC, t.FuncPtr, tmp);
			MatchToken(R_BRACKET);
			break;
		case L_BRACKET:
			MatchToken(L_BRACKET);
			address = Expression(); // Tree_trace(address);
			MatchToken(R_BRACKET);
			break;
		default:
			SyntaxError(3);
	}
	return address;
}

//构建语法树，根据函数的参数构造不同的节点，并返回指向该节点的指针
//包含了头文件stdarg.h，故此函数参数可变
static struct ExprNode * MakeExprNode(enum Token_Type opcode, ...){
	struct ExprNode *ExprPtr = new(struct ExprNode);	//为新节点开辟空间
	ExprPtr->OpCode = opcode;							//向节点写入记号类别

	/*
    首先定义一具VA_LIST型的变量，是指向参数的指针
    然后用VA_START宏初始化变量刚定义的VA_LIST变量
    然后用VA_ARG返回可变的参数，VA_ARG的第二个参数为要返回的参数的类型
    最后用VA_END宏结束可变参数的获取。
    */

	va_list ArgPtr;				//指向可变函数的参数的指针
	va_start(ArgPtr, opcode);	//初始化va_list变量，第一个参数也就是固定参数为opcode
	switch (opcode)				//根据记号的类别构造不同的节点
	{
		case CONST_ID://常数节点
			ExprPtr->Content.CaseConst = (double)va_arg(ArgPtr, double);	//返回可变参数，可变参数类型是常数
			break;
		case T://参数T节点
			ExprPtr->Content.CaseParmPtr = &Parameter;	//返回可变参数，可变参数类型是参数T
			break;
		case FUNC://函数调用节点
			ExprPtr->Content.CaseFunc.MathFuncPtr = (FuncPtr)va_arg(ArgPtr, FuncPtr);	//可变参数类型是对应函数的指针
			ExprPtr->Content.CaseFunc.Child = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);	//可变参数类型是节点
			break;
		default://二元运算节点
			ExprPtr->Content.CaseOperator.Left = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);	//可变参数类型是节点
			ExprPtr->Content.CaseOperator.Right = (struct ExprNode *)va_arg(ArgPtr, struct ExprNode *);	//同上
			break;
	}
	va_end(ArgPtr);	//结束可变参数的获取
	return ExprPtr;
}

//匹配当前记号
static void MatchToken(enum Token_Type The_Token){
	if (token.type != The_Token){
		SyntaxError(4);			//若失败，指出语法错误
	}
	FetchToken();				//若成功，获取下一个
}

//语法错误处理
static void SyntaxError(int case_of){
	switch (case_of)
	{
		case 1:   	//由FetchToken函数抛出：词法分析获取的记号类型为ERRTOKEN，即不构成正确的单词
			ErrMsg(LineNo, (char*)"Incorrect word:", token.lexeme);
			break;
		case 2: 	//由Statement函数抛出：不符合文法定义，句子开头的Token不正确
			ErrMsg(LineNo, (char*)"Incorrect start of sentence:", token.lexeme);
			break;
		case 3:     //由Atom函数抛出：不符合文法定义，表达式不正确
			ErrMsg(LineNo, (char*)"Incorrect Expression:", token.lexeme);
			break;
		case 4: 	//由MatchToken函数抛出：不符合文法定义，语句错误（保留字、标点符等匹配失败）
			ErrMsg(LineNo, (char*)"Incorrect Match:", token.lexeme);
			break;
	}
}

//打印错误信息
static void ErrMsg(unsigned LineNo, char *descrip, char *string){
	char msg[256];
	memset(msg, 0, 256);
	sprintf(msg, "Line No %d:%s %s !", LineNo, descrip, string);

	MessageBoxA(NULL, msg, "error!", MB_OK);

	CloseScanner();
	exit(1);
}

//先序遍历打印语法树，根-左-右
static void PrintSyntaxTree(struct ExprNode *root){
	switch (root->OpCode)
	{
		case CONST_ID: printf("\t%f\n", root->Content.CaseConst); break;
        case T: printf("\t%s\n", "T"); break;
        case FUNC:{
            printf("\t%x\n", root->Content.CaseFunc.MathFuncPtr);
            PrintSyntaxTree(root->Content.CaseFunc.Child);
        } break;
        case PLUS:{
            printf("  +\n");
            PrintSyntaxTree(root->Content.CaseOperator.Left);
            PrintSyntaxTree(root->Content.CaseOperator.Right);
        } break;
        case MINUS:{
            printf("  -\n");
            PrintSyntaxTree(root->Content.CaseOperator.Left);
            PrintSyntaxTree(root->Content.CaseOperator.Right);
        } break;
        case MUL:{
            printf("  *\n");
            PrintSyntaxTree(root->Content.CaseOperator.Left);
            PrintSyntaxTree(root->Content.CaseOperator.Right);
        } break;
        case DIV:{
            printf("  /\n");
            PrintSyntaxTree(root->Content.CaseOperator.Left);
            PrintSyntaxTree(root->Content.CaseOperator.Right);
        } break;
        case POWER:{
            printf("  **\n");
            PrintSyntaxTree(root->Content.CaseOperator.Left);
            PrintSyntaxTree(root->Content.CaseOperator.Right);
        } break;
	}
}