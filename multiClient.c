/******************************************************************************

                  版权所有 (C), 2018-2030, For Everyone 

 ******************************************************************************
  文 件 名   : multiClient.c
  版 本 号   : 初稿
  作    者   : HansenLiu
  生成日期   : 2021年9月21日
  最近修改   :
  功能描述   : 实现多个TCP客户端连接不同IP，还需读写分-
                   离
  函数列表   :
  修改历史   :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "multiClient.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/


/*----------------------------------------------*
 * struct                                     *
 *----------------------------------------------*/

//Tcp客户端需要的一些组成
typedef struct
{
    int sfd;                                             // socket描述符
    struct sockaddr_in addr;                             // socket地址
    const char * url;                                    // ip
    int port;                                            // 端口

} SOCKSER_CTR_STRUCT;


/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/
static int SocketClientCreat(uint8_t idx);
static uint8_t  GetClientIdx(uint32_t fd);


/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/


/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/
//用idx在本模块的各个函数里来管理各个客户端
static TcpClient gClient[MAXCLIENT];

//用来暂存一个客户端建立连接需要的内容，与gClient的idx对于
static SOCKSER_CTR_STRUCT gSocketServer[MAXCLIENT];	


static pthread_mutex_t  client_mutex[MAXCLIENT];	//互斥锁

//#if ! PTHREAD
static struct pollfd 	sPfd[MAXCLIENT];			//用来监听的套接字
static uint32_t 		sFdNum = 0;
//#endif

static int sTimes;

/*    BEGIN: Added by HansenLiu, 2021/9/21    */
// to-do-list


// Linux 定时器的功能不能用，可能是开了多线程，
//select 的使用是否多余

/*          END:   Added by HansenLiu, 2021/9/21           */

/*****************************************************************************
 函 数 名  : print_info
 功能描述  : 定时器   		  //简单打印信息，定时器触发函数
 输入参数  : int signo  
 输出参数  : 无
 返 回 值  : static
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static void print_info(int signo){

	sTimes++;


    Debug("timer fired %d = ", sTimes);
}

/*****************************************************************************
 函 数 名  : init_time
 功能描述  : 初始定时器
 输入参数  : 无
 输出参数  : 无
 返 回 值  : static
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static void init_time() 
{ 
    struct itimerval value; 
	
    value.it_value.tv_sec = 0; //定时器启动后，每隔100ms秒将执行相应的函数
    value.it_value.tv_usec = 500 * 1000; 
    value.it_interval = value.it_value; 
	
    setitimer(ITIMER_PROF, &value, NULL); //初始化 timer，到期发送 SIGPROF 信号

	Debug("timeer init success ...");
} 




/*****************************************************************************
 Prototype    : init_sigaction
 Description  : 定时器
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2021/9/22
    Author       : Hansen Liu
    Modification : Created function

*****************************************************************************/
void init_sigaction(void)
{
    struct sigaction act;
    act.sa_handler = print_info;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask); 
    sigaction(SIGPROF,&act,NULL); //设置信号 SIGPROF 的处理函数为 print_info
	
	
	init_time();

}



