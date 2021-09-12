#include "myshm.h"
#include <assert.h>

MyShm g_shm(9999);

MyShm::MyShm(int key):key(key)
{

		/*开辟空间*/
		myshm = (shm_t *)malloc(sizeof(shm_t));
		
		assert(myshm != NULL);
		/*清空空间*/
		memset(myshm, 0, sizeof(shm_t));
		
		int shmid;
		shmid = shmget(key, 0, 0);
		int size = sizeof(shm_t);
		/*如果未创建共享内存*/
		if (shmid == -1)
		{
			myshm->shmid = shmget(key, size, IPC_CREAT | 0666);
			if (myshm->shmid == -1)
				ERR_EXIT("shmget");
			
			myshm = (shm_t*)shmat(myshm->shmid, NULL, 0);
			if (myshm == (shm_t*)-1)
				ERR_EXIT("shmat");
			
			myshm->connection_count = 0;
            myshm->login = 0;
            myshm->reg = 0;
            myshm -> video = 0;
            myshm->photo = 0;
            myshm->get_video = 0;

            myshm->connection_count_mutex = sem_create(key + 1);
            myshm->login_mutex = sem_create(key+2);
            myshm->reg_mutex = sem_create(key+3);
            myshm->video_mutex = sem_create(key+4);
            myshm->photo_mutex = sem_create(key+5);
            myshm->get_video_mutex = sem_create(key+6);

			sem_setval(myshm->connection_count_mutex, 1);
            sem_setval(myshm->login_mutex, 1);
            sem_setval(myshm->reg_mutex, 1);
            sem_setval(myshm->video_mutex, 1);
            sem_setval(myshm->photo_mutex, 1);
            sem_setval(myshm->get_video_mutex, 1);
		}
		else
		{
			myshm->shmid = shmid;
			myshm = (shm_t*)shmat(myshm->shmid, NULL, 0);
			if (myshm == (shm_t*)-1)
				ERR_EXIT("shmat");
			
			myshm->connection_count_mutex = sem_open(key + 1);
            myshm->login_mutex = sem_open(key+2);
            myshm->reg_mutex = sem_open(key+3);
            myshm->video_mutex = sem_open(key+4);
            myshm->photo_mutex = sem_open(key+5);
            myshm->get_video_mutex = sem_open(key+6);
		}
		
	
		
}

shm_t MyShm::getInfo(){

    shm_t t;
    memcpy(&t,this->myshm,sizeof(shm_t));
    return t;
}

void MyShm::changeConnectionCount(int count){

    sem_p(myshm->connection_count_mutex);
    myshm->connection_count += count;
    sem_v(myshm->connection_count_mutex);

}
void MyShm::changeLogin(int count){

    sem_p(myshm->login_mutex);
    myshm->login += count;
    sem_v(myshm->login_mutex);
}
void MyShm::changeReg(int count){
    sem_p(myshm->reg_mutex);
    myshm->reg += count;
    sem_v(myshm->reg_mutex);
}
void MyShm::changeVideo(int count){
    sem_p(myshm->video_mutex);
    myshm->video += count;
    sem_v(myshm->video_mutex);
}
void MyShm::changePhoto(int count){
    sem_p(myshm->photo_mutex);
    myshm->photo += count;
    sem_v(myshm->photo_mutex);
}
void MyShm::changeGetVideo(int count){
    sem_p(myshm->get_video_mutex);
    myshm->get_video += count;
    sem_v(myshm->get_video_mutex);
}

