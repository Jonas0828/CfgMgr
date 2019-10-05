/**
  ******************************************************************************
  * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
  * @file     message.c
  * @author   Chenxu
  * @version  V1.0.0
  * @date     2019/09/25
  * @defgroup cfgMgr
  * @ingroup  cfgMgr
  * @brief    message function implement
  * @par History
  * Date          Owner         BugID/CRID        Contents
  * 2019/09/25    Chenxu        None              File Create
  ****************************************************************************** 
  */

#include <message.h>
#include <stdlib.h>

msgID msgOpen (char *msgName)
{
    msgID mId;

    mId = (msgID)mq_open(msgName, O_RDWR|O_CREAT, NULL);

    return mId;
}


void msgClose (msgID id)
{
    mq_close((mqd_t)id);
}

int msgSend (msgID id, msg *m)
{
    if(mq_send((mqd_t)id, (char*)m, sizeof(msg), 1) == -1)
    {
        return -1;
    }
    
	return 0;
}

int msgRecv (msgID id, msg *m)
{
    int len;
    
    if ((len = (int)mq_receive((mqd_t)id, (char*)m, sizeof(msg), NULL)) == -1) 
    {
        return -1;
    }

	return len;
}

