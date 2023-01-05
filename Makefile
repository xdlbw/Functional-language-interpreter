semantic:lexer/scanner.o parser/parser.o semantic/semantic.o semantic/main.o
	g++ -o semantic scanner.o main.o parser.o semantic.o -lgdi32
lexer/scanner.o:lexer/scanner.cpp
	g++ -c lexer/scanner.cpp
parser/parser.o:parser/parser.cpp
	g++ -c parser/parser.cpp
semantic/semantic.o:semantic/semantic.cpp
	g++ -c semantic/semantic.cpp
semantic/main.o:semantic/main.cpp
	g++ -c semantic/main.cpp