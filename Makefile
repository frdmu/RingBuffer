CFLAGS += -Wall -Werror
demo: main.o libbuf.so
	cc -o demo -g main.o -lbuf -L. -Wl,-rpath=.
	#cc -o demo main.o -lbuf -lpthread -L. -Wl,-rpath=.
main.o: main.c
	cc -o main.o -c $(CFLAGS) main.c -I.
libbuf.so:
	cc -o libbuf.so -c $(CFLAGS) buf.c -I. -shared -fPIC
.PHONEY:
clean:
	-rm demo *.o *.so [0-9]*.jpg
