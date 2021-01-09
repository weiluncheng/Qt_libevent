#include "thread.h"

//线程的构造函数
Thread::Thread()
{
    this->m_base = event_base_new();
    if(this->m_base == NULL)
    {
        printf("event_base_new error");
        exit(-1);
    }

    //创建管道
    int fd[2];
    if(pipe(fd) == -1)
    {
        perror("pipe");
        exit(-1);
    }
    this->m_pipeReadFd = fd[0];
    this->m_pipiWriteFd = fd[1];


    //让管道事件监听管道的读端
    //如果监听到 管道的读端有数据可读
   event_set(&this->pipeEvent,this->m_pipeReadFd,EV_SIGNAL|EV_PERSIST,pipeRead,this);

   //将事件添加到base的事件集合中
   event_base_set(this->m_base,&this->pipeEvent);

   //开辟事件的监听
   event_add(&this->pipeEvent,0);
}

//事件的回调函数
void Thread::pipeRead(evutil_socket_t, short, void *)
{

}

//线程运行
void Thread::start()
{
    //创建一个线程
    pthread_create(&(this->m_threadId),NULL,worker,this);

    //线程分离
    pthread_detach(this->m_threadId);
}


//线程的回调函数
void * Thread::worker(void * arg)
{
    Thread * p = (Thread *) arg;
    p->run();

    return NULL;
}


//线程的逻辑处理函数
void Thread::run()
{
    //监听事件集合
    //event_base_dispatch 死循环 用来除磷事件 类似于Qt中的exec()
    //如果 m_base 事件是空的，那么event_base_dispatch会立刻返回
    event_base_dispatch(this->m_base);
    event_base_free(this->m_base);

    printf("%d:done\n",this->m_threadId);
}

//获取事件集合
 struct event_base * Thread::getBase()
 {
     return this->m_base;
 }



