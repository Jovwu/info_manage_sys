#include "businessthreadpool.h"
BusinessThreadPool::BusinessThreadPool():ThreadPool(5){

    /*开启监听线程*/
    pthread_create(&this->Monitor,NULL,this->BusinessShmMonitor,this);
    /*初始化连接池*/
    
}

BusinessThreadPool::~BusinessThreadPool(){


}

void BusinessThreadPool::task(){

    SERVICE_DATA_T* data = (SERVICE_DATA_T*)malloc(sizeof(SERVICE_DATA_T));
    BusinessShm.shmfifo_get(data);
    this->doSome(data);
    delete data;  
}

void BusinessThreadPool::argCheck(void* data){


}

void* BusinessThreadPool::BusinessShmMonitor(void* arg){

    std::cout<<"BusinessShmMonitor启动！"<<std::endl;
    BusinessThreadPool* pool = (BusinessThreadPool*)arg;

    while(1){

        BusinessShm.shmReadable();/*监听读写*/
        pool->addTask(NULL);/*添加任务*/
    }

}

int BusinessThreadPool::doSome(SERVICE_DATA_T* data){

    int res = 0;
    std::cout<<"***********业务为"<<data->type<<std::endl;
    switch (data->type)
    {
    case REQ_LOGIN:/*用户登录请求*/
        res = userLogin(data);
        g_shm.changeLogin(1);
        break;
    
    case REQ_REG:/*用户注册请求*/
        res = userReg(data);
        g_shm.changeReg(1);
        break;

    case REQ_PIC:/*图片记录上传请求*/
        res = savePic(data);
        g_shm.changeVideo(1);
        break;

    case REQ_VIDEO:/*图片记录上传请求*/
        res = videoRecordUpload(data);
        g_shm.changePhoto(1);
        break;
    
    case REQ_PLAY_DOWN:/*图片记录上传请求*/
        res = getVideoRecord(data);
        g_shm.changeGetVideo(1);
        break;

    default:
        break;
    }
    return res;
}

int BusinessThreadPool::userLogin(SERVICE_DATA_T* data){

    sqlite3* sqlite = db.get_connection();
    REQ_LOGIN_T* req_login = (REQ_LOGIN_T*)(data->context);
    /*sql语句*/
    char sql[128] = {0};
    sprintf(sql,"select user_name from user where user_id = '%s' and user_pwd = '%s' limit 1;",
            req_login->user_id,req_login->user_pwd);
    /*执行*/    
    char user_name[16] = {'\0'};
    int rc = sqlite3_exec(sqlite,sql,
    [](void *data, int cNum, char *column_values[], char *column_names[]) mutable->int{

        memcpy(data,column_values[0],16);
        strcpy((char*)data,column_values[0]);  
        return 0;
    }
    ,(void*)user_name,NULL);
    /*结果*/
    if(rc == SQLITE_OK){

        /*流水号*/
        char serial[24];
        setSerial(serial);
        /*创建登录响应包*/
        RES_LOGIN_T res_login;
        res_login.res = user_name[0] == '\0'?FAIL:SUCCESS;
        memcpy(res_login.user_id,req_login->user_id,16);
        memcpy(res_login.user_name,user_name,16);

        /*创建回应包*/
        NET_DATA_T* net_data = (NET_DATA_T*)malloc(sizeof(NET_DATA_T));
        *net_data = setNetData(
            setAccessData(serial,0,0),
            setServiceData(data->fd,RES_LOGIN,(char*)&res_login),
            0,
            NULL
        );
        /*存入应答共享内存*/
        ResponseShm.shmfifo_put(net_data);
        delete net_data;

    }else{

        std::cout<<"sqlite3 error!fd:"<<data->fd<<",type:"<<data->type<<std::endl;
    }
    db.return_connection(sqlite);
}

int BusinessThreadPool::userReg(SERVICE_DATA_T* data){

    sqlite3* sqlite = db.get_connection();
    REQ_REG_T* req_reg = (REQ_REG_T*)(data->context);
    /*sql语句*/
    char sql[128] = {0};
    sprintf(sql,"insert into user values('%s','%s','%s',1);",
            req_reg->user_id,req_reg->user_name,req_reg->user_pwd);
    /*执行*/    
    int rc = sqlite3_exec(sqlite,sql,NULL,NULL,NULL);
    /*创建注册响应包*/
    RES_REG_T res_reg;
    memcpy(res_reg.user_id,req_reg->user_id,16);
    res_reg.res = rc==SQLITE_OK?SUCCESS:FAIL;
    /*流水号*/
    char serial[24];
    setSerial(serial);
    /*创建回应包*/
    NET_DATA_T* net_data = (NET_DATA_T*)malloc(sizeof(NET_DATA_T));
        *net_data = setNetData(
        setAccessData(serial,0,0),
        setServiceData(data->fd,RES_REG,(char*)&res_reg),
        0,
        NULL
        );
    /*存入应答共享内存*/
    ResponseShm.shmfifo_put(net_data);
    delete net_data;
    db.return_connection(sqlite);
    return 0;
}

