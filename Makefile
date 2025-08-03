CC=clang -std=c99
CFLAGS=-Wall -o0 -g
LDFLAGS=

obj=obj/fhedit.o
targets=fhedit

fhedit: $(obj)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o fhedit

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -c $^ -o $@
