/**
 ******************************************************************************
 * @par Copyright (c): ChengDu BinHong Science & Technology Co.,ltd
 * @file     share.c
 * @author   Chenxu
 * @version  V1.0.0
 * @date     2019/10/08
 * @defgroup share
 * @ingroup  cfgMgr cgi  
 * @brief    this method will be used by CfgMgr & CGI
 * @par History
 * Date          Owner         BugID/CRID        Contents
 * 2019/10/08    Chenxu        None              File Create
 ****************************************************************************** 
 */

static int macString2Hex (char *str, char *mac)
{
    int i;
    int len;
	
    if ((len = strlen(str)) < 17)
		return -1;
	
	for (i = 0; i < 6; i++)
	{
	    sscanf(&str[i * 2 + i], "%02x", (unsigned int *)&mac[i]);
	}

	return 0;
}

static int macHex2String (char *mac, char *str)
{
    int i;

	for (i = 0; i < 6; i++)
	{
	    snprintf(&str[i*2 + i], 4, "%02x-", mac[i]);
	}
	str[i*2] = 0;

	return 0;
}

