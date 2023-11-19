CFLAGS=`pkg-config --cflags sdl2 sdl2_image` -Wall -Wextra
LIBS=`pkg-config --libs sdl2 sdl2_image` -lcurl



clean: run
	@rm main

run: compile
	@./main

compile: main.c
	@clang $(CFLAGS) -o main main.c $(LIBS)
