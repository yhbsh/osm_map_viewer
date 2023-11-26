all: libcurl url_construct camera_view sdl2

libcurl: libcurl_test.c
	clang -o bin/libcurl libcurl_test.c -lcurl

url_construct: url_construct_test.c
	clang -o bin/url_construct url_construct_test.c

camera_view: camera_view_test.c
	clang -I$(INC_PATH) -o bin/camera_view camera_view_test.c -L$(LIB_PATH) -lsdl2 -lsdl2_image

sdl2: sdl2_test.c
	clang -I$(INC_PATH) -o bin/sdl2 sdl2_test.c -L$(LIB_PATH) -lsdl2
