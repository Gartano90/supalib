CC = gcc
# Flags base: Include locale, Warning attivi, Debug info
# Nota: Abbiamo rimosso -DCURL_STATICLIB perché ora usiamo la DLL
CFLAGS = -I./include -Wall -Wextra -g

# --- RILEVAMENTO SISTEMA OPERATIVO ---
ifeq ($(OS),Windows_NT)
    # CONFIGURAZIONE WINDOWS (MSYS2 / MinGW)
    TARGET = app.exe
    
    # Su Windows linkiamo solo curl. 
    # Il linker cercherà libcurl.dll.a o libcurl.lib per collegarsi alla DLL.
    # Non serve -lcjson perché lo compiliamo dai sorgenti.
    LIBS = -lcurl
else
    # CONFIGURAZIONE LINUX / MAC
    TARGET = app
    
    # Su Linux linkiamo curl standard.
    LIBS = -lcurl
endif

# --- SORGENTI ---
# Includiamo cJSON.c direttamente nella compilazione
SRCS = main.c src/supabase.c src/cJSON.c
OBJS = $(SRCS:.c=.o)

# --- REGOLE ---
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o main.o $(TARGET) app.exe