/*****************************************************************************
 函 数 名  : SocketSend
 功能描述  : TCP send
 输入参数  : int sfd                   
             const unsigned char *buf  
             int len                   
 输出参数  : 无
 返 回 值  : static
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static int SocketSend(int sfd, const unsigned char *buf, int len)
{
    int res;
    int connect = SOCKET_TRUE;

    //JOLogDebug("发送%d个字节 sfd = %d", res, sfd);

    if(sfd < 0)
    {
        return SOCKET_ERROR;
    }

    while(connect == SOCKET_TRUE)
    {
        // 设置为阻塞且禁止向系统发送异常消息(防止进程退出)
        res = send(sfd, buf, len, MSG_NOSIGNAL | MSG_DONTWAIT);

        // 返回-1代表有错误信息
        if( -1 == res )
        {
            Debug("send error: %s!", strerror(errno));

            switch(errno)
            {
                case ECONNRESET:
                case EBADF:
                case EPIPE:
                case ENOTSOCK:
                    connect = SOCKET_ERROR;
                    //Debug("socket 断开了");
                    break;
                    // 可能是发送缓存满了, 可以再次尝试发送
                case EAGAIN:
                    usleep(1000);
                    break;
                case EINTR:
                    connect = SOCKET_FALSE;
                    break;
                default:
                    break;
            }
        }
        else if( res > 0 )
        {
            Debug("id = %d, 发送 %d个字节 ", GetClientIdx(sfd),       res);

            // 已经发送了一部分
            if( res < len )
            {
                buf += res;
                len -= res;
            }
            // 已经发送完成了
            else if( res == len )
            {
                break;
            }
        }
    }

    return connect;
}


/*****************************************************************************
 函 数 名  : tcp_client_Send
 功能描述  : 发送数据
 输入参数  : uint8_t idx   
             uint8_t *buf  
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
void  tcp_client_Send(uint8_t idx,uint8_t *buf ,  uint8_t len)
{
    pthread_mutex_lock(&client_mutex[idx]);

    if(gClient[idx].alive == 0)
    {
        Debug("Client idx = [%d] in not alive, send  error ", idx);
		
        pthread_mutex_unlock(&client_mutex[idx]);
        return;
    }

    if(SocketSend(gClient[idx].fd, buf, len) == SOCKET_ERROR)
    {
        Debug("Client发送失败 %d \n", gClient[idx].fd);
        close(gClient[idx].fd );   // 将原先的连接关闭

        gClient[idx].fd = SOCKET_ERROR;
    }
	
    pthread_mutex_unlock(&client_mutex[idx]);
}


/*****************************************************************************
 函 数 名  : SocketClientRecvDo
 功能描述  : tcp Recv
 输入参数  : int idx  
 输出参数  : 无
 返 回 值  : static
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static void SocketClientRecvDo(int idx)
{
	int res;

	uint8_t clientRecvBuf[1024] = {0};

	if(gClient[idx].fd < 0)
	{
		Erro("sfd < 0 in SocketClientRecvDo");
	}

	res = recv(gClient[idx].fd, clientRecvBuf, sizeof(clientRecvBuf)/sizeof(clientRecvBuf[0]), MSG_NOSIGNAL | MSG_DONTWAIT);

	if( res == 0)
	{
		goto SocketClientRecvDo_ERR;
	}
	else if( res == -1)
	{
		Debug("sdf = %d, recv error: %s!", gClient[idx].fd, strerror(errno));

		switch(errno)
		{
			case ECONNRESET:
			case EBADF:
			case EPIPE:
			case ENOTSOCK:
				goto SocketClientRecvDo_ERR;;
			default:
				break;
		}

		goto SocketClientRecvDo_ERR;
	}
	else if( res > 0)
	{
		// 接收到了正确数据
		Debug("idx = %d 接收到了正确数据[%d]", idx,  res);

		//调用接收回调
		gClient[idx].Recv(clientRecvBuf, res);
		
		return;
	}

SocketClientRecvDo_ERR:

	Debug("socket断开连接");
	close(gClient[idx].fd);
	
	gClient[idx].fd = -1;
	gClient[idx].alive =0;

}

/*****************************************************************************
 函 数 名: GetClientIdx
 功能描述  : 通过套接字获得客户端索引号
 输入参数   :
 输出参数  : 无
 返 回 值: 无
 调用函数  :
 被调函数  :

 修改历史    :
  1.日    期  : 2021年01月05日
    作    者  : ZWF
    修改内容   : 新生成函数

*****************************************************************************/
static uint8_t  GetClientIdx(uint32_t fd)
{
    int i;
    for(i=0; i<MAXCLIENT; i++)
    {

        if(fd == gClient[i].fd)
        {
            return i;
        }
    }

    return 0;
}

