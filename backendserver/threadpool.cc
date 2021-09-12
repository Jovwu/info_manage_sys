#include "threadpool.h"
ThreadPool::ThreadPool(int thread_wait_time){

       

        /*初始化pool信息*/
        this->pool_msg.INFO.created = 0;
        this->pool_msg.INFO.timer.tv_sec = time(NULL) + (thread_wait_time>0?thread_wait_time:0);
        this->pool_msg.INFO.timer.tv_nsec = 0;
        this->pool_msg.INFO.waitTask = 0;
        
        /*初始化锁和条件变量*/
        condition_init(&this->pool_msg.CTR.allocCond);
        condition_init(&this->pool_msg.CTR.createCond);
        pthread_mutex_init(&this->pool_msg.CTR.createdMutex, NULL);
        pthread_mutex_init(&this->pool_msg.CTR.waitTaskMutex, NULL);
        pthread_mutex_init(&this->pool_msg.CTR.pthreadVecMutex, NULL);
        /*初始化任务信息*/
       
        TASK_T task_info;
        /*初始化数据*/
        task_info.INFO.unused = 0;
        task_info.INFO.suicide = 0;
        task_info.INFO.total = 0;
        /*初始化锁和条件变量*/
        pthread_mutex_init(&task_info.CTR.unused_mutex, NULL);
        pthread_mutex_init(&task_info.CTR.suicide_mutex, NULL);
        pthread_mutex_init(&task_info.CTR.total_mutex, NULL);
        condition_init(&task_info.CTR.taskCond);

        /*开启任务分配器线程*/
        pthread_create(&this->pool_msg.INFO.alloc_tid,NULL,this->allocation,this);   
        /*开启线程创建器*/
        pthread_create(&this->pool_msg.INFO.create_tid,NULL,this->create,this);     
}

ThreadPool::~ThreadPool(){


    /*先释放任务分配器,可避免分配任务*/
    pthread_cancel(this->pool_msg.INFO.alloc_tid);
    pthread_cancel(this->pool_msg.INFO.create_tid);
    /*释放所有任务*/
    for(std::vector<pthread_t>::iterator i = this->pthreadVec.begin();i != this->pthreadVec.end();i++){
        pthread_cancel(*i);
    }
}

bool ThreadPool::addTask(void* data){

   
    /*先处理数据，避免任务添加但是没有数据处理*/
    this->argCheck(data);
    
    /*添加待完成任务*/
    this->addWaitTask();
   
    /*唤醒任务处理器*/
    this->allocArouse();
    

    return true;
}

void* ThreadPool::allocation(void* arg){

    std::cout<<"allocation启动！"<<std::endl;
    ThreadPool* pool = (ThreadPool*)arg;

    while(1){
        
        
        while(pool->getWaitTask() == 0){/*如果待完成为0*/
            
            pool->allocWait();/*等待*/
        }

        //std::cout<<*pool;
        pool->printPoolMsg();
        
        /*判断是否有等待线程*/
        if(pool->getTaskInfoUnused() > 0){/*如果有*/

            pool->taskArouse();/*唤醒等待线程*/

        }else{/*如果没有，唤醒线程创建*/

            pthread_t tid;
            if(pthread_create(&tid,NULL,pool->rountine,pool) != 0)
	        {
		    std::cout<<"线程无法启动!"<<std::endl;

	        }
            //pool->createArouse();/*唤醒等待线程*/
        }

        pool->subWaitTask();/*减去待完成*/
     
    }   

}

void* ThreadPool::rountine(void *arg)
{	
	ThreadPool *pool = (ThreadPool*)arg;
    pool->addPthreadVec(pthread_self());/*添加线程号*/
    pool->addPoolInfoCreated();/*添加线程总数*/
    do{
        pool->task();/*完成任务*/
        pool->addTaskInfoTotal();/*添加任务总数*/
        
            if(pool->taskWait() == false){/*如果超时*/

                pool->addTaskInfoSuicide();/*添加自杀数*/
                pthread_exit(NULL);
            }
        }while(1);

    }
    

void ThreadPool::argCheck(void* data){


}

void* ThreadPool::create(void* arg){

    ThreadPool* pool = (ThreadPool*)arg;
 
    pthread_t tid;
    while (1)
    {

        pool->createWait();/*阻塞等待*/

        if(pthread_create(&tid,NULL,pool->rountine,pool) != 0)
	    {
		    std::cout<<"线程无法启动!"<<std::endl;

	    }
    }
    
}


int ThreadPool::getTaskInfoUnused(){
    int res;
    pthread_mutex_lock(&this->task_msg.CTR.unused_mutex);
    res = this->task_msg.INFO.unused;
    pthread_mutex_unlock(&this->task_msg.CTR.unused_mutex);
    return res;
}

void ThreadPool::addTaskInfoUnused(){
    pthread_mutex_lock(&this->task_msg.CTR.unused_mutex);
    this->task_msg.INFO.unused++;
    pthread_mutex_unlock(&this->task_msg.CTR.unused_mutex);
}

