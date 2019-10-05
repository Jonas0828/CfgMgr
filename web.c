/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     web.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/09/25
 * @defgroup cfgMgr
 * @ingroup  cfgMgr  
 * @brief    Web Request Function implement
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/09/25    Chenxu        None              File Create
 ****************************************************************************** 
 */
#include <config.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <parameters.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/prctl.h>
#include <cfgMgr.h>
#include <linux/if.h> 
#include <linux/ethtool.h> 
#include <linux/sockios.h>
#include <net/route.h>
#include <trace.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <message.h>
#include <assert.h>





#define WEB_THREAD_NAME "CfgMgrWebThread"

pthread_t webThreadId = -1;


static BOOL is_netipvalid( in_addr_t IP )
{
	int i;
	struct in_addr addr;
	addr.s_addr = IP;

	i = inet_addr(inet_ntoa(addr));

	if((i == 0)||(i == 0xffffffff))
		return FALSE;
	else
		return TRUE;
}

static cfgMgrStatus set_addr( in_addr_t addr, int flag, int ethn)
{
    struct ifreq ifr;
    struct sockaddr_in sin;
	struct in_addr test;
    int sockfd;

	test.s_addr = addr;
	if(!is_netipvalid(addr))
	{
	    trace(DEBUG_ERR, "invalid IP[%s]!!!\n", inet_ntoa(test));
		return CFGMGR_IP_INVALID;
	}
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1)
	{
        trace(DEBUG_ERR, "socket fail!!!\n");
        return CFGMGR_ERR;
    }

	if(0 == ethn)
	    snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", "eth0");
	else if(1 == ethn)
		snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", "eth1");
    else
	{
	    trace(DEBUG_ERR, "netNumber [%d] is invalid!!!\n", ethn);
        return CFGMGR_NET_NUMBER_INVALID;
	}
    
	/* Read interface flags */
	if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0)
		trace(DEBUG_ERR,  "ifdown: shutdown \n");

    memset(&sin, 0, sizeof(struct sockaddr));

    sin.sin_family = AF_INET;

    sin.sin_addr.s_addr = addr;

    memcpy(&ifr.ifr_addr, &sin, sizeof(struct sockaddr));

	if(ioctl(sockfd, flag, &ifr) < 0)
	{
		trace(DEBUG_ERR, "fail to set address [%s]. \n", inet_ntoa(test));
        close(sockfd);
		return CFGMGR_ERR;
	}

    close(sockfd);

    return CFGMGR_OK;
}

static cfgMgrStatus set_gateway( in_addr_t addr, int ethn )
{
    int sockFd;
//    struct sockaddr_in sockaddr;
    struct rtentry rt;
	int ret;

    sockFd = socket(AF_INET, SOCK_DGRAM, 0);

	if (sockFd < 0)
	{
		trace(DEBUG_ERR, "set_gateway Socket create error.\n");
		return CFGMGR_ERR;
	}

	memset(&rt, 0, sizeof(struct rtentry));
    rt.rt_dst.sa_family = AF_INET; 
    ((struct sockaddr_in *)&rt.rt_gateway)->sin_family = AF_INET;    
	((struct sockaddr_in *)&rt.rt_gateway)->sin_addr.s_addr = addr; 

	((struct sockaddr_in *)&rt.rt_dst)->sin_family = AF_INET;
	
	((struct sockaddr_in *)&rt.rt_genmask)->sin_family = AF_INET;
    rt.rt_flags = RTF_GATEWAY;

	if ((ret = ioctl(sockFd, SIOCADDRT, &rt)) < 0)
	{
		struct in_addr inaddr;

		inaddr.s_addr = addr;
		trace(DEBUG_ERR, "ioctl(SIOCADDRT) [%s] error[%d]\n", inet_ntoa(inaddr), ret);
		close(sockFd);
		return CFGMGR_ERR;
	}

	return CFGMGR_OK;
}



static cfgMgrStatus lanTest (netParam *net, int netNumber)
{
    cfgMgrStatus status;

    /** set ip */
    if(CFGMGR_OK != (status = set_addr(net->ip, SIOCSIFADDR, netNumber)))
    {
        trace(DEBUG_ERR, "Lan %d test : set ip failed !!!\n", netNumber);
        return status;
    }
    /** set net mask */
    if(CFGMGR_OK != (status = set_addr(net->mask, SIOCSIFNETMASK, netNumber)))
    {
        trace(DEBUG_ERR, "Lan %d test : set net mask failed !!!\n", netNumber);
        return status;
    }
    /** set gateway */
    if(CFGMGR_OK != (status = set_gateway(net->gateway, netNumber)))
    {
        trace(DEBUG_ERR, "Lan %d test : set gateway failed !!!\n", netNumber);
        return status;
    }
    /** set mac */
    //TODO
    
    /** lan test */
    //TODO
    
    if (status == CFGMGR_OK)
        trace(DEBUG_INFO, "Lan %d test OK\n", netNumber);
    else
        trace(DEBUG_ERR, "Lan %d test ERROR(%d)\n", netNumber, status);
    
    return CFGMGR_OK;
}


void webProcess (void)
{
    int len;
    msgID mId;
    msg m;
    cfgMgrStatus status;
    
    /** set process name */
    prctl(PR_SET_NAME, WEB_THREAD_NAME);

    /** open message */
    if((msgID)-1 == (mId = msgOpen(CGI_CFGMGR_MSG_NAME)))
    {
        trace(DEBUG_ERR, "msgOpen %s error !!!\n", CGI_CFGMGR_MSG_NAME);
        return;
    }

    while(1)
    {
        if((len = msgRecv(mId, &m)) == -1)
        {
            trace(DEBUG_ERR, "msgRecv %s error !!!\n", CGI_CFGMGR_MSG_NAME);
            break;
        }

        switch(m.type)
        {
            case MSGTYPE_LAN1TEST:
                status = lanTest((netParam *)m.data, 1);
                m.type = MSGTYPE_COMFIRM;
                *(cfgMgrStatus*)m.data = status;
                break;
            case MSGTYPE_LAN2TEST:
                status = lanTest((netParam *)m.data, 1);
                m.type = MSGTYPE_COMFIRM;
                *(cfgMgrStatus*)m.data = status;
                break;
            case MSGTYPE_NETCONFIGSAVE:
                m.type = MSGTYPE_COMFIRM;
                *(cfgMgrStatus*)m.data = status;
                break;
            default:
                status = CFGMGR_NOT_SUPPORT;
                m.type = MSGTYPE_COMFIRM;
                *(cfgMgrStatus*)m.data = status;
                break;
        }
        

    }

    if(-1 == msgSend(mId, &m))
    {
        trace(DEBUG_ERR, "msgSend failed !!!\n");
    }

    msgClose(mId);
}
 
int webInit (void)
{
	int ret;
	pthread_attr_t attr;
	
	ret = pthread_attr_init(&attr);
	assert(ret == 0);
	ret = pthread_attr_setstacksize(&attr, WEB_THREAD_STACK_SIZE);
	assert(ret == 0);
	
	//创建相应web配置线程
	ret = pthread_create(&webThreadId, &attr, (void *) webProcess, NULL);
	if(ret != 0)
		trace(DEBUG_ERR,  "Create pthread error[%d]!\n", ret);

    return 0;
}
