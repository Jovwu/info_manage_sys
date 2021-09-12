#include "dbconnectionpool.h"

condition_t DBConnectionPool::ready;
DBConnectionPool* DBConnectionPool::instance = new DBConnectionPool();

DBConnectionPool::DBConnectionPool() {


    /*连接信息暂时写死*/
    this->db_path = "/root/prj/prj/test.db";
    this->test_sql = "SELECT 1";
    this->max_wait_time  = 1000;
    this->wait_interval = 200;
    this->max_connection_count  = 300;
    this->current_connection_count = 0;
    this->applied_count = 0;
    this->extra_connection_count = 0;
    this->extra_connection_close_count = 0;
    /*初始化锁和条件变量*/
    condition_init(&ready);
    /*创建连接*/
    int i = 0;
    while(i < 10)
    {
        if(NULL != create_connection())
        {
            i++;
        }     
    }

    
}

DBConnectionPool::~DBConnectionPool() {

    /*异常情况：销毁时申请数据库连接*/
    /*销毁所有未使用数据库连接*/
    for(int i = 0;i <= this->unused_connection.size();i++) {

        sqlite3_close(this->unused_connection.front());
        this->unused_connection.pop();
    }
    /*销毁正在使用数据库*/
    for(std::list<sqlite3*>::iterator used_it = this->used_connection.begin();used_it!=this->used_connection.end();used_it++){
        sqlite3_close(*used_it);
    }
    
}

void DBConnectionPool::destroy() {


    condition_lock(&ready);

        delete instance;
        instance = NULL;
    
    condition_unlock(&ready);

}


DBConnectionPool& DBConnectionPool::getInstance() {

    return *instance;
}

sqlite3* DBConnectionPool::get_connection()
{   
    DBConnectionPool& pool = DBConnectionPool::getInstance();
    sqlite3* connection = NULL;
    condition_lock(&ready);

        pool.applied_count++;
        if(pool.unused_connection.size() > 0){/*如果有未使用的连接，返回队列中的第一个连接*/

            connection = pool.unused_connection.front();
            
        }else if(pool.current_connection_count < pool.max_connection_count){/*没有未使用的连接，但是未达到数据库最大连接数，创建新连接*/

            pool.create_connection();
            connection = pool.unused_connection.front();
            
        }

        /*如果有获得连接，将连接从使用转移到未使用*/
        if(NULL != connection)
        {
            pool.unused_connection.pop();
            pool.used_connection.push_back(connection);
            std::cout<<"获得池连接："<<connection<<std::endl;

        }else{/*连接池中的连接达到最大值，直接创建新连接，但是不由连接池管理*/
            sqlite3_open(pool.db_path,&connection);
            pool.extra_connection_count++;
            std::cout<<"获得额外连接："<<connection<<std::endl;
        }
    
    std::cout<<pool;
    condition_unlock(&ready);
    return connection;
}

sqlite3* DBConnectionPool::create_connection(){

    sqlite3* connection = NULL;
    /*如果连接已达到最大数量*/
    if(this->current_connection_count == this->max_connection_count)
    {
        return connection;
    }
    char *zErrMsg = 0;
    int rc = sqlite3_open(this->db_path, &connection);
    if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(connection));
      connection = NULL;
      //exit(0);
    }else{
      fprintf(stderr, "Opened database successfully\n");
        this->unused_connection.push(connection);
        this->current_connection_count++;
   }
    // if(sqlite3_open(this->db_path, &connection) == 0){/*连接成功打开数据库*/

    //     /*测试连接是否可用?*/
        
    //     std::cout<<"创建新的池连接："<<connection<<std::endl;
    //     this->unused_connection.push(connection);
    //     this->current_connection_count++;

    // }else{

    //     
    // }
    
    return connection;
}

void DBConnectionPool::return_connection(const sqlite3* connection){
    
    DBConnectionPool& pool = DBConnectionPool::getInstance();
    /*先判断这个连接属不属于连接池*/
    /*这样是否有问题？*/
    for(std::list<sqlite3*>::iterator user_id = pool.used_connection.begin();user_id != pool.used_connection.end();user_id++){
        
        if(*user_id == connection){

            condition_lock(&ready);

                pool.unused_connection.push(*user_id);
                pool.used_connection.erase(user_id);
                std::cout<<"回收池连接："<<connection<<std::endl;
                std::cout<<pool;

            condition_unlock(&ready);
            return;
        }
    }
   
    /*不属于连接池的直接关闭*/
    sqlite3_close(const_cast<sqlite3*>(connection));

    condition_lock(&ready);
        pool.extra_connection_close_count++;
    condition_unlock(&ready);

    std::cout<<"回收并关闭额外连接："<<connection<<std::endl;
    std::cout<<pool;
    
}