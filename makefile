.PHONY: main
main:
	cc $@.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o $@

