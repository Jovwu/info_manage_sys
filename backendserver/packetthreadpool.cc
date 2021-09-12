#include "packetthreadpool.h"
#include "businessthreadpool.h"
PacketThreadPool::PacketThreadPool():ThreadPool(5){


    /*开启监听线程*/
    pthread_create(&this->Monitor,NULL,this->PacketShmMonitor,this);
}

PacketThreadPool::~PacketThreadPool(){


}

void PacketThreadPool::task(){

    NET_DATA_T data;
    PacketShm.shmfifo_get(&data);
    /*验证包正确性*/
    if(isDataIntact(data)){/*如果正确*/

        /*先判断流水号是否重复*/
        
        
        /*存入业务共享内存*/
        BusinessShm.shmfifo_put(&data.service_data);
        
        printfNetData(data);
    }else{
        std::cout<<"数据出错"<<std::endl;
        /*将错误写入响应共享内存*/
    }
}

void PacketThreadPool::argCheck(void* data){


}

void* PacketThreadPool::PacketShmMonitor(void* arg){

    std::cout<<"PacketShmMonitor已启动"<<std::endl;
    PacketThreadPool* pool = (PacketThreadPool*)arg;

    while(1){

        PacketShm.shmReadable();/*监听读写*/
        std::cout<<"PacketShmMonitor监听到事件！"<<std::endl;
        pool->addTask(NULL);/*添加任务*/
    }

}

