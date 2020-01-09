gcc = g++ -std=c++17 

all: test

test: 
	bison -d -o parser.c parser.y
	flex -o lexer.c lexer.l
	$(gcc) -c lexer.c
	$(gcc) -c parser.c