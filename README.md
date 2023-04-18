# Functional-language-interpreter
西电编译原理实验—简单函数语言解释器

整个项目分为三个模块，**词法分析器 scanner**，**语法分析器 parser**，**语义分析器 semantic**。其中，语法分析部分是语法制导翻译的基础，语法分析器是函数绘图语言解释器的核心，因此语法分析器的构造是整个解释器构造的关键，也是最难的一部分。词法分析和语义分析都是为其服务的。 

项目还包括**用于测试和绘图的主程序 main**，且最终的主程序是调用语法分析器对外的接口来实现绘图。 

每个部分都是一个模块，可独立运行。但要实现最终的绘图效果，需要用**分离式编译**将每个模块编译链接起来形成可执行文件。具体解释为：g++是 c++的 编译器，如果 cpp 文件是在一个文件夹中，那可以进入这个文件夹直接 g++ 加上源代码的名字就可以编译，但是项目是在不同文件夹下的，不同 cpp 文件又存 在包含其他模块的头文件，此时如果想分离式编译就要先把各个文件夹下的 cpp 文件先编译为可重定位目标文件，再将这些可重定位目标文件链接为一个可执行文件，每次这样操作的话是非常繁琐的，所以就用 makefile 将编译链接命令写在一个文件中，再使用 mingw32-make 执行 makefile 文件。所以，最终的项目结构如下：

```makefile
C:.
│ Makefile
│
├─lexer
│ 		scanner.cpp
│ 		scanner.h
│
├─parser
│ 		parser.cpp
│ 		parser.h
│
└─semantic
        main.cpp
        semantic.cpp
        semantic.h
```

Makefile 文件内容为：

```makefile
semantic:lexer/scanner.o parser/parser.o semantic/semantic.o semantic/
main.o
	g++ -o semantic scanner.o main.o parser.o semantic.o -lgdi32
lexer/scanner.o:lexer/scanner.cpp
	g++ -c lexer/scanner.cpp
parser/parser.o:parser/parser.cpp
	g++ -c parser/parser.cpp
semantic/semantic.o:semantic/semantic.cpp
	g++ -c semantic/semantic.cpp
semantic/main.o:semantic/main.cpp
	g++ -c semantic/main.cpp
```

`text1—text3.txt`为项目测试文件。

更多详情见编译原理实验报告。

编译原理提问为个人总结的项目验收时老师可能会问的问题，酌情参考~~
