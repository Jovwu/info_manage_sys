#include "shmfifo.h"
#include <assert.h>

ShmFifo PacketShm(PACKET_SHM,sizeof(NET_DATA_T),3000);
ShmFifo BusinessShm(BUSINESS_SHM,sizeof(SERVICE_DATA_T),3000);
ShmFifo ResponseShm(RESPONSE_SHM,sizeof(SERVICE_DATA_T),3000);

ShmFifo::ShmFifo(int key, int blksize,int blocks )
		:key_(key),blksize_(blksize),blocks_(blocks)	
{

		/*开辟空间*/
		fifo = (shmfifo_t *)malloc(sizeof(shmfifo_t));
		
		assert(fifo != NULL);
		/*清空空间*/
		memset(fifo, 0, sizeof(shmfifo_t));
		
		int shmid;
		shmid = shmget(key, 0, 0);
		int size = sizeof(shmhead_t) + blksize*blocks;
		/*如果未创建共享内存*/
		if (shmid == -1)
		{
			fifo->shmid = shmget(key, size, IPC_CREAT | 0666);
			if (fifo->shmid == -1)
				ERR_EXIT("shmget");
			
			fifo->p_shm = (shmhead_t*)shmat(fifo->shmid, NULL, 0);
			if (fifo->p_shm == (shmhead_t*)-1)
				ERR_EXIT("shmat");
			
			fifo->p_payload = (char*)(fifo->p_shm + 1);
			
			fifo->p_shm->blksize = blksize;
			fifo->p_shm->blocks = blocks;
			fifo->p_shm->rd_index = 0;
			fifo->p_shm->wr_index = 0;
			fifo->p_shm->put_count = 0;
			fifo->p_shm->get_count = 0;
			fifo->p_shm->current_count = 0;

			
			fifo->sem_mutex = sem_create(key);
			fifo->sem_full = sem_create(key+1);
			fifo->sem_empty = sem_create(key+2);
			fifo->sem_mutex1 = sem_create(key+3);
			
			sem_setval(fifo->sem_mutex, 1);
			sem_setval(fifo->sem_mutex1, 1);
			sem_setval(fifo->sem_full, blocks);
			sem_setval(fifo->sem_empty, 0);
		}
		else
		{
			fifo->shmid = shmid;
			fifo->p_shm = (shmhead_t*)shmat(fifo->shmid, NULL, 0);
			if (fifo->p_shm == (shmhead_t*)-1)
				ERR_EXIT("shmat");
			
			fifo->p_payload = (char*)(fifo->p_shm + 1);
			
			fifo->sem_mutex = sem_open(key);
			fifo->sem_full = sem_open(key+1);
			fifo->sem_empty = sem_open(key+2);
			fifo->sem_mutex1 = sem_open(key+3);
		}
		
	
		
}

void ShmFifo::shmfifo_put(const void *buf)
{		
		//剩余多少块能读写
		sem_p(fifo->sem_full);

			//同时只有一个线程能进行写
			sem_p(fifo->sem_mutex);
		
				memcpy(fifo->p_payload+fifo->p_shm->blksize*fifo->p_shm->wr_index, 
						buf, fifo->p_shm->blksize);
				
				fifo->p_shm->put_count++;
				fifo->p_shm->current_count++;

				fifo->p_shm->wr_index = (fifo->p_shm->wr_index + 1) % fifo->p_shm->blocks;

			sem_v(fifo->sem_mutex);

		sem_v(fifo->sem_empty);

}

void ShmFifo::shmfifo_get(void *buf)
{
		//sem_p(fifo->sem_empty);

			//读锁
			sem_p(fifo->sem_mutex1);
	
				memcpy(buf, fifo->p_payload+fifo->p_shm->blksize*fifo->p_shm->rd_index, fifo->p_shm->blksize);

				fifo->p_shm->get_count++;
				fifo->p_shm->current_count--;

				fifo->p_shm->rd_index = (fifo->p_shm->rd_index + 1) % fifo->p_shm->blocks;

			sem_v(fifo->sem_mutex1);

		sem_v(fifo->sem_full);

}

void ShmFifo::shmfifo_destroy()
{
		sem_d(fifo->sem_mutex);
		sem_d(fifo->sem_full);
		sem_d(fifo->sem_empty);
		sem_d(fifo->sem_mutex1);
		
		shmdt(fifo->p_shm);
		shmctl(fifo->shmid, IPC_RMID, 0);
		free(fifo);
}

int ShmFifo::shmfifo_isempty()
{
	if (fifo->p_shm->put_count <= 0)
			return 1;
	else
			return 2;
}

void ShmFifo::shmReadable(){

    sem_p(fifo->sem_empty);
}


