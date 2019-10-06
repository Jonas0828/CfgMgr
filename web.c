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
#include <mxml.h>

#define ETHER_ADDR_LEN    6
#define UP    1
#define DOWN    0
#define BCD2HEX(x) (((x) >> 4) * 10 + ((x) & 0x0F))       /*20H -> 20*/
#define HEX2BCD(x) (((x) % 10) + ((((x) / 10) % 10) << 4))  /*20 -> 20H*/




#define WEB_THREAD_NAME "CfgMgrWebThread"
#define CONFIG_FILE_NAME "config.xml"

pthread_t webThreadId = -1;

static param pa;

static int macString2Hex (char *str, char (*mac)[6])
{
    int i;
    int len;
	
    if ((len = strlen(str)) < 12)
		return -1;
	
	for (i = 0; i < 6; i++)
	{
	    sscanf(&str[i * 2], "%02x", mac[i]);
	}

	return 0;
}

static int macHex2String (char (*mac)[6], char *str)
{
    int i;

	for (i = 0; i < 6; i++)
	{
	    snprintf(&str[i*2], "%02x", mac[i], 2);
	}
	str[i] = 0;

	return 0;
}

static cfgMgrStatus paramLoad(param *p)
{
	int fd = -1;
	int whitespace;
	FILE *fp;
	mxml_node_t *tree,
		*Config,
		*Lan1,Lan1_Auto,Lan1_IP,Lan1_Mask,Lan1_GateWay,Lan1_Mac,
			Lan1_CaptureServiceStatus,Lan1_AutoUpLoadEnable,Lan1_AutoUpLoadPath,Lan1_NetFilterServiceStatus,	
		*Lan2,Lan2_Auto,Lan2_IP,Lan2_Mask,Lan2_GateWay,Lan2_Mac,
			Lan2_CaptureServiceStatus,Lan2_AutoUpLoadEnable,Lan2_AutoUpLoadPath,Lan2_NetFilterServiceStatus,
		*User,Administrators,NomalUser,UserName,UserPws,
		*node;
	char *attr_value;

	if(NULL == (fp = fopen(CONFIG_FILE_NAME, "r")))
	{
		trace(DEBUG_ERR,  "fopen %s failed\n", CONFIG_FILE_NAME);
		return CFGMGR_ERR;
	}

	tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);
	fclose(fp);

	Config = mxmlFindElement(tree, tree, (const char *)"Config", NULL, NULL, MXML_DESCEND);
	assert(Config);

	/** Lan1 */
	Lan1 = mxmlFindElement(Config, Config, (const char *)"Lan1", NULL, NULL, MXML_DESCEND);
	assert(Lan1);
	    /** Lan1_Auto */
	    Lan1_Auto = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_Auto", NULL, NULL, MXML_DESCEND);
		assert(Lan1_Auto);
		node = mxmlGetLastChild(Lan1_Auto);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value,"true"))
			p->lan1.net.isDhcp = TRUE;
		else
			p->lan1.net.isDhcp = FALSE;
		trace(DEBUG_INFO, "Lan1 dhcp %s\n", p->lan1.net.isDhcp ? "true" : "false");
		/** Lan1_IP */
	    Lan1_IP = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_IP", NULL, NULL, MXML_DESCEND);
		assert(Lan1_IP);
		node = mxmlGetLastChild(Lan1_IP);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan1.net.ip);
        trace(DEBUG_INFO, "Lan1_IP 0x%08x\n", (int)p->lan1.net.ip);
		/** Lan1_Mask */
	    Lan1_Mask = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_Mask", NULL, NULL, MXML_DESCEND);
		assert(Lan1_Mask);
		node = mxmlGetLastChild(Lan1_Mask);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan1.net.mask);
        trace(DEBUG_INFO, "Lan1_Mask 0x%08x\n", (int)p->lan1.net.mask);
		/** Lan1_GateWay */
	    Lan1_GateWay = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_GateWay", NULL, NULL, MXML_DESCEND);
		assert(Lan1_GateWay);
		node = mxmlGetLastChild(Lan1_GateWay);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan1.net.gateway);
        trace(DEBUG_INFO, "Lan1_GateWay 0x%08x\n", (int)p->lan1.net.gateway);
		/** Lan1_Mac */
	    Lan1_Mac = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_Mac", NULL, NULL, MXML_DESCEND);
		assert(Lan1_Mac);
		node = mxmlGetLastChild(Lan1_Mac);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		macString2Hex(attr_value, p->lan1.net.mac);
        trace(DEBUG_INFO, "Lan1_Mac %s\n", attr_value);
		/** Lan1_CaptureServiceStatus */
		Lan1_CaptureServiceStatus = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_CaptureServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan1_CaptureServiceStatus);
		node = mxmlGetLastChild(Lan1_CaptureServiceStatus);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "ON"))
			p->lan1.capture.isCapture = TRUE;
		else
			p->lan1.capture.isCapture = FALSE;
        trace(DEBUG_INFO, "Lan1_CaptureServiceStatus %s\n", p->lan1.capture.isCapture ? "ON":"OFF");
		/** Lan1_AutoUpLoadEnable */
		Lan1_AutoUpLoadEnable = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_AutoUpLoadEnable", NULL, NULL, MXML_DESCEND);
		assert(Lan1_AutoUpLoadEnable);
		node = mxmlGetLastChild(Lan1_AutoUpLoadEnable);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "Y"))
			p->lan1.capture.isCapture = TRUE;
		else
			p->lan1.capture.isCapture = FALSE;
        trace(DEBUG_INFO, "Lan1_AutoUpLoadEnable %s\n", p->lan1.capture.isCapture ? "Y":"N");
		/** Lan1_AutoUpLoadPath */
		Lan1_AutoUpLoadPath = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_AutoUpLoadPath", NULL, NULL, MXML_DESCEND);
		assert(Lan1_AutoUpLoadPath);
		node = mxmlGetLastChild(Lan1_AutoUpLoadPath);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		strncpy(p->lan1.capture.autoUpLoadPath, attr_value, sizeof(p->lan1.capture.autoUpLoadPath));
        trace(DEBUG_INFO, "Lan1_AutoUpLoadPath %s\n", p->lan1.capture.autoUpLoadPath);
		/** Lan1_NetFilterServiceStatus */
		Lan1_NetFilterServiceStatus = mxmlFindElement(Lan1, Lan1, (const char *)"Lan1_NetFilterServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan1_NetFilterServiceStatus);
		node = mxmlGetLastChild(Lan1_NetFilterServiceStatus);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "ON"))
			p->lan1.filter.isFilter = TRUE;
		else
			p->lan1.filter.isFilter = FALSE;
        trace(DEBUG_INFO, "Lan1_CaptureServiceStatus %s\n", p->lan1.filter.isFilter ? "ON":"OFF");
		
						
	/** Lan2 */
    Lan2 = mxmlFindElement(Config, Config, (const char *)"Lan2", NULL, NULL, MXML_DESCEND);
	assert(Lan2);
	    /** Lan2_Auto */
	    Lan2_Auto = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_Auto", NULL, NULL, MXML_DESCEND);
		assert(Lan2_Auto);
		node = mxmlGetLastChild(Lan2_Auto);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value,"true"))
			p->lan2.net.isDhcp = TRUE;
		else
			p->lan2.net.isDhcp = FALSE;
		trace(DEBUG_INFO, "Lan2 dhcp %s\n", p->lan2.net.isDhcp ? "true" : "false");
		/** Lan2_IP */
	    Lan2_IP = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_IP", NULL, NULL, MXML_DESCEND);
		assert(Lan2_IP);
		node = mxmlGetLastChild(Lan2_IP);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan2.net.ip);
        trace(DEBUG_INFO, "Lan2_IP 0x%08x\n", (int)p->lan2.net.ip);
		/** Lan2_Mask */
	    Lan2_Mask = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_Mask", NULL, NULL, MXML_DESCEND);
		assert(Lan2_Mask);
		node = mxmlGetLastChild(Lan2_Mask);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan2.net.mask);
        trace(DEBUG_INFO, "Lan2_Mask 0x%08x\n", (int)p->lan2.net.mask);
		/** Lan2_GateWay */
	    Lan2_GateWay = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_GateWay", NULL, NULL, MXML_DESCEND);
		assert(Lan2_GateWay);
		node = mxmlGetLastChild(Lan2_GateWay);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		inet_pton(AF_INET, attr_value, (void*)&p->lan2.net.gateway);
        trace(DEBUG_INFO, "Lan2_GateWay 0x%08x\n", (int)p->lan2.net.gateway);
		/** Lan2_Mac */
	    Lan2_Mac = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_Mac", NULL, NULL, MXML_DESCEND);
		assert(Lan2_Mac);
		node = mxmlGetLastChild(Lan2_Mac);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		macString2Hex(attr_value, p->lan2.net.mac);
        trace(DEBUG_INFO, "Lan2_Mac %s\n", attr_value);
		/** Lan2_CaptureServiceStatus */
		Lan2_CaptureServiceStatus = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_CaptureServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan2_CaptureServiceStatus);
		node = mxmlGetLastChild(Lan2_CaptureServiceStatus);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "ON"))
			p->lan2.capture.isCapture = TRUE;
		else
			p->lan2.capture.isCapture = FALSE;
        trace(DEBUG_INFO, "Lan2_CaptureServiceStatus %s\n", p->lan2.capture.isCapture ? "ON":"OFF");
		/** Lan2_AutoUpLoadEnable */
		Lan2_AutoUpLoadEnable = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_AutoUpLoadEnable", NULL, NULL, MXML_DESCEND);
		assert(Lan2_AutoUpLoadEnable);
		node = mxmlGetLastChild(Lan2_AutoUpLoadEnable);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "Y"))
			p->lan2.capture.isCapture = TRUE;
		else
			p->lan2.capture.isCapture = FALSE;
        trace(DEBUG_INFO, "Lan2_AutoUpLoadEnable %s\n", p->lan2.capture.isCapture ? "Y":"N");
		/** Lan2_AutoUpLoadPath */
		Lan2_AutoUpLoadPath = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_AutoUpLoadPath", NULL, NULL, MXML_DESCEND);
		assert(Lan2_AutoUpLoadPath);
		node = mxmlGetLastChild(Lan2_AutoUpLoadPath);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		strncpy(p->lan2.capture.autoUpLoadPath, attr_value, sizeof(p->lan2.capture.autoUpLoadPath));
        trace(DEBUG_INFO, "Lan2_AutoUpLoadPath %s\n", p->lan2.capture.autoUpLoadPath);
		/** Lan2_NetFilterServiceStatus */
		Lan2_NetFilterServiceStatus = mxmlFindElement(Lan2, Lan2, (const char *)"Lan2_NetFilterServiceStatus", NULL, NULL, MXML_DESCEND);
		assert(Lan2_NetFilterServiceStatus);
		node = mxmlGetLastChild(Lan2_NetFilterServiceStatus);
		assert(node);
		attr_value = mxmlGetText(node, &whitespace);
		assert(attr_value);
		if(strstr(attr_value, "ON"))
			p->lan2.filter.isFilter = TRUE;
		else
			p->lan2.filter.isFilter = FALSE;
        trace(DEBUG_INFO, "Lan2_CaptureServiceStatus %s\n", p->lan2.filter.isFilter ? "ON":"OFF");
	
	/** User */
	User = mxmlFindElement(Config, Config, (const char *)"User", NULL, NULL, MXML_DESCEND);
	assert(User);
	    /** Administrators */
	    Administrators = mxmlFindElement(User, User, (const char *)"Administrators", NULL, NULL, MXML_DESCEND);
		assert(Administrators);
		    /** UserName */
		    UserName = mxmlFindElement(Administrators, Administrators, (const char *)"UserName", NULL, NULL, MXML_DESCEND);
			assert(UserName);
			node = mxmlGetLastChild(UserName);
			assert(node);
			attr_value = mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[0].userName, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, "Administrators UserName %s\n", p->users[0].userName);
			/** UserPws */
		    UserPws = mxmlFindElement(Administrators, Administrators, (const char *)"UserPws", NULL, NULL, MXML_DESCEND);
			assert(UserPws);
			node = mxmlGetLastChild(UserPws);
			assert(node);
			attr_value = mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[0].passwd, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, "Administrators UserPws %s\n", p->users[0].passwd);	    
        /** NomalUser */
	    NomalUser = mxmlFindElement(User, User, (const char *)"NomalUser", NULL, NULL, MXML_DESCEND);
		assert(NomalUser);
		    /** UserName */
		    UserName = mxmlFindElement(NomalUser, NomalUser, (const char *)"UserName", NULL, NULL, MXML_DESCEND);
			assert(UserName);
			node = mxmlGetLastChild(UserName);
			assert(node);
			attr_value = mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[1].userName, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, "NomalUser UserName %s\n", p->users[1].userName);
			/** UserPws */
		    UserPws = mxmlFindElement(NomalUser, NomalUser, (const char *)"UserPws", NULL, NULL, MXML_DESCEND);
			assert(UserPws);
			node = mxmlGetLastChild(UserPws);
			assert(node);
			attr_value = mxmlGetText(node, &whitespace);
			assert(attr_value);
			strncpy(p->users[1].passwd, attr_value, USR_KEY_LNE_MAX + 1);
	        trace(DEBUG_INFO, "NomalUser UserPws %s\n", p->users[1].passwd);
			
	mxmlDelete(tree);

	return CFGMGR_OK;
}

