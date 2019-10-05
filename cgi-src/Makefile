include ../config.mk
LIB=-lrt -pthread libcgic.a
SRCS=msg.c share.c
OBJS=$(SRCS:.c=.o)


all: libcgic.a cfgMgrCgi.cgi
	#cp *.cgi $(CGI_BIN)

#install: libcgic.a
	#cp *.cgi ../../tftpboot/
	#cp capture ../../tftpboot
	#cp cgic.h /usr/local/include
	#@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

cgic.o:cgic.c cgic.h
	$(CC) -c cgic.c
#CfgMgr.o:CfgMgr.c
#	$(CC) -c CfgMgr.c

cfgMgrCgi.cgi: cfgMgrCgi.c libcgic.a 
	$(CC) $(CFLAGS) -I$(INC) cfgMgrCgi.c -o cfgMgrCgi.cgi $(LIB)	

clean:
	rm -vf *.o
	rm -vf *.a
	rm -vf *.cgi
	rm -vf capture
	
%.o:%.c  
	$(CC) $(CFLAGS) $(DEVMGR_INC) -o $@ -c $<