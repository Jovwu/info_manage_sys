#ifndef PRJ_THREADPOOL_BUSINESSTHREADPOOL_
#define PRJ_THREADPOOL_BUSINESSTHREADPOOL_
#include "threadpool.h"
#include "shmfifo.h"
#include <stdio.h>
#include "dbconnectionpool.h"
#include "businessthreadpool.h"
#include "myshm.h"

class BusinessThreadPool:public ThreadPool
{

public:

    BusinessThreadPool(/* args */);
    ~BusinessThreadPool();

private:

    static void* BusinessShmMonitor(void* arg);/*共享内存监听线程*/
    pthread_t Monitor;

    DBConnectionPool& db = DBConnectionPool::getInstance();
    
    void task();
    void argCheck(void* data);

    /*业务*/
    int doSome(SERVICE_DATA_T* data);/*业务分配*/
    int userLogin(SERVICE_DATA_T* data);/*处理用户登录*/
    int userReg(SERVICE_DATA_T* data);/*处理用户注册*/
    int savePic(SERVICE_DATA_T* data);/*特征图片上传*/
    int videoRecordUpload(SERVICE_DATA_T* data);/*视频播放记录上传*/
    int getVideoRecord(SERVICE_DATA_T* data);/*获取视频记录*/



};







#endif //PRJ_THREADPOOL_BUSINESSTHREADPOOL_