/*****************************************************************************
 Prototype    : ReadIsReadyPoll
 Description  : poll 监听服务器的信息写入
 Input        : const int *sfdTab          
                int sfdLen                 
                int waitTimMs              
                void (*whatToDo)(int sfd)  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2021/9/22
    Author       : Hansen Liu
    Modification : Created function

*****************************************************************************/
static void  * ReadIsReadyPoll(void *p)
{
	int32_t i = 0, j = 0, idx = 0;
	int nready = 0;
	
	uint32_t checkAliveTim[MAXCLIENT] = {0};
	

	while(1)
	{
		Debug("1111000000000 nready = %d --------", nready);

			
#if 0
		for(j = 0; j < sFdNum; j++)
		{
			//每500ms判断一次是否有断开
			if(++checkAliveTim[j] >= 500)
			{
				checkAliveTim[j] = 0;

				idx = GetClientIdx(sPfd[i].fd);
				
				//断开了，重新创立连接
				if(gClient[idx].alive == 0)
				{

					SocketClientCreat(idx);	
				}

				
				Debug("1111000002222 nready = %d --------", nready);
			}

			Debug("1111003333322 nready = %d --------", nready);
		}
#endif
	
		//nready = poll(sPfd, sFdNum, -1);
		nready = poll(sPfd, sFdNum, 1000);

		Debug("0000000000000000 nready = %d --------", nready);

		if(nready == -1)
		{
			if(errno == EINTR)
			{
				Erro("poll --");
				return -1;
			}
		}

		Debug("111111111111111 nready = %d --------", nready);
		
		if(nready == 0)
		{
			continue;
		}

		//遍历已连接的套接字，看是哪个连接的服务有输入
		for(i = 0; i < sFdNum; i++)
		{
			if(sPfd[i].revents == POLLIN)
			{
				--nready;
				
				SocketClientRecvDo( GetClientIdx(sPfd[i].fd) );
				
			}
			
		}

		usleep(100 * 1000);
	}

	
}

/*****************************************************************************
 函 数 名  : ReadIsReady
 功能描述  : poll监听套接字
select 与 poll的使用场景差不多，就是建立的连接数较少，有较多的数据交互
epoll 的使用场景是 连接数较多，经常有新的连接进来，和旧的连接断开
 
 输入参数  : const int *sfdTab          
             int sfdLen                 
             int waitTimMs              
             void (*whatToDo)(int sfd)  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
uint16_t ReadIsReady(const int *sfdTab, int sfdLen, int waitTimMs, void (*whatToDo)(int sfd))
{
	struct pollfd pfd;
	
	pfd.fd = *sfdTab; 
	pfd.events = POLLIN; //关注POLLIN（读）事件

/* 有个问题：
	这里是开了新线程来处理一个socket连接
	如果将这里的pollfd 定义为static的，那么不同线程之间调用此函数，是公用pollfd ???
	可以不开启新线程，就用poll或者select来处理不同的客户端
	
*/
	int nready = 0;
	nready = poll(&pfd, 1, -1); //1：监听的文件描述符的数量；-1：无限超时等待
	//nready = poll(&pfd, 1, 0); //立即返回

	if(nready == -1)
	{
		if(errno == EINTR)
		{
			Erro("poll --");
			return -1;
		}
	}

	//返回的事件是否是输入
	if(pfd.revents == POLLIN )
	{
		Debug("nready == %d", nready);
		--nready; 	//处理一个事件，待处理事件就减1

	    if(whatToDo)
        {
            whatToDo( GetClientIdx(*sfdTab));
        }
		
	}


	
}

