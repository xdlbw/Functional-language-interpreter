#include "semantic.h"
#include <tchar.h> 

#define MAX_CHARS 200

extern HDC hDC;							// 设备描述表句柄，全局变量
char SrcFilePath[MAX_CHARS];			// 用于存放源程序文件路径
TCHAR szClassName[] = _T("Compiler");   // 注册窗口类名

//检查源程序文件是否合法函数实现
//LPSTR：32bit指针 指向一个字符串，每个字符占1字节
static bool CheckSrcFile(LPSTR lpszCmdParam){
	FILE* file = NULL;
	if(strlen(lpszCmdParam) == 0){
		//弹出消息框，未指定源程序
		MessageBoxA(NULL, "Source program file not specified!", "error", MB_OK);
		return false;
	}
	if((file = fopen(lpszCmdParam, "r")) == NULL){
		//弹出消息框，源程序文件名错误
		MessageBoxA(NULL, "Error opening source program file!", "error", MB_OK);
		MessageBoxA(NULL, lpszCmdParam, "filename error!", MB_OK);
		return false;
	}
	else fclose(file);
	return true;
}

//消息处理函数
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message)                  
	{
		case WM_DESTROY:
			PostQuitMessage(0);       
			break;
		default:                      
			return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

//windows窗口初始化及程序主函数
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil){
	HWND hwnd;               //窗口句柄，包含窗口的属性
	MSG messages;            //消息
	WNDCLASSEX wincl;        //注册窗口类
	int i;

	//对窗口结构体属性进行初始化	
	wincl.hInstance = hThisInstance;		 			//本窗口的实例句柄
	wincl.lpszClassName = szClassName;		  			//指向类名称的指针
	wincl.lpfnWndProc = WindowProcedure;      			//窗口处理函数的指针
	wincl.style = CS_DBLCLKS;                           //窗口样式
	wincl.cbSize = sizeof(WNDCLASSEX);        			//WNDCLASSEX的大小，用sizeof（WNDCLASSEX）来获得准确的值
	wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);      //图标的句柄
	wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);    //和窗口类关联的小图标
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);        //光标的句柄
	wincl.lpszMenuName = NULL;                          //没有指向菜单的指针
	wincl.cbClsExtra = 0;                     			//指定紧跟在窗口类结构后的附加字节数=0
	wincl.cbWndExtra = 0;					  			//指定紧跟在窗口实例的附加字节数=0
	wincl.hbrBackground = CreateSolidBrush(black);      //窗口背景色为黑

	if (!RegisterClassEx(&wincl))                       //如果注册窗口失败
		return 0;

	hwnd = CreateWindowEx(   //对本窗口的实例句柄进行初始化
		0,                   //无窗口拓展风格
		szClassName,         //指向注册类名的指针
		_T("Compiler"),   	 //指向窗口名称的指针
		WS_OVERLAPPEDWINDOW, //默认窗口风格
		CW_USEDEFAULT,       //创建窗口在桌面上的水平位置
		CW_USEDEFAULT,       //创建窗口在桌面上的垂直位置
		740,                 //窗口宽度
		690,                 //窗口高度
		HWND_DESKTOP,        //子窗口
		NULL,                //没有菜单
		hThisInstance,       //此应用程序实例的句柄
		NULL                 //没有窗口创建数据
	);

	ShowWindow(hwnd, nFunsterStil);    //将窗口显示在屏幕上
	hDC = GetDC(hwnd);                 //检索设备上下文环境

	strcpy(SrcFilePath, "test2.txt");   //传入文件名

	if (!CheckSrcFile(SrcFilePath)) return 1;

	//调用绘图语言解释器
	Parser(SrcFilePath);
	
	//消息循环，不断地从消息队列中取出消息，并进行响应
	while (GetMessage(&messages, NULL, 0, 0)){
		//TranslateMessage函数用于将虚拟键消息转换为字符消息
		TranslateMessage(&messages);
		//DispatchMessage函数分派一个消息到窗口过程，由窗口过程函数对消息进行处理
		DispatchMessage(&messages);
	}
	return messages.wParam;
}