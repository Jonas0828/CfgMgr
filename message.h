/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     message.h
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/25
  * @defgroup cfgMgr
  * @ingroup  cfgMgr
  * @brief    message declare
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/25    Chenxu        None              File Create
  ****************************************************************************** 
  */
#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <mqueue.h>
#include <parameters.h>
#include <trace.h>

#define MSG_DATA_LEN_MAX        4096

#define CGI_CFGMGR_MSG_NAME     "/cgiCfgMgrMessage"

typedef enum
{
    MSGTYPE_COMFIRM,
    MSGTYPE_LAN1TEST,
    MSGTYPE_LAN2TEST,
    MSGTYPE_NETCONFIGSAVE,
    MSGTYPE_NETCAPTURE,
    MSGTYPE_NETFILTER,
    MSGTYPE_FILELOOKUP,
    MSGTYPE_DISKINFO,
    MSGTYPE_NORMALUSERMGR,
    MSGTYPE_SUPERUSERMGR,
    MSGTYPE_SYSTEMINFO,
    MSGTYPE_SYSTEMTIMESET,
    MSGTYPE_SYSTEMTIMEGET,
    MSGTYPE_GETVERSION,
    MSGTYPE_FACTORYRESET,
    MSGTYPE_LOGLOOKUP
}msgType;

//typedef enum
//{
//    COMFIRM_OK,
//    COMFIRM_ERROR,
//}comfirmWord;

typedef enum
{
    CFGMGR_OK = 0,
    CFGMGR_ERR,
    CFGMGR_NET_NUMBER_INVALID,
    CFGMGR_IP_INVALID,
    CFGMGR_NOT_SUPPORT,
}cfgMgrStatus;


typedef struct
{
    int netNumber;
    time_t startTime;
    time_t endTime;
}fileLookUpCtrlInfo;


typedef struct
{
    char passwd[USR_KEY_LNE_MAX + 1];
}normalUserMgrCtrlInfo;

typedef struct
{
    char userName[USR_KEY_LNE_MAX + 1];
    char passwd[USR_KEY_LNE_MAX + 1];
}superUserMgrCtrlInfo;

typedef enum
{
    LOGTYPE_USER,
    LOGTYPE_SYSTEM,
    LOGTYPE_ALL,
}msgLogType;

typedef enum
{
    LOGSIGNIFICANCE_GENERAL,
    LOGSIGNIFICANCE_KEY,
    LOGSIGNIFICANCE_ALL
}msgLogSignificance;

typedef struct
{
    msgLogType logType;
    msgLogSignificance logSignificance;
    time_t startTime;
    time_t endTime;
}logLookUpCtrlInfo;

typedef struct
{
    cfgMgrStatus status;
    char errMessage[LOG_BUF_LEN_MAX + 1];
}confirmResponse;

typedef struct
{
    msgType type;
    char    data[MSG_DATA_LEN_MAX];
}msg;

typedef mqd_t msgID;

msgID msgOpen (char *msgName);

void msgClose (msgID id);

int msgSend (msgID id, msg *m);

int msgRecv (msgID id, msg *m);

#endif

