all: libcurl_test url_construct latlng_conv

clean: bin/*
	rm -f bin/*

libcurl_test: libcurl_test.c
	clang -o bin/libcurl_test libcurl_test.c -lcurl

url_construct: url_construct.c
	clang -o bin/url_construct url_construct.c

latlng_conv: latlng_conv.c
	clang -o bin/latlng_conv latlng_conv.c