/*****************************************************************************
 函 数 名  : ReadIsReady
 功能描述  : 监听套接字是否有写入事件发生 select
 输入参数  : const int *sfdTab   套接字       
             int sfdLen              1   
             int waitTimMs            ms为单位  
             void (*whatToDo)(int sfd)  
 输出参数  : 无
 返 回 值  :  0-表正确
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static uint16_t ReadIsReady1(const int *sfdTab, int sfdLen, int waitTimMs, void (*whatToDo)(int sfd))
{
	int i;
    int rc;
    int maxSfd = -1;
    fd_set fds;
    struct timeval tv;

    for(i = 0, maxSfd = 0; i < sfdLen; i++)
    {
        if(sfdTab[i] > maxSfd)
        {
            maxSfd = sfdTab[i];
        }
    }

	if(maxSfd < 0)
    {
        // 没找到有效的描述符
        return 1;
    }

    // socket集合清空
    FD_ZERO(&fds);

    // 加入需要监测的socket
    for(i = 0; i < sfdLen; i++)
    {
        if(sfdTab[i] >= 0)
        {
            FD_SET(sfdTab[i], &fds);
        }
    }

    // 等待100ms
    tv.tv_sec = 0;
    tv.tv_usec = waitTimMs * 100;
    // para1: 要监测的描述符的数量，由于描述符从0开始，所以要加1
    // para2: 监测可读性
    // para3: 监测可写性
    // para4: 监测异常信号
    // para5: 超时时间
    rc = select(maxSfd + 1, &fds, NULL, NULL, &tv);


    // 有错误信息
    if (rc < 0)
    {
        Debug("select error: %s!", strerror(errno));

        switch(errno)
        {
            case ECONNRESET:
            case EBADF:
            case EPIPE:
            case ENOTSOCK:
                Debug("socket 断开了");
                break;
            case EAGAIN:
                break;
            case EINTR:
                break;
            default:
                break;
        }

        FD_ZERO(&fds);

        return 1;
    }

    // 检查sfd是否在集合内
    for(i = 0; i < sfdLen; i++)
    {
        if(sfdTab[i] < 0)
        {
            continue;
        }

        if(FD_ISSET(sfdTab[i], &fds))
        {
            // 从集合中清除
            FD_CLR(sfdTab[i], &fds);

            if(whatToDo)
            {
                whatToDo( GetClientIdx(sfdTab[i]));
            }
        }
    }

    // socket集合清空
    FD_ZERO(&fds);

	return 0;
}

/*****************************************************************************
 Prototype    : SockClientTaskPoll
 Description  : poll来管理多个服务器信息输入
 Input        : uint8_t idx  
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2021/9/22
    Author       : Hansen Liu
    Modification : Created function

*****************************************************************************/
void SockClientTaskPoll(uint8_t idx)
{
	

	Debug("idx = %d fd = %d  %d ",idx, gClient[idx].fd, gClient[idx].port);

	struct pollfd pfd;
	pfd.fd = gClient[idx].fd;
	pfd.events = POLLIN; 		//只监听输入

	//加入到全局的poll事件列表中
	sPfd[sFdNum++] = pfd;

}