void ThreadPool::addPoolInfoCreated(){

    pthread_mutex_lock(&this->pool_msg.CTR.createdMutex);
    this->pool_msg.INFO.created ++;
    pthread_mutex_unlock(&this->pool_msg.CTR.createdMutex);
}

void ThreadPool::allocArouse(){

    condition_lock(&this->pool_msg.CTR.allocCond);
    condition_signal(&this->pool_msg.CTR.allocCond);
    condition_unlock(&this->pool_msg.CTR.allocCond);
}

void ThreadPool::allocWait(){

    condition_lock(&this->pool_msg.CTR.allocCond);
    condition_wait(&this->pool_msg.CTR.allocCond);
    condition_unlock(&this->pool_msg.CTR.allocCond);
}

int ThreadPool::getWaitTask(){

    int res;
    pthread_mutex_lock(&this->pool_msg.CTR.waitTaskMutex);
    res = this->pool_msg.INFO.waitTask;
    pthread_mutex_unlock(&this->pool_msg.CTR.waitTaskMutex);
    return res;
}

void ThreadPool::addWaitTask(){

    pthread_mutex_lock(&this->pool_msg.CTR.waitTaskMutex);
    this->pool_msg.INFO.waitTask++;
    pthread_mutex_unlock(&this->pool_msg.CTR.waitTaskMutex);
}

void ThreadPool::subWaitTask(){
    pthread_mutex_lock(&this->pool_msg.CTR.waitTaskMutex);
    this->pool_msg.INFO.waitTask--;
    pthread_mutex_unlock(&this->pool_msg.CTR.waitTaskMutex);
}


bool ThreadPool::taskWait(){

    bool res = true;
    condition_lock(&this->task_msg.CTR.taskCond);
    
    this->addTaskInfoUnused();
    do{

        if(condition_timedwait(&this->task_msg.CTR.taskCond,&this->pool_msg.INFO.timer) == ETIMEDOUT){ /*如果超时*/

            res = false;
            break;
        }

    }while (this->getWaitTask() == 0);
    this->subTaskInfoUnused();
    condition_unlock(&this->task_msg.CTR.taskCond);
    return res;
}   

void ThreadPool::taskArouse(){

    condition_lock(&this->task_msg.CTR.taskCond);
    condition_broadcast(&this->task_msg.CTR.taskCond);
    condition_unlock(&this->task_msg.CTR.taskCond);
}

void ThreadPool::createWait(){

    condition_lock(&this->pool_msg.CTR.createCond);
    condition_wait(&this->pool_msg.CTR.createCond);
    condition_unlock(&this->pool_msg.CTR.createCond);

}

void ThreadPool::createArouse(){

    condition_lock(&this->pool_msg.CTR.createCond);
    condition_signal(&this->pool_msg.CTR.createCond);
    condition_unlock(&this->pool_msg.CTR.createCond);
}

int ThreadPool::getTaskInfoTotal(){

    int res;
    pthread_mutex_lock(&this->task_msg.CTR.total_mutex);
    res = this->task_msg.INFO.total;
    pthread_mutex_unlock(&this->task_msg.CTR.total_mutex);
    return res;
}

void ThreadPool::addTaskInfoTotal(){

    pthread_mutex_lock(&this->task_msg.CTR.total_mutex);
    this->task_msg.INFO.total++;
    pthread_mutex_unlock(&this->task_msg.CTR.total_mutex);
}

void ThreadPool::addPthreadVec(pthread_t tid){

    pthread_mutex_lock(&this->pool_msg.CTR.pthreadVecMutex);
    this->pthreadVec.push_back(tid);
    pthread_mutex_unlock(&this->pool_msg.CTR.pthreadVecMutex);
}


int ThreadPool::getTaskInfoSuicide(){

    int res;
    pthread_mutex_lock(&this->task_msg.CTR.suicide_mutex);
    res = this->task_msg.INFO.suicide;
    pthread_mutex_unlock(&this->task_msg.CTR.suicide_mutex);
    return res;
}   

void ThreadPool::addTaskInfoSuicide(){

    pthread_mutex_lock(&this->task_msg.CTR.suicide_mutex);
    this->task_msg.INFO.suicide++;
    pthread_mutex_unlock(&this->task_msg.CTR.suicide_mutex);
}  

void ThreadPool::subTaskInfoUnused(){

    pthread_mutex_lock(&this->task_msg.CTR.unused_mutex);
    this->task_msg.INFO.unused--;
    pthread_mutex_unlock(&this->task_msg.CTR.unused_mutex);
}

int ThreadPool::getPoolInfoCreate(){

    int res;
    pthread_mutex_lock(&this->pool_msg.CTR.createdMutex);
    res = this->pool_msg.INFO.created;
    pthread_mutex_unlock(&this->pool_msg.CTR.createdMutex);
    return res;
}


void ThreadPool::printPoolMsg(){

    std::cout<<"待完成："<<this->getWaitTask()
            <<"，未使用："<<this->getTaskInfoUnused()
            <<"，自杀数："<<this->getTaskInfoSuicide()
            <<"，已处理"<<this->getTaskInfoTotal()
            <<"，已创建"<<this->getPoolInfoCreate()
            <<std::endl;
}
















