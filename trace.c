/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     trace.c
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/30
  * @defgroup cfgMgr
  * @ingroup  log  
  * @brief    log function declear
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/30    Chenxu        None              File Create
  ****************************************************************************** 
  */
#include <trace.h>
#include <string.h>
#include <sys/prctl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>


void trace(int logLevel, const char *fmt, ...)
{
    va_list ap;
    char buffer[LOG_BUF_LEN_MAX+1];
    int len = 0;

    va_start(ap, fmt);
    vsnprintf(&buffer[len], LOG_BUF_LEN_MAX - len, fmt, ap);
    va_end(ap);
    
    printf(buffer);
    
//    log_save(buffer);
}

