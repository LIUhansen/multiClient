/*!
 *****************************************************************************
 *
 *  Copyright © 2017-2018 none. All Rights Reserved.
 *
 * \file      common.h
 * \author    HansenLiu
 * \version   1.0
 * \date      2021年9月8日
 * \brief     一些通过的宏定义
 *
 *----------------------------------------------------------------------------
 * \attention
 *
 *
 *****************************************************************************
 */

/*****************************************************************************
 change history: 
    1.date  : 2021年9月8日
      author: HansenLiu
      change: create file

*****************************************************************************/


#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/select.h>
#include <termios.h>
#include <math.h>
#include <sys/types.h>
#include <inttypes.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/sysinfo.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <sys/poll.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <semaphore.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <stdint.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <time.h>
#include <sched.h>
#include <linux/types.h>
#include <linux/errqueue.h>
#include <linux/sockios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <netinet/tcp.h>
#include <poll.h>


#define MAX_LOG_SIZE	    (5 * 1024)
#define MAX_LOG_INFO_SIZE	(5 * 1024UL * 1024UL)


static FILE 	*dmail_debug;
static FILE 	*dmail_info;


#define ERR_EXIT(m)         \
	do                      \
	{                       \
		perror(m);          \
		exit(EXIT_FAILURE); \
	} while (0)


//调试信息的打印
/* 默认黑色 */
#define Debug(format, ...)						\
	do													\
	{													\
		time_t seconds; 								\
		struct tm *tmp = NULL;							\
		int size = 0;									\
		if(dmail_debug) 								\
		{												\
			size = ftell(dmail_debug);					\
			if(size > MAX_LOG_SIZE) 					\
			{											\
				rewind(dmail_debug);					\
			}											\
		}												\
		seconds = time(NULL);							\
		tmp = gmtime(&seconds); 						\
		printf("\r[\033[0mDEBUG\033[0m][%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n",	\
			tmp->tm_year + 1900,									\
			tmp->tm_mon + 1,										\
			tmp->tm_mday,											\
			tmp->tm_hour,											\
			tmp->tm_min,											\
			tmp->tm_sec,											\
		##__VA_ARGS__,											\
			__FILE__,												\
			__LINE__												\
			);														\
		if(dmail_debug) 											\
		{															\
			fprintf(dmail_debug, "[%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n", 	\
				tmp->tm_year + 1900,									\
				tmp->tm_mon + 1,										\
				tmp->tm_mday,											\
				tmp->tm_hour,											\
				tmp->tm_min,											\
				tmp->tm_sec,											\
		    ##__VA_ARGS__,											\
				__FILE__,												\
				__LINE__												\
			);															\
		}																\
	} while(0)

/* 蓝色 */
#define Info(format, ...)						\
	do													\
	{													\
		time_t seconds; 								\
		struct tm *tmp = NULL;							\
		int size = 0;									\
		if(dmail_info)									\
		{												\
			size = ftell(dmail_info);					\
			if(size > MAX_LOG_INFO_SIZE)				\
			{											\
				rewind(dmail_info); 					\
			}											\
		}												\
		seconds = time(NULL);							\
		tmp = gmtime(&seconds); 						\
		printf("\r[\033[34mINFO \033[0m][%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n",	\
			tmp->tm_year + 1900,									\
			tmp->tm_mon + 1,										\
			tmp->tm_mday,											\
			tmp->tm_hour,											\
			tmp->tm_min,											\
			tmp->tm_sec,											\
		##__VA_ARGS__,											\
			__FILE__,												\
			__LINE__												\
			);														\
		if(dmail_info)												\
		{															\
			fprintf(dmail_info, "[%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n",	\
				tmp->tm_year + 1900,								\
				tmp->tm_mon + 1,									\
				tmp->tm_mday,										\
				tmp->tm_hour,										\
				tmp->tm_min,										\
				tmp->tm_sec,										\
    		##__VA_ARGS__,										\
				__FILE__,											\
				__LINE__											\
				);													\
		}															\
	} while(0)

/* 黄色 */
#define Warn(format, ...)						\
	do													\
	{													\
		time_t seconds; 								\
		struct tm *tmp = NULL;							\
		int size = 0;									\
		if(dmail_debug) 								\
		{												\
			size = ftell(dmail_debug);					\
			if(size > MAX_LOG_SIZE) 					\
			{											\
				rewind(dmail_debug);					\
			}											\
		}												\
		seconds = time(NULL);							\
		tmp = gmtime(&seconds); 						\
		printf("\r[\033[33mWARN \033[0m][%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n",	\
			tmp->tm_year + 1900,									\
			tmp->tm_mon + 1,										\
			tmp->tm_mday,											\
			tmp->tm_hour,											\
			tmp->tm_min,											\
			tmp->tm_sec,											\
		##__VA_ARGS__,											\
			__FILE__,												\
			__LINE__												\
			);														\
		if(dmail_debug) 											\
		{															\
			fprintf(dmail_debug, "[%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n", 	\
				tmp->tm_year + 1900,								\
				tmp->tm_mon + 1,									\
				tmp->tm_mday,										\
				tmp->tm_hour,										\
				tmp->tm_min,										\
				tmp->tm_sec,										\
    		##__VA_ARGS__,										\
				__FILE__,											\
				__LINE__											\
				);													\
		}															\
	} while(0)

/* 红色 */
#define Erro(format, ...)						\
	do													\
	{													\
		time_t seconds; 								\
		struct tm *tmp = NULL;							\
		int size = 0;									\
		if(dmail_debug) 								\
		{												\
			size = ftell(dmail_debug);					\
			if(size > MAX_LOG_SIZE) 					\
			{											\
				rewind(dmail_debug);					\
			}											\
		}												\
		seconds = time(NULL);							\
		tmp = gmtime(&seconds); 						\
		printf("\r[\033[31mERR	\033[0m][%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n",	\
			tmp->tm_year + 1900,									\
			tmp->tm_mon + 1,										\
			tmp->tm_mday,											\
			tmp->tm_hour,											\
			tmp->tm_min,											\
			tmp->tm_sec,											\
		##__VA_ARGS__,											\
			__FILE__,												\
			__LINE__												\
			);														\
		if(dmail_debug) 											\
		{															\
			fprintf(dmail_debug, "[%d-%02d-%02d %02d:%02d:%02d.000Z] "format" [%s:%d]\r\n", 	\
				tmp->tm_year + 1900,								\
				tmp->tm_mon + 1,									\
				tmp->tm_mday,										\
				tmp->tm_hour,										\
				tmp->tm_min,										\
				tmp->tm_sec,										\
    		##__VA_ARGS__,										\
				__FILE__,											\
				__LINE__											\
				);													\
		}															\
	} while(0)





//数据类型定义
typedef unsigned char  uint8_t;
typedef char	cint8_t;
typedef unsigned short uint16_t;
typedef short	int16_t;
typedef int   int32_t;
typedef unsigned int   uint32_t;



#endif /* __JO_CHG_H__ */

