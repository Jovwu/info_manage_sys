#ifndef PRJ_MYSQLITE3_DBCONNECTIONPOOL_
#define PRJ_MYSQLITE3_DBCONNECTIONPOOL_

#include <iostream>
#include <sqlite3.h>
#include <list>
#include <queue>
#include <string>
#include <stdio.h>

#include "condition.h"


class DBConnectionPool
{
public:
    
    static DBConnectionPool& getInstance();
    static void destroy();/*释放所有数据库连接*/
    static sqlite3* get_connection();/*获取数据库连接*/
    static void return_connection(const sqlite3* connection);/*释放数据库连接后返回数据库连接池*/
    
private:

    friend std::ostream &operator<<(std::ostream &output,const DBConnectionPool &pool )
    { 
        output << "max:" << pool.max_connection_count
               << ",current:" << pool.current_connection_count
               << ",used:" << pool.used_connection.size()
               << ",unused:" << pool.unused_connection.size()
               << ",applied:" << pool.applied_count
               << ",extra:" << pool.extra_connection_count
               << ",extra_close:" << pool.extra_connection_close_count<<std::endl;
        return output;            
    }

    DBConnectionPool();
    DBConnectionPool(const DBConnectionPool& other);
    DBConnectionPool& operator=(const DBConnectionPool& other);
    sqlite3* create_connection();/*创建数据库连接*/
    /*需添加私有的拷贝构造和移动构造*/
    ~DBConnectionPool();

    std::list<sqlite3*> used_connection;/*已使用的数据库连接*/
    std::queue<sqlite3*> unused_connection;/*未使用的数据库连接*/

    /*连接池信息*/
    const char* db_path;
    std::string test_sql;/*测试访问数据库的sql*/
    int max_wait_time;/*获取连接最大等待时间*/
    int wait_interval;/*尝试获取连接时等待时间间隔*/
    int max_connection_count;/*最大连接数*/
    int is_allot;/*用于销毁时，禁止分配数据库连接*/
    int current_connection_count;/*当前连接数*/
    int applied_count;/*已申请连接数*/
    int extra_connection_count;/*额外连接数*/
    int extra_connection_close_count;/*额外连接删除数量*/

    /*锁*/
    static condition_t ready;/*互斥量与条件变量*/

    /*单一实例指针*/
    static DBConnectionPool* instance;
    
};





#endif //PRJ_MYSQLITE3_DBCONNECTIONPOOL_