static cfgMgrStatus paramSave (param *p)
{
	cfgMgrStatus ret = CFGMGR_ERR;
	char filename[100];
	char filename1[100];
	char buffer[100] = {0};
	int 		i;
	int 		 fd = -1;
	FILE		*fp;
	mxml_node_t *tree, 
		*Config,
		*Lan1,Lan1_Auto,Lan1_IP,Lan1_Mask,Lan1_GateWay,Lan1_Mac,
		    Lan1_CaptureServiceStatus,Lan1_AutoUpLoadEnable,Lan1_AutoUpLoadPath,Lan1_NetFilterServiceStatus,	
		*Lan2,Lan2_Auto,Lan2_IP,Lan2_Mask,Lan2_GateWay,Lan2_Mac,
		    Lan2_CaptureServiceStatus,Lan2_AutoUpLoadEnable,Lan2_AutoUpLoadPath,Lan2_NetFilterServiceStatus,	
		*User,Administrators,NomalUser,UserName,UserPws,
		*node;
	char *attr_value;

	tree = mxmlNewXML("1.0");
	assert(tree);
	Config = mxmlNewElement(tree, "Config");
	assert(Config);
		Lan1 = mxmlNewElement(Config, "Lan1");
		assert(Lan1);
		    /** Lan1_Auto */
			Lan1_Auto = mxmlNewElement(Lan1, "Lan1_Auto");
			assert(Lan1_Auto);
			node = mxmlNewText(Lan1_Auto, 0, p->lan1.net.isDhcp ? "true" : "false");
			assert(node);
			/** Lan1_IP */
			Lan1_IP = mxmlNewElement(Lan1, "Lan1_IP");
			assert(Lan1_IP);
			inet_ntop(AF_INET, (void *)&p->lan1.net.ip, buffer, 16);
			node = mxmlNewText(Lan1_IP, 0, buffer);
			assert(node);
			/** Lan1_Mask */
			Lan1_Mask = mxmlNewElement(Lan1, "Lan1_Mask");
			assert(Lan1_Mask);
			inet_ntop(AF_INET, (void *)&p->lan1.net.mask, buffer, 16);
			node = mxmlNewText(Lan1_Mask, 0, buffer);
			assert(node);
			/** Lan1_GateWay */
			Lan1_GateWay = mxmlNewElement(Lan1, "Lan1_GateWay");
			assert(Lan1_GateWay);
			inet_ntop(AF_INET, (void *)&p->lan1.net.gateway, buffer, 16);
			node = mxmlNewText(Lan1_GateWay, 0, buffer);
			assert(node);
			/** Lan1_Mac */
			Lan1_Mac = mxmlNewElement(Lan1, "Lan1_Mac");
			assert(Lan1_Mac);
			macHex2String(p->lan1.net.mac, buffer);
			node = mxmlNewText(Lan1_Mac, 0, buffer);
			assert(node);
			/** Lan1_CaptureServiceStatus */
			Lan1_CaptureServiceStatus = mxmlNewElement(Lan1, "Lan1_CaptureServiceStatus");
			assert(Lan1_CaptureServiceStatus);
			if(p->lan1.capture.isCapture)
				strncpy(buffer, "ON", sizeof(buffer));
			else
				strncpy(buffer, "OFF", sizeof(buffer));
			node = mxmlNewText(Lan1_CaptureServiceStatus, 0, buffer);
			assert(node);
			/** Lan1_AutoUpLoadEnable */
			Lan1_AutoUpLoadEnable = mxmlNewElement(Lan1, "Lan1_AutoUpLoadEnable");
			assert(Lan1_AutoUpLoadEnable);
			if(p->lan1.capture.isAutoUpLoad)
				strncpy(buffer, "Y", sizeof(buffer));
			else
				strncpy(buffer, "N", sizeof(buffer));
			node = mxmlNewText(Lan1_AutoUpLoadEnable, 0, buffer);
			assert(node);
			/** Lan1_AutoUpLoadPath */
			Lan1_AutoUpLoadPath = mxmlNewElement(Lan1, "Lan1_AutoUpLoadPath");
			assert(Lan1_AutoUpLoadPath);
			node = mxmlNewText(Lan1_AutoUpLoadPath, 0, p->lan1.capture.autoUpLoadPath);
			assert(node);
			/** Lan1_NetFilterServiceStatus */
			Lan1_NetFilterServiceStatus = mxmlNewElement(Lan1, "Lan1_NetFilterServiceStatus");
			assert(Lan1_NetFilterServiceStatus);
			if(p->lan1.filter.isFilter)
				strncpy(buffer, "ON", sizeof(buffer));
			else
				strncpy(buffer, "OFF", sizeof(buffer));
			node = mxmlNewText(Lan1_NetFilterServiceStatus, 0, buffer);
			assert(node);
			
			
	    Lan2 = mxmlNewElement(Config, "Lan2");
		assert(Lan2);
		    /** Lan2_Auto */
			Lan2_Auto = mxmlNewElement(Lan2, "Lan2_Auto");
			assert(Lan2_Auto);
			node = mxmlNewText(Lan2_Auto, 0, p->lan2.net.isDhcp ? "true" : "false");
			assert(node);
			/** Lan2_IP */
			Lan2_IP = mxmlNewElement(Lan2, "Lan2_IP");
			assert(Lan2_IP);
			inet_ntop(AF_INET, (void *)&p->lan2.net.ip, buffer, 26);
			node = mxmlNewText(Lan2_IP, 0, buffer);
			assert(node);
			/** Lan2_Mask */
			Lan2_Mask = mxmlNewElement(Lan2, "Lan2_Mask");
			assert(Lan2_Mask);
			inet_ntop(AF_INET, (void *)&p->lan2.net.mask, buffer, 26);
			node = mxmlNewText(Lan2_Mask, 0, buffer);
			assert(node);
			/** Lan2_GateWay */
			Lan2_GateWay = mxmlNewElement(Lan2, "Lan2_GateWay");
			assert(Lan2_GateWay);
			inet_ntop(AF_INET, (void *)&p->lan2.net.gateway, buffer, 26);
			node = mxmlNewText(Lan2_GateWay, 0, buffer);
			assert(node);
			/** Lan2_Mac */
			Lan2_Mac = mxmlNewElement(Lan2, "Lan2_Mac");
			assert(Lan2_Mac);
			macHex2String(p->lan2.net.mac, buffer);
			node = mxmlNewText(Lan2_Mac, 0, buffer);
			assert(node);
			/** Lan2_CaptureServiceStatus */
			Lan2_CaptureServiceStatus = mxmlNewElement(Lan2, "Lan2_CaptureServiceStatus");
			assert(Lan2_CaptureServiceStatus);
			if(p->lan2.capture.isCapture)
				strncpy(buffer, "ON", sizeof(buffer));
			else
				strncpy(buffer, "OFF", sizeof(buffer));
			node = mxmlNewText(Lan2_CaptureServiceStatus, 0, buffer);
			assert(node);
			/** Lan2_AutoUpLoadEnable */
			Lan2_AutoUpLoadEnable = mxmlNewElement(Lan2, "Lan2_AutoUpLoadEnable");
			assert(Lan2_AutoUpLoadEnable);
			if(p->lan2.capture.isAutoUpLoad)
				strncpy(buffer, "Y", sizeof(buffer));
			else
				strncpy(buffer, "N", sizeof(buffer));
			node = mxmlNewText(Lan2_AutoUpLoadEnable, 0, buffer);
			assert(node);
			/** Lan2_AutoUpLoadPath */
			Lan2_AutoUpLoadPath = mxmlNewElement(Lan2, "Lan2_AutoUpLoadPath");
			assert(Lan2_AutoUpLoadPath);
			node = mxmlNewText(Lan2_AutoUpLoadPath, 0, p->lan2.capture.autoUpLoadPath);
			assert(node);
			/** Lan2_NetFilterServiceStatus */
			Lan2_NetFilterServiceStatus = mxmlNewElement(Lan2, "Lan2_NetFilterServiceStatus");
			assert(Lan2_NetFilterServiceStatus);
			if(p->lan2.filter.isFilter)
				strncpy(buffer, "ON", sizeof(buffer));
			else
				strncpy(buffer, "OFF", sizeof(buffer));
			node = mxmlNewText(Lan2_NetFilterServiceStatus, 0, buffer);
		/** User */
		User = mxmlNewElement(Config, "User");
		assert(User);
		    /** Administrators */
		    Administrators = mxmlNewElement(User, "Administrators");
			assert(Administrators);
			    /** UserName */
			    UserName = mxmlNewElement(Administrators, "UserName");
				assert(UserName);
				node = mxmlNewText(UserName, 0, p->users[0].userName);
				assert(node);
				/** UserPws */
			    UserPws = mxmlNewElement(Administrators, "UserPws");
				assert(UserPws);
				node = mxmlNewText(UserPws, 0, p->users[0].passwd);
				assert(node);
			/** NomalUser */
		    NomalUser = mxmlNewElement(User, "NomalUser");
			assert(NomalUser);
			    /** UserName */
			    UserName = mxmlNewElement(NomalUser, "UserName");
				assert(UserName);
				node = mxmlNewText(UserName, 0, p->users[1].userName);
				assert(node);
				/** UserPws */
			    UserPws = mxmlNewElement(NomalUser, "UserPws");
				assert(UserPws);
				node = mxmlNewText(UserPws, 0, p->users[1].passwd);
				assert(node);

	if(NULL == (fp = fopen(CONFIG_FILE_NAME, "w+")))
		goto Error;

	assert(mxmlSaveFile(tree, fp, MXML_NO_CALLBACK) != -1);
	fclose(fp);
	ret =  CFGMGR_OK;

Error:
	mxmlDelete(tree);
	return ret;
}


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

	if(1 == ethn)
	    snprintf(ifr.ifr_name, (sizeof(ifr.ifr_name) - 1), "%s", "eth0");
	else if(2 == ethn)
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

