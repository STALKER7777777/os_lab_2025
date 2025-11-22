# Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGETS = parallel_min_max process_memory zombie_demo parallel_sum

all: $(TARGETS)

parallel_min_max: parallel_min_max.c
	$(CC) $(CFLAGS) -o $@ $<

process_memory: process_memory.c
	$(CC) $(CFLAGS) -o $@ $<

parallel_sum: parallel_sum.c
	$(CC) $(CFLAGS) -pthread -o $@ $<

zombie_demo: zombi_demo.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
