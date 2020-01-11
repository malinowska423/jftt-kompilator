gcc = g++ -std=c++17 

all:	clean compiler

compiler: 
	bison -d -o parser.c parser.y
	flex -o lexer.c lexer.l
	$(gcc) lexer.c parser.c symbol-table.cpp compiler.cpp -o compiler

clean: 
	rm -f lexer.c lexer.o parser.c parser.h parser.o compiler