int BusinessThreadPool::savePic(SERVICE_DATA_T* data){

    sqlite3* sqlite = db.get_connection();
    REQ_IMG_T* req_img = (REQ_IMG_T*)(data->context);
    /*sql语句*/
    char sql[128] = {0};
    sprintf(sql,"insert into photo values('%s','%s','%s','%s',%d);",
            req_img->typefile,req_img->video_id,req_img->user_id,req_img->now_time,req_img->type);
    /*执行*/    
     std::cout<<"****************pthoto_id="<< req_img->typefile<<std::endl;
    int rc = sqlite3_exec(sqlite,sql,NULL,NULL,NULL);
    /*创建图片响应包*/
    RES_IMG_T res_img;
    char typefile[16] = {'\0'};
    char update_flowid[32] = {'\0'};
    memcpy(res_img.user_id,req_img->user_id,16);
    memcpy(res_img.typefile,typefile,16);
    memcpy(res_img.update_flowid,update_flowid,23);
    res_img.res = rc==SQLITE_OK?SUCCESS:FAIL;
    std::cout<<"****************res="<<res_img.res<<std::endl;
    /*流水号*/
    char serial[24];
    setSerial(serial);
    /*创建回应包*/
    NET_DATA_T* net_data = (NET_DATA_T*)malloc(sizeof(NET_DATA_T));
        *net_data = setNetData(
        setAccessData(serial,0,0),
        setServiceData(data->fd,RES_PIC,(char*)&res_img),
        0,
        NULL
        );
    /*存入应答共享内存*/
    ResponseShm.shmfifo_put(net_data);
    delete net_data;
    db.return_connection(sqlite);
   // std::cout<<"BusinessThreadPool::savePic"<<std::cout;
    return 0;
}

int BusinessThreadPool::videoRecordUpload(SERVICE_DATA_T* data){

    sqlite3* sqlite = db.get_connection();
    REQ_RECORD_UPLOAD_T* req_video = (REQ_RECORD_UPLOAD_T*)(data->context);
    /*sql语句*/
    char sql[128] = {0};
    sprintf(sql,"insert into video values('%s','%s','%s','%s','%s');",
            req_video->user_id,req_video->video_id,req_video->video_seek,req_video->video_duration,req_video->update_flowid);
    /*执行*/    
    int rc = sqlite3_exec(sqlite,sql,NULL,NULL,NULL);
    /*创建视频响应包*/
    RES_RECORD_UPLOAD_T res_video;
    res_video.res = rc == SQLITE_OK?SUCCESS:FAIL;
    memcpy(res_video.user_id,req_video->user_id,16);
    memcpy(res_video.video_id,req_video->video_id,16);
    memcpy(res_video.update_flowid,req_video->update_flowid,23);
    /*流水号*/
    char serial[24];
    setSerial(serial);
    /*创建回应包*/
    NET_DATA_T* net_data = (NET_DATA_T*)malloc(sizeof(NET_DATA_T));
        *net_data = setNetData(
        setAccessData(serial,0,0),
        setServiceData(data->fd,RES_VIDEO,(char*)&res_video),
        0,
        NULL
        );
    /*存入应答共享内存*/
    ResponseShm.shmfifo_put(net_data);
    delete net_data;
    db.return_connection(sqlite);
    return 0;
}

int BusinessThreadPool::getVideoRecord(SERVICE_DATA_T* data){

    sqlite3* sqlite = db.get_connection();
    REQ_GETVIDEO_T* req_getvideo = (REQ_GETVIDEO_T*)(data->context);
    /*sql语句*/
    char sql[128] = {0};
    sprintf(sql,"select * from video where user_id = '%s' order by 5 limit 1;",
            req_getvideo->user_id);
    /*执行*/    
    RES_GETVIDEO_T videoinfo;
    videoinfo.res = FAIL;
    memcpy(videoinfo.user_id,"NULL",16);
    memcpy(videoinfo.video_id,"NULL",16);
    memcpy(videoinfo.video_seek,"NULL",16);
    memcpy(videoinfo.video_duration,"NULL",16);
    memcpy(videoinfo.update_flowid,"NULL",23);
    int rc = sqlite3_exec(sqlite,sql,
    [](void *data, int cNum, char *column_values[], char *column_names[]) mutable->int{

        RES_GETVIDEO_T* video = (RES_GETVIDEO_T*)data;
        video->res = SUCCESS;
        memcpy(video->user_id,column_values[0],16);
        memcpy(video->video_id,column_values[1],16);
        memcpy(video->video_seek,column_values[2],16);
        memcpy(video->video_duration,column_values[3],16);
        memcpy(video->update_flowid,column_values[4],23);
        return 0;
    }
    ,(void*)&videoinfo,NULL);
    
    /*流水号*/
    char serial[24];
    setSerial(serial);
    /*创建回应包*/
    NET_DATA_T* net_data = (NET_DATA_T*)malloc(sizeof(NET_DATA_T));
        *net_data = setNetData(
        setAccessData(serial,0,0),
        setServiceData(data->fd,RES_PLAY_DOWN,(char*)&videoinfo),
        0,
        NULL
        );
    /*存入应答共享内存*/
    ResponseShm.shmfifo_put(net_data);
    delete net_data;
    db.return_connection(sqlite);
    return 0;
}