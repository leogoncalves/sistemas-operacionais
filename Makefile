all: run

run: build
	@./main

build: 
	gcc queue.c -o main -Wall