#ifndef _SHM_H_
#define _SHM_H_

#include "ipc.h"
#include "condition.h"

typedef struct shm_ shm_t;

struct shm_
{
    int connection_count;       //已连接数
    int login;                  //登录请求
    int reg;                    //注册请求
    int video;                  //上传视频请求
    int photo;                  //上传图片请求
    int get_video;              //获取视频信息请求
	
    /*锁*/
	int shmid;					
	int connection_count_mutex;       //已连接数锁
    int login_mutex;                  //登录请求锁
    int reg_mutex;                    //注册请求锁
    int video_mutex;                  //上传视频请求锁
    int photo_mutex;                  //上传图片请求锁
    int get_video_mutex;              //获取视频信息请求锁
};


class MyShm
{
	public:

		MyShm(int key);
    

        shm_t getInfo();
        void changeConnectionCount(int count);
        void changeLogin(int count);
        void changeReg(int count);
        void changeVideo(int count);
        void changePhoto(int count);
        void changeGetVideo(int count);

	private:
	
        char* p;
		shm_t *myshm;
        int key;
		
};

extern MyShm g_shm;


#endif /* _SHM_FIFO_H_ */
