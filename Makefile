TARGET=threadpool

# model loading/drawing
OBJS=ThreadPool.o threads.o utils/list.o

CC=gcc
CFLAGS=-Wall -O3 -std=gnu17 -I/usr/X11/include
LDFLAGS=-L/usr/X11/lib -lm -lpthread -lX11

all: $(TARGET)

debug: CFLAGS+= -DDEBUG -D_DEBUG -g -ggdb -O0
debug: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

.c: .o
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	-rm -f $(TARGET) $(OBJS)
