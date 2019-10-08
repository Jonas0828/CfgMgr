/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     trace.h
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

#define DEBUG_ERR          1
#define DEBUG_WARN         2
#define DEBUG_INFO         3

#define LOG_BUF_LEN_MAX    256


void trace (int logLevel, const char *fmt, ...);

const char *getLastCfgMgrErr(void);
