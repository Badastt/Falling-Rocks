.PHONY: main
main:
	cc src/$@.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o $@

