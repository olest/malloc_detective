all: malloc-detective.so main


clean: 
	rm -rv main malloc-detective.so

malloc-detective.so: malloc-detective.c
	gcc -shared -fPIC -ldl -o $@ $<

main: main.c
	gcc -o $@ $<

