CFLAGS=-O3

all: sqr
sqr: sqr.o
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $@ $<

