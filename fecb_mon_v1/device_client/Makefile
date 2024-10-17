# Define constants
INCLUDEDIR = -L. -lftd2xx -Wl,-rpath /usr/local/lib
CFLAGS_COMMON = -Wall -g -I.
CFLAGS_DEBUG = -DDEBUG
CFLAGS_MCM5OFF = -DMCM5OFF
LIBS = -lm

# Define the target executable
TARGET = deviceClientfecb

# Define the object files
OBJS = deviceClient.o wrapperCom.o fecb_eth.o fecb_mcm.o low_level_mcm.o response.o

# Define the compiler flags
CFLAGS = $(CFLAGS_COMMON) $(CFLAGS_DEBUG) $(CFLAGS_MCM5OFF)

# Define the link flags
LFLAGS = $(INCLUDEDIR) $(LIBS)

# Rule to build the target executable
$(TARGET): $(OBJS)
	gcc $(CFLAGS) -o $@ $^ $(LFLAGS)

# Rules to build the object files
%.o: %.c %.h
	gcc $(CFLAGS) -c $<

# Clean rule
clean:
	rm -f *.o $(TARGET)
