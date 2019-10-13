/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     config.h
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/09/25
 * @defgroup cfgMgr
 * @ingroup  cfgMgr  
 * @brief    config
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/09/25    Chenxu        None              File Create
 ****************************************************************************** 
 */

#define HOST_CX                          1
#define HOST_ZHANG                       2
#define HOST_LOONGSON                    3

#define HOST                             HOST_ZHANG
//#define HOST                             HOST_CX

#define ADMIN_USER_NUM                   1
#define NORMAL_USER_NUM                  1
#define USER_NUM                         (ADMIN_USER_NUM + NORMAL_USER_NUM)
#define WEB_THREAD_STACK_SIZE            (512 *1024)

#if (HOST == HOST_CX)
#define NET1_NAME                        "enp0s31f6"
#elif (HOST == HOST_ZHANG)
#define NET1_NAME                        "wlo1"
#else
#error "host config error !!!"
#endif
#define NET2_NAME                        "null"

#define FILE_NAME_LEN_MAX                30

#define LOG_BUF_LEN_MAX                  FILE_NAME_LEN_MAX

#define PAGE_RECORDS_MAX                 100

#define NET_FILES_PATH                   "../NetFiles/"

#define VERSION_STRING_LEN_MAX           10

#define LOG_DATA_BASE_FILE_NAME          "../cfgmgrLog/cfgmgrLog.db"

