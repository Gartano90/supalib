CC = gcc
CFLAGS = -I./include -I/ucrt64/include -Wall -Wextra -g
LIBS = -L/ucrt64/lib -lcurl -lcjson

all: app

app: main.c src/supabase.c
	$(CC) $(CFLAGS) -o app main.c src/supabase.c $(LIBS)

clean:
	rm -f app