int get_mac_addr(char *ifname, char *mac)
{
    int fd, rtn;
    struct ifreq ifr;
    
    if( !ifname || !mac ) {
        return -1;
    }
    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) {
        perror("socket");
           return -1;
    }
    ifr.ifr_addr.sa_family = AF_INET;    
    strncpy(ifr.ifr_name, (const char *)ifname, IFNAMSIZ - 1 );

    if ( (rtn = ioctl(fd, SIOCGIFHWADDR, &ifr) ) == 0 )
        memcpy(mac, (unsigned char *)ifr.ifr_hwaddr.sa_data, 6);
    close(fd);
    return rtn;
}

static cfgMgrStatus setMacAddress(int netNumber, char *mac)
{
    int fd, rtn;
    struct ifreq ifr;
	char ifname[20];

    snprintf(ifname, sizeof(ifname), "eth%d", netNumber - 1);
	
    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) 
	{
        trace(DEBUG_ERR, "setMacAddress : Socket create error.\n");
		return CFGMGR_ERR;
    }
    ifr.ifr_addr.sa_family = ARPHRD_ETHER;
    strncpy(ifr.ifr_name, (const char *)ifname, IFNAMSIZ - 1 );
    memcpy((unsigned char *)ifr.ifr_hwaddr.sa_data, mac, 6);
    
    if((rtn = ioctl(fd, SIOCSIFHWADDR, &ifr) ) != 0)
    {
        trace(DEBUG_ERR, "setMacAddress : Set Mac Address(SIOCSIFHWADDR) error.\n");
		return CFGMGR_ERR;
    }
    close(fd);
    return CFGMGR_OK;
}

