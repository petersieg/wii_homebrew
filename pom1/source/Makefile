CC := gcc

CFLAGS := `sdl-config --cflags` -DLOCAL -O3 --std=c99
LDFLAGS := `sdl-config --libs` -lm 
EXTRA_FLAGS :=

HDRS :=

SRCS := $(wildcard *.c)

OBJS := $(SRCS:.c=.o)

EXEC := pom1

# default recipe
all: compile

compile: $(OBJS) $(HDRS) Makefile
	$(CC) -o $(EXEC) $(OBJS) $(LDFLAGS) $(EXTRA_FLAGS)

%.o: %.c $(HDRS) Makefile
	$(CC) -o $@ -c $< $(CFLAGS) $(EXTRA_FLAGS)

clean:
#	rm -f $(EXEC) $(OBJS)
	rm -f $(OBJS)

