/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     cfgMgr.c
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/25
  * @defgroup cgi
  * @ingroup  cgi  
  * @brief    Configure management unit function cgi implement
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/25    Chenxu        None              File Create
  ****************************************************************************** 
  */

#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <CgicDebug.h>
#include <parameters.h>
#include <string.h>
#include <assert.h>
#include <message.h>
#include <time.h>
#include <cfgMgrCgi.h>
#include <../message.c>



#define stringLenZeroChkReturn(str)\
    if (strlen(str) == 0)\
    {\
        CGIDEBUG("%s line[%d] stringLenZeroChk Fail !!!\n", __FILE__, __LINE__);\
        CGICASSERT(0);\
        return -1;\
    }

typedef struct
{
    char *formName;
    int  (*func)(msg *m);
}formMethod;

/*
 * YYYY-MM-DD HH:MM:SS to time_t
 */
time_t format2time(char *format)
{
    struct tm daytime;

	daytime.tm_year = atoi(format) - 1900;
	daytime.tm_mon  = atoi(&format[5]) - 1;
	daytime.tm_mday = atoi(&format[8]);
	daytime.tm_hour = atoi(&format[11]);
	daytime.tm_min = atoi(&format[14]);
	daytime.tm_sec = atoi(&format[17]);

	return (mktime(&daytime));
}

