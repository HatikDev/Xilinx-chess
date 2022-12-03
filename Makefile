.PHONY: all clean

all:
	gcc `pkg-config --cflags gtk+-3.0` -o gui main.c graphics.c testenvironment.c `pkg-config --libs gtk+-3.0`

clean:
	rm gui
