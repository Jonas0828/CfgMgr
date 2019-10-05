include ./config.mk

OBJS = cfgMgr.o message.o trace.o web.o

CfgMgr : $(OBJS)
	$(CC) $(OBJS) -o $@  $(LIBS)
    
.c.o:
	echo Compiling $<
	$(CC) $(CFLAGS) $(INC) -c $<
    
clean:
	rm -vf *.o

