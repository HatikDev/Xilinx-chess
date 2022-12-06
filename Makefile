.PHONY: test vga clean

test:
	gcc -DTESTENVIRONMENT `pkg-config --cflags gtk+-3.0` -o gui main.c graphics.c testenvironment.c `pkg-config --libs gtk+-3.0`

#vga:
#	gcc qwerty

#main.o: main.c

#graphics.o: graphics.c

#//testenvironment.o: testenvironment.c

clean:
	rm gui