/*****************************************************************************
 函 数 名  : SockClientTask
 功能描述  : 接收服务器子线程的主函数
 输入参数  : void *p  
 输出参数  : 无
 返 回 值  : static
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static void  *SockClientTask(void *p)
{
    uint32_t checkAliveTim = 0;
    uint8_t idx= *(uint8_t *)p;

	Debug("idx = %d fd = %d  %d ",idx, gClient[idx].fd, gClient[idx].port);
	
	//init_sigaction();

	while(1)
	{
		//监听套接字是否有信息输入
		ReadIsReady(&gClient[idx].fd, 1, 100, SocketClientRecvDo);

		

		//每500ms判断一次是否有断开
		if(++checkAliveTim >= 500)
		{
			checkAliveTim = 0;

			//断开了，重新创立连接
			if(gClient[idx].alive == 0)
			{

				SocketClientCreat(idx);	
			}
		}

	}
}


/*****************************************************************************
 函 数 名  : SocketClientConnect
 功能描述  : TCP 连接
 输入参数  : uint8_t idx  
 输出参数  : 0-连接正常
 返 回 值  : static
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static int SocketClientConnect(uint8_t idx)
{
	int rt = 0;
	int ret = 0;

	static int retLast = 0;

	if(gClient[idx].fd < 0)
	{
		return -1;
	}

	while(1)
	{
		//建立与服务器的连接
		ret = connect(gClient[idx].fd, (struct sockaddr *)&gSocketServer[idx].addr, sizeof(gSocketServer[idx].addr));

        unsigned long ul = 1;
        //ioctl(gClent[idx].fd, FIONBIO, &ul);  //设置为非阻塞模式

        if (ret == SOCKET_ERROR)
        {
            if(retLast != SOCKET_ERROR)
            {
                Debug("连接服务器[IP:%s, port:%d]失败, %s", inet_ntoa(gSocketServer[idx].addr.sin_addr), gSocketServer[idx].port, strerror(errno));
            }

            retLast = ret;
            rt = -1;

            //break;
        }
        else
        {
            Debug("连接服务器[IP:%s, port:%d]成功", inet_ntoa(gSocketServer[idx].addr.sin_addr), gSocketServer[idx].port);

			gClient[idx].alive = 1;

//#if !PTHREAD
				//sFdNum++;
//#endif
			
            break;
        }
    }

    retLast = ret;
	
    return rt;




}


/*****************************************************************************
 函 数 名  : SocketClientCreat
 功能描述  : TCP socket赋值
 输入参数  : uint8_t idx  
 输出参数  : 无
 返 回 值  : static  0-表正确
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
static int SocketClientCreat(uint8_t idx)
{
    // 忽略SIGPIPE信号，防止进程中止
    // 注意信号被捕获一次后，信号句柄就会还原，若想永久有效可用sigaction
    // send的地方已经配置为SIG_IGN，这个地方加不加都行
    //signal(SIGPIPE, SIG_IGN);
	uint32_t ret = 0;
	
    if(gClient[idx].fd > 0)
    {
        close(gClient[idx].fd);    // 如果已经存在，则关闭原来的socket
        sleep(1);
    }

	//建立一个非阻塞的套接字
	if ((gClient[idx].fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP)) == -1)
    {
        Debug("ERROR: Failed to obtain Socket Descriptor!\n");
        return (0);
    }


	if(gClient[idx].fd < 0)
    {
        Debug("socket error：%s!", strerror(errno));
        return SOCKET_ERROR;
    }
    else
    {
        Debug("Create socket  fd=%d  %d  %d", gClient[idx].fd,idx,gClient[idx].port);
    }


	//套接字地址，端口 赋值
	gSocketServer[idx].addr.sin_family = AF_INET;
	gSocketServer[idx].addr.sin_addr.s_addr = inet_addr(gClient[idx].url);
	gSocketServer[idx].addr.sin_port	=	htons(gClient[idx].port);

	//connect 连接服务器
	ret  = SocketClientConnect(idx);

	return ret;
	
}


/*****************************************************************************
 函 数 名  : tcp_client_create
 功能描述  : 创建TCP客户端，将外部传入的TCP结构体，赋值到本模块中进行线程创立
 输入参数  : TcpClient client  
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
int  tcp_client_create(TcpClient client)
{
	uint32_t ret = 0;
	uint8_t idx = client.idx;

	//初始化一个本地客户端结构体
	memset(&gSocketServer[idx], 0, sizeof(SOCKSER_CTR_STRUCT) );

	gClient[idx].fd = SOCKET_ERROR;
	memcpy(&gClient[idx], &client, sizeof(TcpClient));//把结构体赋值

	//初始化本客户端的互斥锁
	pthread_mutex_init(&client_mutex[idx], NULL);	

	Debug("idx = %d  %s  %d ", idx, client.url, client.port);

	//socket连接服务器
	ret = SocketClientCreat( idx);

	//开启一个子线程来来接收服务器信息
#if PTHREAD


	pthread_t pid = 0;
	
	pthread_create(&pid, NULL, SockClientTask, & idx );
	pthread_detach(pid);

#else
	//将新来的客户端套接字加入到监听队列
	SockClientTaskPoll(idx);


#endif

	return 0;
	
}






/*****************************************************************************
 函 数 名  : tcp_client_init
 功能描述  : 初始化一些变量
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 
 调用函数  : 
 被调函数  : 
 
 修改历史      :
  1.日    期   : 2021年9月21日
    作    者   : HansenLiu
    修改内容   : 新生成函数

*****************************************************************************/
void tcp_client_init()
{
	//
	memset(gClient, 0, MAXCLIENT * sizeof(TcpClient));

	//init_sigaction();

//至少需要新开一个线程，死循环专门进行监听套接字是否有输入
#if !PTHREAD
	pthread_t pid = 0;

	
	pthread_create(&pid, NULL, ReadIsReadyPoll, NULL );
	
	Debug(" Create A phread to poll success, pid = %d ----", pid);
	
	pthread_detach(pid);

#endif


}




