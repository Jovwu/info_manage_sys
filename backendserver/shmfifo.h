#ifndef _SHM_FIFO_H_
#define _SHM_FIFO_H_

#include "ipc.h"
#include "condition.h"
#include "packet.h"

#define PACKET_SHM 1111
#define BUSINESS_SHM 2222
#define RESPONSE_SHM 3333

typedef struct shmfifo shmfifo_t;
typedef struct shmhead shmhead_t;

struct shmhead
{
	unsigned int blksize;		// ���С
	unsigned int blocks;		// �ܿ���
	unsigned int rd_index;		// ������
	unsigned int wr_index;		// д����
	int current_count;          //当前数量
	int put_count;				//放入总数
	int get_count;              //取出总数

};
struct shmfifo
{
	shmhead_t *p_shm;			// �����ڴ�ͷ��ָ��
	char *p_payload;			// ��Ч���ص���ʼ��ַ
	
	int shmid;					// �����ڴ�ID
	int sem_mutex;				// ���������õ��ź���
	int sem_mutex1;				// ���������õ��ź���
	int sem_full;				// �������ƹ����ڴ��Ƿ������ź���
	int sem_empty;				// �������ƹ����ڴ��Ƿ�յ��ź���
};


class ShmFifo
{
	public:
		ShmFifo(int key, int blksize,int blocks );
       
        void shmReadable();/*共享内存可读*/

		void shmfifo_put(const void *buf);

		void shmfifo_get(void *buf);

		void shmfifo_destroy();
		int shmfifo_isempty();

	    
	

	private:
		int key_;
		int blksize_;
		int blocks_;
		shmfifo_t *fifo;
		

};

extern ShmFifo PacketShm;
extern ShmFifo BusinessShm;
extern ShmFifo ResponseShm;

#endif /* _SHM_FIFO_H_ */
