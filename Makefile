CFLAGS=-O3 -Wall

all: sqr
sqr: sqr.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