static cfgMgrStatus setNetParameters(netParam *net, int netNumber)
{
    cfgMgrStatus status;
    
    /** set ip */
    if(CFGMGR_OK != (status = set_addr(net->ip, SIOCSIFADDR, netNumber)))
    {
        trace(DEBUG_ERR, "net%d setNetParameters : set ip failed !!!\n", netNumber);
        return status;
    }
    /** set net mask */
    if(CFGMGR_OK != (status = set_addr(net->mask, SIOCSIFNETMASK, netNumber)))
    {
        trace(DEBUG_ERR, "net%d setNetParameters : set net mask failed !!!\n", netNumber);
        return status;
    }
    /** set gateway */
    if(CFGMGR_OK != (status = set_gateway(net->gateway, netNumber)))
    {
        trace(DEBUG_ERR, "net%d setNetParameters : set gateway failed !!!\n", netNumber);
        return status;
    }
    /** set mac */
    if(CFGMGR_OK != (status = setMacAddress(netNumber, net->mac)))
    {
        trace(DEBUG_ERR, "net%d setNetParameters : set mac address failed !!!\n", netNumber);
        return status;
    }

    return status;
}

int if_updown(char *ifname, int flag)
{
    int fd, rtn;
    struct ifreq ifr;        

    if (!ifname) {
        return -1;
    }

    fd = socket(AF_INET, SOCK_DGRAM, 0 );
    if ( fd < 0 ) {
        perror("socket");
        return -1;
    }
    
    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name, (const char *)ifname, IFNAMSIZ - 1 );

    if ( (rtn = ioctl(fd, SIOCGIFFLAGS, &ifr) ) == 0 ) {
        if ( flag == DOWN )
            ifr.ifr_flags &= ~IFF_UP;
        else if ( flag == UP ) 
            ifr.ifr_flags |= IFF_UP;
        
    }

    if ( (rtn = ioctl(fd, SIOCSIFFLAGS, &ifr) ) != 0) {
        perror("SIOCSIFFLAGS");
    }

    close(fd);

    return rtn;
}

