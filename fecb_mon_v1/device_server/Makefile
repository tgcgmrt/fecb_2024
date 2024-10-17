TOPDIR  := $(shell cd ..; pwd)
include $(TOPDIR)/FECBethernet_device_server/Rules.make

APP = server

all: $(APP)

$(APP): server.c	
	$(CC) -o $(APP) server.c $(CFLAGS)	
	
clean:
	rm -f *.o ; rm $(APP)
