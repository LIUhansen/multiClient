/*!
 *****************************************************************************
 *
 *  Copyright © 2017-2018 none. All Rights Reserved.
 *
 * \file      main.c
 * \author    HansenLiu
 * \version   1.0
 * \date      2021年9月8日
 * \brief     main函数
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

/*----------------------------------------------*
 * 包含头文件                                        *
 *----------------------------------------------*/

#include "multiClient.h"



enum
{
    CLIENT_0,
    CLIENT_1,
    CLIENT_2,
};

/*----------------------------------------------*
 * 内部函数                                     *
 *----------------------------------------------*/





static void  AgetRecv0(uint8_t *buf ,  uint8_t len);
static void  AgetRecv1(uint8_t *buf ,  uint8_t len);
static void  AgetRecv2(uint8_t *buf ,  uint8_t len);


TcpClient CliientAget[MAXCLIENT]= //客户端定义实例化
{

	/*	add your code here*/

    {
        AgetRecv0,
        0,
        "192.168.177.1",
        601,
        0,
        CLIENT_0
    },
    {
        AgetRecv1,
        0,
        "192.168.177.1",
        602,

        0,
        CLIENT_1
    },
    {
        AgetRecv2,
        0,
        "192.168.177.1",
        603,

        0,
        CLIENT_2
    },
};



/*****************************************************************************
 函 数 名  : AgetRecv0
 功能描述  : 接收回调
 输入参数  : uint8_t *buf  
             uint8_t len   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
void  AgetRecv0(uint8_t *buf ,  uint8_t len)
{
    int i;
    Debug("AgetRecv0 recv: ");

/*  add your code here*/

    if(len > 4)
    {
        for(i = 0; i < len; i++)
        {
            printf("%x ",buf[i]);
        }
    }

    printf("\n");
}

/*****************************************************************************
 函 数 名  : AgetRecv0
 功能描述  : 接收回调
 输入参数  : uint8_t *buf  
             uint8_t len   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
void  AgetRecv1(uint8_t *buf ,  uint8_t len)
{
    int i;
    Debug("AgetRecv1 recv: ");

    if(len > 4)
    {
        for(i = 0; i < len; i++)
        {
            printf("%x ",buf[i]);
        }
    }

    printf("\n");
}

/*****************************************************************************
 函 数 名  : AgetRecv0
 功能描述  : 接收回调
 输入参数  : uint8_t *buf  
             uint8_t len   
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
void  AgetRecv2(uint8_t *buf ,  uint8_t len)
{
    int i;
    Debug("AgetRecv2 recv: ");

    if(len > 4)
    {
        for(i = 0; i < len; i++)
        {
            printf("%x ",buf[i]);
        }
    }

    printf("\n");
}




/*****************************************************************************
 函 数 名  : main
 功能描述  : 主循环
 输入参数  : int argc      
             char *argv[]  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
int main(int argc, char *argv[])
{

	//init_sigaction();
	//while(1);
	uint8_t numClint = 0;
#if 1	

    tcp_client_init();
    tcp_client_create(CliientAget[numClint++]);
    tcp_client_create(CliientAget[numClint++]);
    tcp_client_create(CliientAget[numClint++]);

    while(1)
    {
        static uint8_t idx=0;
        uint8_t msg[10]= {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09 ,0x0F};
        msg[9] = 0xF0 + idx;
		
        tcp_client_Send(idx, msg, 10);
		
        idx++;

        if(idx >= numClint )//MAXCLIENT
        {
            idx = 0;
        }

        sleep(2);
    }

	
#endif


    return (0);
}








