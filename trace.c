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
#include <sqlite3.h>


#define LOG_LOCK 	    		pthread_mutex_lock(&logMutex)
#define LOG_UNLOCK  			pthread_mutex_unlock(&logMutex)


static pthread_mutex_t logMutex;
static char lastCfgMgrErr[LOG_BUF_LEN_MAX + 1] = {0};



void trace(int logLevel, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(lastCfgMgrErr, LOG_BUF_LEN_MAX, fmt, ap);
    va_end(ap);

    printf(lastCfgMgrErr);
    printf("\n");
//    log_save(buffer);
}

const char *getLastCfgMgrErr(void)
{
    return (const char *)lastCfgMgrErr;
}

void logInit( void )
{
    if(0 != pthread_mutex_init(&logMutex, NULL))
	{
		printf("create logMutex failed\n");
	}
}

static sqlite3 *logOpen(char *fileName)
{
    uint32 index;
	sqlite3 *db;
	struct tm daytime;
	time_t sec, lsec = 0;
	uint32 ms, lms = 0;
	char buf[30];
	int result;
	char *errmsg = NULL;
	char *sql = "create table if not exists logtable( \
		OccurTime, \
		LogType,\
		Significance,\
		Content
		)";

    result = sqlite3_open(fileName, &db);  
	if(result != SQLITE_OK)
	{
		trace(DEBUG_ERR, "Can't open datebase(sqlite_errmsg : %s)\n", sqlite3_errmsg(db));
		return NULL;
	}
	result = sqlite3_exec(db, sql,0,0, &errmsg);

	if(result != SQLITE_OK)
		trace(DEBUG_ERR, "warning:Create table fail! May table \
		    logtable already result[%d] errmsg[%s].\n", result,  errmsg);

	return db;
}

static void logClose(sqlite3 * db)
{
    sqlite3_close(db);
}

void logWrite(logType typ, logSignificance sgnfcc, char *content)
{

#define MAX_MESSAGE_LEN		20
    sqlite3 *db;
	int result;
	char *errmsg = NULL;
	char sql[200];
	char tbuf[30] ={0};
	time_t tinow = time(NULL);
	struct tm daytime;
	struct timeval tv;

    LOG_LOCK;

    time2format(NULL, tbuf);
	memset(sql, 0x00, sizeof(sql));
	sprintf(sql, "insert into logtable values('%d','%d','%d','%s')", 
		tinow, typ, sgnfcc, content);

    db = logOpen(LOG_DATA_BASE_FILE_NAME);
	assert(db);
	result = sqlite3_exec(db,sql,0,0,&errmsg);
	if(result != SQLITE_OK) 
	{
		printf("Can't insert into datebase result[%d] errmsg[%s], sql[%s]\n", result,errmsg,sql);
		goto Write_Err;
	}

Write_Err:
    logClose(db);
    
	LOG_UNLOCK;
}


