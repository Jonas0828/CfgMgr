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
#define ADMIN_USER_NUM                   1
#define NORMAL_USER_NUM                  1
#define USER_NUM                         (ADMIN_USER_NUM + NORMAL_USER_NUM)
#define WEB_THREAD_STACK_SIZE            (512 *1024)

#define NET1_NAME                        "enp0s31f6"
//#define NET1_NAME                        "wlo1"
#define NET2_NAME                        "null"

