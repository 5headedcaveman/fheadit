CC=clang -std=c99
CFLAGS=-Wall -o0 -g
LDFLAGS=

obj=obj/fhedit.o
targets=build/fhedit

build/fhedit: $(obj)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@