int
ether_atoe(const char *a, unsigned char *e)
{
    char *c = (char *) a;
    int i = 0;

    memset(e, 0, ETHER_ADDR_LEN);
    for (;;) {
        e[i++] = (unsigned char) strtoul(c, &c, 16);
        if (!*c++ || i == ETHER_ADDR_LEN)
            break;
    }
    return (i == ETHER_ADDR_LEN);
}

char *
ether_etoa(const unsigned char *e, char *a)
{
    char *c = a;
    int i;

    for (i = 0; i < ETHER_ADDR_LEN; i++) {
        if (i)
            *c++ = ':';
        c += sprintf(c, "%02X", e[i] & 0xff);
    }
    return a;
}

static cfgMgrStatus lanTest (netParam *net, int netNumber)
{
    cfgMgrStatus status;
    netParam *netOrigin;

    /** set ip */
    if(CFGMGR_OK != (status = setNetParameters(net, netNumber)))
    {
        trace(DEBUG_ERR, "Lan %d test : setNetParameters failed !!!\n", netNumber);
        return status;
    }
    
    
    /** lan test */
    //TODO

	/** net parameter set bak */
    if (netNumber == 1)
        netOrigin = &pa->lan1.net;
    else
        netOrigin = &pa->lan2.net;
    if(CFGMGR_OK != (status = setNetParameters(netOrigin, netNumber)))
    {
        trace(DEBUG_ERR, "Lan %d test : setNetParameters failed !!!\n", netNumber);
        return status;
    }
    
    if (status == CFGMGR_OK)
        trace(DEBUG_INFO, "Lan %d test OK\n", netNumber);
    else
        trace(DEBUG_ERR, "Lan %d test ERROR(%d)\n", netNumber, status);
    
    return CFGMGR_OK;
}

