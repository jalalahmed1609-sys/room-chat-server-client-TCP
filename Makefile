CFLAGS = -Wall -g -pthread
PROGS = client server

all: $(PROGS)

clean:
	rm -rf $(PROGS)
