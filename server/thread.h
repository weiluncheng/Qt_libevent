#ifndef THREAD_H
#define THREAD_H

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_compat.h>
#include <event2/event_struct.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


//线程类
class Thread
{
public:
    Thread();

    //线程运行
    void start();

    //获取事件集合
     struct event_base * getBase();
protected:
    //线程的回调函数
    static void * worker(void * arg);

    //事件的回调函数
    static void pipeRead(evutil_socket_t, short, void *);

    //线程的逻辑处理函数
    void run();

private:
    //事件集合
    struct event_base * m_base;

    //线程ID
    pthread_t m_threadId;

    //管道的读端
    int m_pipeReadFd;
     //管道的写端
    int m_pipiWriteFd;

    //管道事件
    struct event pipeEvent;
};

#endif // THREAD_H
