

include ../config.mk

CFLAGS	+= -I. -I$(ACOSTOPDIR)/nvram -I$(ACOSTOPDIR)/shared -I$(ACOSTOPDIR)/include -Wall
CFLAGS	+= -D__LINUX__
#CFLAGS	+= -g 
CFLAGS	+= -s -O2
CFLAGS  += -s -O2
LDFLAGS += -L$(ROUTERDIR)/nvram -L$(INSTALLDIR)/nvram/usr/lib -lnvram
LDFLAGS += -L$(ACOSTOPDIR)/shared -L$(TARGETDIR)/shared/usr/lib 

CFLAGS  += -I$(SRCBASE)/include
LDFLAGS += -L$(SRCBASE)/router/shared -lshared
#Foxconn add start by Hank 07/30/2012
#For Kernel 2.6.36
ifeq ($(CONFIG_KERNEL_2_6_36),y)
LDFLAGS	+= -lgcc_s
endif
#Foxconn add end by Hank 07/30/2012

OBJS := usb_updatefw_main.o
OBJS += usb_updatefw.o
BIN  := usb_updateFW

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS)


clean:
	rm -f *~ $(BIN)
	rm -f *.o

