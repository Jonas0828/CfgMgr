include ./config.mk

OBJS = cfgMgr.o message.o trace.o web.o ping.o

CfgMgr : $(OBJS)
	$(CC) $(OBJS) -o $@  $(LIBS)
	cp CfgMgr /home/chenxu/LOONGSON-2k1000/src/web/boa/
    
.c.o:
	echo Compiling $<
	$(CC) $(CFLAGS) $(INC) -c $<
    
clean:
	rm -vf *.o

