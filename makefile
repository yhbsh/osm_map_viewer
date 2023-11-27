CFLAGS=`pkg-config --cflags sdl2 sdl2_image libcurl` -Wall -Wextra
LIBS=`pkg-config --libs sdl2 sdl2_image libcurl`



clean: run
	@rm main

run: compile
	@./main

compile: main.c
	@clang $(CFLAGS) -O3 -o main main.c $(LIBS)
