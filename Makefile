CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -I.
TARGET = mgit

SRCS = src/main.c \
       src/utils/term_utils.c \
       src/utils/git_utils.c \
       src/commands/push.c \
       src/commands/tui_show.c \
       src/commands/clone.c \
       src/commands/update.c \
       src/commands/uninstall.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe 2>/dev/null || del /f /q src\*.o src\utils\*.o src\commands\*.o $(TARGET).exe 2>NUL || true

install: $(TARGET)
	mkdir -p $(HOME)/.mgit/bin
	cp $(TARGET)* $(HOME)/.mgit/bin/

.PHONY: all clean install
