CFLAGS = -Wall -g -pthread
PROGS = client server
PORT = 2301

all: $(PROGS)

check: $(PROGS)
	@echo "=== Running integration test ==="
	@./server &                                      \
	SERVER_PID=$$!;                                  \
	sleep 1;                                         \
	nc -z 127.0.0.1 $(PORT) && echo "PASS: server is listening on port $(PORT)" || (echo "FAIL: server not reachable"; kill $$SERVER_PID 2>/dev/null; exit 1); \
	kill $$SERVER_PID 2>/dev/null;                   \
	echo "=== Test passed ==="

distcheck: check

clean:
	rm -rf $(PROGS)
