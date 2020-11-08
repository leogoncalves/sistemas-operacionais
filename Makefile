all: run

run: build
	@./main < input > output.md

build: 
	gcc scheduler.c -o main -Wall