static cfgMgrStatus doNetCapture(captureParam *capture, int netNumber)
{
    return CFGMGR_NOT_SUPPORT;
}


void webProcess (void)
{
    int len;
    msgID mId;
    msg m;
    cfgMgrStatus status;
    int netNumber;
    netParam *net;
    captureParam *capture;
    filterParam *filter;
    
    /** set process name */
    prctl(PR_SET_NAME, WEB_THREAD_NAME);

    /** open message */
    if((msgID)-1 == (mId = msgOpen(CGI_CFGMGR_MSG_NAME)))
    {
        trace(DEBUG_ERR, "msgOpen %s error !!!\n", CGI_CFGMGR_MSG_NAME);
        return;
    }

    /** load parameters */
    if(CFGMGR_OK != (status = paramLoad(&pa))
    {
        trace(DEBUG_ERR, "paramLoad failed(%d)\n", (int)status);
        goto webProcessExit;
    }
    /** set net parameters */
    for(netNumber = 1; netNumber <= 2; netNumber++)
    {
        if (netNumber == 1)
            net = &pa.lan1.net;
        else
            net = &pa.lan2.net;
        if(CFGMGR_OK != (status = setNetParameters(net, netNumber)))
        {
            trace(DEBUG_ERR, "webProcess setNetParameters net%d failed!!!\n", netNumber);
            goto webProcessExit;
        }
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
                do
                {
                    net = (netParam*)m.data;
                    if(CFGMGR_OK != (status = setNetParameters(net, 1)))
                    {
                        trace(DEBUG_ERR, "net 1 setNetParameters failed.\n");
                        break;
                    }
                    memcpy (&pa->lan1.net, net, sizeof(netParam));
                    net++;
                    if(CFGMGR_OK != (status = setNetParameters(net, 2)))
                    {
                        trace(DEBUG_ERR, "net 2 setNetParameters failed.\n");
                        break;
                    }
                    memcpy (&pa->lan2.net, net, sizeof(netParam));
                    status = paramSave(&pa);
                }while(0);
                m.type = MSGTYPE_COMFIRM;
                *(cfgMgrStatus*)m.data = status;
                break;
            case MSGTYPE_NETCAPTURE:                
                do
                {
                    capture = (captureParam *)m.data;
                    if(CFGMGR_OK != (status = doNetCapture(capture, 1)))
                    {
                        trace(DEBUG_ERR, "net 1 doNetCapture failed.\n");
                        break;
                    }
                    memcpy(&pa.lan1.capture, capture, sizeof(captureParam));
                    capture++;
                    if(CFGMGR_OK != (status = doNetCapture(capture, 2)))
                    {
                        trace(DEBUG_ERR, "net 2 doNetCapture failed.\n");
                        break;
                    }
                    memcpy(&pa.lan2.capture, capture, sizeof(captureParam));
                    status = paramSave(&pa);
                }
                while(0);
                m.type = MSGTYPE_COMFIRM;
                *(cfgMgrStatus*)m.data = status;
                break;
            default:
                status = CFGMGR_NOT_SUPPORT;
                m.type = MSGTYPE_COMFIRM;
                *(cfgMgrStatus*)m.data = status;
                break;
        }

        if(-1 == msgSend(mId, &m))
        {
            trace(DEBUG_ERR, "msgSend failed !!!\n");
            goto webProcessExit;
        }        
    }

webProcessExit:

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
	
	ret = pthread_create(&webThreadId, &attr, (void *) webProcess, NULL);
	if(ret != 0)
		trace(DEBUG_ERR,  "Create pthread error[%d]!\n", ret);

    return 0;
}

