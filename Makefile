9wmextend: main.c
	gcc -Werror -O3 main.c -o 9wmextend -lX11

clean:
	rm -f 9wmextend
