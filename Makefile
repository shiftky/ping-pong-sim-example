ifndef CODESBASE
$(error CODESBASE is undefined, see README.txt)
endif
ifndef CODESNET
$(error CODESNET is undefined, see README.txt)
endif
ifndef ROSS
$(error ROSS is undefined, see README.txt)
endif

# ross conflates CFLAGS with CPPFLAGS, so use just this one
override CPPFLAGS += $(shell $(ROSS)/bin/ross-config --cflags) -I$(CODESBASE)/include -I$(CODESNET)/include
CC = $(shell $(ROSS)/bin/ross-config --cc)
LDFLAGS = $(shell $(ROSS)/bin/ross-config --ldflags) -L$(CODESBASE)/lib -L$(CODESNET)/lib
LDLIBS = $(shell $(ROSS)/bin/ross-config --libs) -lcodes-net -lcodes-base

TARGET = example
OBJS = example.o client_lp.o server_lp.o

$(TARGET): $(OBJS)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) -o $@ -c $<

run: $(TARGET)
	./example --codes-config=example.conf --synch=1

optimistic-run: $(TARGET)
	mpirun -n 4 ./example --codes-config=example.conf --synch=3

clean:
	rm -f $(TARGET) $(OBJS)
