#ifndef PRJ_THREADPOOL_THREADPOOL_
#define PRJ_THREADPOOL_THREADPOOL_

#include <queue>
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include "condition.h"

typedef struct task{

    struct info{

        int unused;                                     /*未使用*/
        int suicide;                                    /*线程自杀数量*/
        int total;                                      /*已处理任务总数*/

    }INFO;

    struct ctr{

        /*数据锁*/
        pthread_mutex_t unused_mutex;
        pthread_mutex_t suicide_mutex;
        pthread_mutex_t total_mutex;

        /*任务锁*/
        condition taskCond;

    }CTR;
    
}TASK_T;


typedef struct pool{

    struct info
    {
        int created;                                           /*已创建线程数*/
        pthread_t alloc_tid;                                    /*任务分配器tid*/
        pthread_t create_tid;                                    /*线程创建器tid*/
        struct timespec timer;                                 /*延时结构体*/
        int waitTask;                                          /*等待任务数量*/

    }INFO;
    
    struct ctr
    {
        condition allocCond;                            /*任务分配器条件变量*/
        condition createCond;                            /*线程创建器条件变量*/
        pthread_mutex_t createdMutex;                   /*已创建线程数锁*/
        pthread_mutex_t waitTaskMutex;                   /*等待任务数锁*/
        pthread_mutex_t pthreadVecMutex;                  /*线程向量锁*/


    }CTR;

}POOL_T;

class ThreadPool
{

public:

    ThreadPool(int thread_wait_time);
    virtual ~ThreadPool();
    bool addTask(void* data);              /*外部调用任务接口*//*模板模式*/

private:
    
    friend std::ostream &operator<<(std::ostream &output,const ThreadPool &pool)
    { 

        output <<  "未使用："<< pool.task_msg.INFO.unused
                   <<"自杀数："<<pool.task_msg.INFO.suicide
                   <<"已处理："<<pool.task_msg.INFO.total
                   <<"已创建："<<pool.pool_msg.INFO.created
                   <<std::endl;
        return output;            
    }


    /*数据*/
    POOL_T pool_msg;                               /*线程池信息*/
    TASK_T task_msg;                               /*任务信息*/
    std::vector<pthread_t> pthreadVec;             /*线程向量*/

    /*方法1*/
    static void* allocation(void* arg);             /*任务分配器*//*以线程的形式存在*/
    static void* create(void* arg);                       /*线程创建*/
    static void* rountine(void *arg);               /*持久任务*/
    virtual void task() = 0;                        /*自定义任务*/
    virtual void argCheck(void* data);     /*传入参数处理*//*默认不做处理*/

    /*方法2*/

    int getTaskInfoUnused(); /*获得未使用*/
    void addTaskInfoUnused(); /*添加未使用*/
    void subTaskInfoUnused(); /*减少未使用*/
    void addPoolInfoCreated();/*添加已创建线程数量*/
    void addTaskInfoTotal();/*增加任务总数*/
    int getTaskInfoTotal();/*获得任务总数*/
    void addPthreadVec(pthread_t tid);/*添加线程号*/
    void addTaskInfoSuicide();/*添加自杀数*/
    int getTaskInfoSuicide();/*获得自杀数*/
    int getPoolInfoCreate();/*获得创建数*/

    void addWaitTask();/*添加等待任务数*/
    void subWaitTask();/*减少等待任务数*/
    int getWaitTask();/*获取等待任务数*/

    void allocArouse();
    void allocWait();

    void createArouse();
    void createWait();

    void taskArouse();
    bool taskWait();

    void printPoolMsg();
    







    
};


#endif // PRJ_THREADPOOL_THREADPOOL_