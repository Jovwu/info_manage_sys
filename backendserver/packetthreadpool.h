#ifndef PACKETTHREADPOOL_
#define PACKETTHREADPOOL_
#include "threadpool.h"
#include "packet.h"
#include "shmfifo.h"

class PacketThreadPool:public ThreadPool
{

public:
    PacketThreadPool(/* args */);
    ~PacketThreadPool();

private:
    
    void task();
    void argCheck(void* data);

    pthread_t Monitor;
    static void* PacketShmMonitor(void* arg);/*共享内存监听线程*/

};




#endif PACKETTHREADPOOL_