static int netParamGet(netParam *net, int netNumber)
{
    char Lan_Auto[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char Lan_IP[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char Lan_Mask[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char Lan_GateWay[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char Lan_Mac[FORM_ELEMENT_STRING_LEN_MAX] = {0};

    if (netNumber == 1)
    {
        cgiFormString("Lan1_Auto", Lan_Auto, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan1_IP", Lan_IP, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan1_Mask", Lan_Mask, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan1_GateWay", Lan_GateWay, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan1_Mac", Lan_Mac, FORM_ELEMENT_STRING_LEN_MAX);

    }
    else
    {
        cgiFormString("Lan2_Auto", Lan_Auto, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan2_IP", Lan_IP, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan2_Mask", Lan_Mask, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan2_GateWay", Lan_GateWay, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan2_Mac", Lan_Mac, FORM_ELEMENT_STRING_LEN_MAX);
    }

    stringLenZeroChkReturn(Lan_Auto);
    stringLenZeroChkReturn(Lan_IP);
    stringLenZeroChkReturn(Lan_Mask);
    stringLenZeroChkReturn(Lan_GateWay);
    stringLenZeroChkReturn(Lan_Mac);

    if (strstr(Lan_Auto, "true"))
        net->isDhcp = TRUE;
    else if (strstr(Lan_Auto, "false"))
        net->isDhcp = FALSE;
    else
    {
        CGIDEBUG ("Lan%d_Auto invalid\n", netNumber);
        CGICASSERT(0);
        return -1;
    }
    inet_pton(AF_INET, Lan_IP, (void*)&net->ip);
    inet_pton(AF_INET, Lan_Mask, (void*)&net->mask);
    inet_pton(AF_INET, Lan_GateWay, (void*)&net->gateway);
    strncpy(net->mac, Lan_Mac, sizeof(net->mac));

    return 0;
}

static int lan1Test (msg *m)
{
    netParam *net = (netParam *)m->data; 

    m->type = MSGTYPE_LAN1TEST;

    return netParamGet(net, 1);
}


static int lan2Test (msg *m)
{
    netParam *net = (netParam *)m->data;

    m->type = MSGTYPE_LAN2TEST;

    return netParamGet(net, 2);
}

static int netConfigSave (msg *m)
{
    netParam *net = (netParam *)m->data;

    m->type = MSGTYPE_NETCONFIGSAVE;

    if (0 != netParamGet(net, 1))
    {
        CGIDEBUG("netParamGet 1 error !!!\n");
        return -1;
    }

    net++;
    
    if (0 != netParamGet(net, 2))
    {
        CGIDEBUG("netParamGet 2 error !!!\n");
        return -1;
    }

    return 0;
}

static int captureParamGet(captureParam *capture, int netNumber)
{
    char Lan_CaptureServiceStatus[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char Lan_AutoUpLoadEnable[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char Lan_AutoUpLoadPath[FORM_ELEMENT_STRING_LEN_MAX] = {0};

    if (netNumber == 1)
    {
        cgiFormString("Lan1_CaptureServiceStatus", Lan_CaptureServiceStatus, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan1_AutoUpLoadEnable", Lan_AutoUpLoadEnable, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan1_AutoUpLoadPath", Lan_AutoUpLoadPath, FORM_ELEMENT_STRING_LEN_MAX);
    }
    else
    {
        cgiFormString("Lan2_CaptureServiceStatus", Lan_CaptureServiceStatus, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan2_AutoUpLoadEnable", Lan_AutoUpLoadEnable, FORM_ELEMENT_STRING_LEN_MAX);
        cgiFormString("Lan2_AutoUpLoadPath", Lan_AutoUpLoadPath, FORM_ELEMENT_STRING_LEN_MAX);
    }

    stringLenZeroChkReturn(Lan_CaptureServiceStatus);
    stringLenZeroChkReturn(Lan_AutoUpLoadEnable);
    stringLenZeroChkReturn(Lan_AutoUpLoadPath);

    if (strstr(Lan_CaptureServiceStatus, "ON"))
        capture->isCapture = TRUE;
    else if (strstr(Lan_CaptureServiceStatus, "OFF"))
        capture->isCapture = FALSE;
    else
    {
        CGIDEBUG ("Lan%d_CaptureServiceStatus invalid\n", netNumber);
        CGICASSERT(0);
        return -1;
    }
    
    if (strstr(Lan_AutoUpLoadEnable, "Y"))
        capture->isAutoUpLoad = TRUE;
    else if (strstr(Lan_AutoUpLoadEnable, "N"))
        capture->isAutoUpLoad = FALSE;
    else
    {
        CGIDEBUG ("Lan%d_AutoUpLoadEnable invalid\n", netNumber);
        CGICASSERT(0);
        return -1;
    }

    strncpy(capture->autoUpLoadPath, Lan_AutoUpLoadPath, sizeof(capture->autoUpLoadPath));

    return 0;
}


static int netCapture(msg *m)
{
    captureParam *capture = (captureParam *)m->data;

    if (0 != captureParamGet(capture, 1))
    {
        CGIDEBUG("captureParamGet 1 error !!!\n");
        return -1;
    }

    capture++;

    if (0 != captureParamGet(capture, 2))
    {
        CGIDEBUG("captureParamGet 2 error !!!\n");
        return -1;
    }

    m->type = MSGTYPE_NETCAPTURE;

    return 0;
}

static int filterParamGet(filterParam *filter, int netNumber)
{
    char Lan_NetFilterServiceStatus[FORM_ELEMENT_STRING_LEN_MAX] = {0};

    if (netNumber == 1)
    {
        cgiFormString("Lan1_CaptureServiceStatus", Lan_NetFilterServiceStatus, FORM_ELEMENT_STRING_LEN_MAX);
    }
    else
    {
        cgiFormString("Lan2_CaptureServiceStatus", Lan_NetFilterServiceStatus, FORM_ELEMENT_STRING_LEN_MAX);
    }

    stringLenZeroChkReturn(Lan_NetFilterServiceStatus);

    if (strstr(Lan_NetFilterServiceStatus, "ON"))
        filter->isFilter = TRUE;
    else if (strstr(Lan_NetFilterServiceStatus, "OFF"))
        filter->isFilter = FALSE;
    else
    {
        CGIDEBUG ("Lan%d_NetFilterServiceStatus invalid\n", netNumber);
        CGICASSERT(0);
        return -1;
    }

    return 0;
}


static int netFilter(msg *m)
{
    filterParam *filter = (filterParam *)m->data;

    if (0 != filterParamGet(filter, 1))
    {
        CGIDEBUG("filterParamGet 1 error !!!\n");
        return -1;
    }

    filter++;

    if (0 != filterParamGet(filter, 2))
    {
        CGIDEBUG("filtereParamGet 2 error !!!\n");
        return -1;
    }

    m->type = MSGTYPE_NETCAPTURE;

    return 0;
}

static int fileLookUp(msg *m)
{
    fileLookUpCtrlInfo *fileLookUpCtrl = (fileLookUpCtrlInfo *)m->data;
    char NetNumber[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char StartTime[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char EndTime[FORM_ELEMENT_STRING_LEN_MAX] = {0};

    cgiFormString("NetNumber", NetNumber, FORM_ELEMENT_STRING_LEN_MAX);
    cgiFormString("StartTime", StartTime, FORM_ELEMENT_STRING_LEN_MAX);
    cgiFormString("EndTime", EndTime, FORM_ELEMENT_STRING_LEN_MAX);

    stringLenZeroChkReturn(NetNumber);
    stringLenZeroChkReturn(StartTime);
    stringLenZeroChkReturn(EndTime);

    fileLookUpCtrl->netNumber = atoi(NetNumber);

    fileLookUpCtrl->startTime = format2time(StartTime);
    fileLookUpCtrl->endTime = format2time(EndTime);

    m->type = MSGTYPE_FILELOOKUP;
    
    return 0;
}


static int diskInfo(msg *m)
{
    m->type = MSGTYPE_DISKINFO;
    
    return 0;
}

static int normalUserMgr(msg *m)
{
    normalUserMgrCtrlInfo *normalUserMgrCtrl = (normalUserMgrCtrlInfo *)m->data;
    char NewKey[FORM_ELEMENT_STRING_LEN_MAX] = {0};

    cgiFormString("NewKey", NewKey, FORM_ELEMENT_STRING_LEN_MAX);

    stringLenZeroChkReturn(NewKey);

    if (strlen(NewKey) > USR_KEY_LNE_MAX)
    {
        CGIDEBUG("normalUserMgr NewKey[%s] len > %d !!!\n", NewKey, USR_KEY_LNE_MAX);
        return -1;
    }

    strncpy(normalUserMgrCtrl->passwd, NewKey, USR_KEY_LNE_MAX);

    m->type = MSGTYPE_NORMALUSERMGR;
    
    return 0;
}

static int superUserMgr(msg *m)
{
    superUserMgrCtrlInfo *superUserMgrCtrl = (superUserMgrCtrlInfo *)m->data;
    char UserName[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char NewKey[FORM_ELEMENT_STRING_LEN_MAX] = {0};

    cgiFormString("UserName", UserName, FORM_ELEMENT_STRING_LEN_MAX);
    cgiFormString("NewKey", NewKey, FORM_ELEMENT_STRING_LEN_MAX);

    stringLenZeroChkReturn(UserName);
    stringLenZeroChkReturn(NewKey);

    if (strlen(UserName) > USR_KEY_LNE_MAX)
    {
        CGIDEBUG("superUserMgr UserName[%s] len > %d !!!\n", UserName, USR_KEY_LNE_MAX);
        return -1;
    }
    strncpy(superUserMgrCtrl->userName, UserName, USR_KEY_LNE_MAX);
    if (strlen(NewKey) > USR_KEY_LNE_MAX)
    {
        CGIDEBUG("superUserMgr NewKey[%s] len > %d !!!\n", NewKey, USR_KEY_LNE_MAX);
        return -1;
    }
    strncpy(superUserMgrCtrl->passwd, NewKey, USR_KEY_LNE_MAX);

    m->type = MSGTYPE_SUPERUSERMGR;
    
    return 0;
}


static int systemInfo(msg *m)
{
    m->type = MSGTYPE_SYSTEMINFO;
    
    return 0;
}

static int systemTimeGet(msg *m)
{
    m->type = MSGTYPE_SYSTEMTIMEGET;
    
    return 0;
}

static int systemTimeSet(msg *m)
{
    m->type = MSGTYPE_SYSTEMTIMESET;
    
    return 0;
}

static int getVersion(msg *m)
{
    m->type = MSGTYPE_GETVERSION;
    
    return 0;
}

static int factoryReset(msg *m)
{
    m->type = MSGTYPE_FACTORYRESET;
    
    return 0;
}

static int logLookUp(msg *m)
{
    logLookUpCtrlInfo *logLookUpCtrl = (logLookUpCtrlInfo *)m->data;
    char LogType[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char Significance[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char StartTime[FORM_ELEMENT_STRING_LEN_MAX] = {0};
    char EndTime[FORM_ELEMENT_STRING_LEN_MAX] = {0};

    cgiFormString("LogType", LogType, FORM_ELEMENT_STRING_LEN_MAX);
    cgiFormString("Significance", Significance, FORM_ELEMENT_STRING_LEN_MAX);
    cgiFormString("StartTime", StartTime, FORM_ELEMENT_STRING_LEN_MAX);
    cgiFormString("EndTime", EndTime, FORM_ELEMENT_STRING_LEN_MAX);

    stringLenZeroChkReturn(LogType);
    stringLenZeroChkReturn(Significance);
    stringLenZeroChkReturn(StartTime);
    stringLenZeroChkReturn(EndTime);    

    if (strstr(LogType, "all"))
        logLookUpCtrl->logType = LOGTYPE_ALL;
    else if (strstr(LogType, "user"))
        logLookUpCtrl->logType = LOGTYPE_USER;
    else if (strstr(LogType, "system"))
        logLookUpCtrl->logType = LOGTYPE_SYSTEM;
    else
    {
        CGIDEBUG ("LogType[%s] invalid\n", LogType);
        CGICASSERT(0);
        return -1;
    }

    if (strstr(Significance, "all"))
        logLookUpCtrl->logSignificance = LOGSIGNIFICANCE_ALL;
    else if (strstr(Significance, "general"))
        logLookUpCtrl->logSignificance = LOGSIGNIFICANCE_GENERAL;
    else if (strstr(Significance, "key"))
        logLookUpCtrl->logSignificance = LOGSIGNIFICANCE_KEY;
    else
    {
        CGIDEBUG ("Significance[%s] invalid\n", Significance);
        CGICASSERT(0);
        return -1;
    }

    logLookUpCtrl->startTime = format2time(StartTime);
    logLookUpCtrl->endTime = format2time(EndTime);

    m->type = MSGTYPE_LOGLOOKUP;
    
    return 0;
}

static const formMethod formMethodTable[] = 
{
    {"Form_Lan1Test", lan1Test},
    {"Form_Lan2Test", lan2Test},
    {"Form_NetConfigSave", netConfigSave},
    {"Form_NetCapture", netCapture},
    {"Form_NetFilter", netFilter},
    {"Form_FileLookUp", fileLookUp},
    {"Form_DiskInfo", diskInfo},
    {"Form_NormalUserMgr", normalUserMgr},
    {"Form_SuperUserMgr", superUserMgr},
    {"Form_SystemInfo", systemInfo},
    {"Form_SystemTimeGet", systemTimeGet},
    {"Form_SystemTimeSet", systemTimeSet},
    {"Form_GetVersion", getVersion},
    {"Form_FactoryReset", factoryReset},
    {"Form_LogLookUp", logLookUp},
};

static formMethod *formMethodLookUp (char *formName)
{
    int i;    

    for(i = 0; i < sizeof(formMethodTable)/sizeof(formMethodTable[0]); i++)
    {
        if (strstr(formName, formMethodTable[i].formName))
            return (formMethod *)&formMethodTable[i];
    }

    return NULL;
}

static void confirm2json (msg *m)
{
    confirmResponse *cfResp = (confirmResponse *)m->data;

    cgiHeaderContentType("text/html");
    
    fprintf(cgiOut, "{\r\n");
    fprintf(cgiOut, "\"status\":%d\r\n", cfResp->status);
    fprintf(cgiOut, "\"message\":%s", cfResp->errMessage);
    fprintf(cgiOut, "}");
}

static void msg2json(msg *m)
{
    

    switch(m->type)
    {
        case MSGTYPE_COMFIRM:
            confirm2json(m);
            break;
        default:
            break;
    }
}

int cgiMain()
{
    int                ret = 0;
    msg                m;
    msgID              mId;
    formMethod         *fmMethod;
    char formName      [FORM_ELEMENT_STRING_LEN_MAX] = {0};
    
	cgiFormString("FunctionName", formName, FORM_ELEMENT_STRING_LEN_MAX);

    if (!strlen(formName))
    {
        CGIDEBUG("FormName didn't found.\n");
        return -1;
    }

    if (NULL != (fmMethod = formMethodLookUp(formName)))
    {
        if (fmMethod->func(&m) != 0)
        {
            CGIDEBUG("fmMethod done failed !!!\n");
            return -1;
        }
    }
    else
    {
        CGIDEBUG("formMethodLookUp failed, formName :%s\n", formName);
        CGICASSERT(0);
        return -1;
    }


    if ((msgID)-1 == (mId = msgOpen (CGI_CFGMGR_MSG_NAME)))
	{
        CGIDEBUG("msgOpen %s failed !!!\n", CGI_CFGMGR_MSG_NAME);
        CGICASSERT(0);
        return -1;
    }

    if(0 != msgSend(mId, &m))
    {
        CGIDEBUG("msgSend failed !!!\n");
        ret = -1;
        goto closeMsg;
    }

    if(0 >= msgRecv(mId, &m))
    {
        CGIDEBUG("msgRecv failed !!!\n");
        ret = -1;
        goto closeMsg;
    }

    msg2json(&m);

closeMsg:
    msgClose(mId);
    
	return ret;
}

