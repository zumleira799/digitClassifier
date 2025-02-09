CC = gcc

CFLAGS = -c -fPIE
LDFLAGS = -no-pie -lX11 -lm

OBJS = main.o fileH.o

TARGET = main

all: $(TARGET)

fileH.o: fileHandler.s
	$(CC) $(CFLAGS) $< -o $@

main.o: main.c
	$(CC) -c main.c -o main.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)