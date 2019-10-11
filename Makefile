include ./config.mk

OBJS = cfgMgr.o message.o trace.o web.o ping.o

CfgMgr : $(OBJS) config.h	
	$(CC) $(OBJS) -o $@  $(LIBS)
	cp CfgMgr /home/chenxu/LOONGSON-2k1000/src/web/boa/
	cp CfgMgr ../webDebugWithZhang/
	rm -vf *.o
    
.c.o:
	echo Compiling $<
	$(CC) $(CFLAGS) $(INC) -c $<
    
clean:
	rm -vf *.o

