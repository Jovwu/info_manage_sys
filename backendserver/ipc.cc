#include "ipc.h"

//创建信号量
int sem_create(key_t key)
{
	int semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if (semid == -1)
		ERR_EXIT("semget");

	return semid;
}

//获得已存在的信号量
int sem_open(key_t key)
{
	int semid = semget(key, 0, 0);
	if (semid == -1)
		ERR_EXIT("semget");

	return semid;
}



int sem_p(int semid)
{

	/**
	 * 
	 * 
	 * struct sembuf  
	 *	{  
  	 *	unsigned short int sem_num;   信号量的序号从0~nsems-1 
 	 *	 short int sem_op;            对信号量的操作，>0, 0, <0 
 	 *	 short int sem_flg;            操作标识：0， IPC_WAIT, SEM_UNDO 
	 *	};  
	 * 
	 * sem_num标识信号量集中的第几个信号量，0表示第1个
	 * 
	 * sem_op：如果其值为正数，
	 * 该值会加到现有的信号内含值中。
	 * 通常用于释放所控资源的使用权；
	 * 如果sem_op的值为负数，
	 * 而其绝对值又大于信号的现值，操作将会阻塞，
	 * 直到信号值大于或等于sem_op的绝对值。
	 * 通常用于获取资源的使用权；
	 * 如果sem_op的值为0，
	 * 如果没有设置IPC_NOWAIT，
	 * 则调用该操作的进程或者线程将暂时睡眠，
	 * 直到信号量的值为0；否则，进程或者线程不会睡眠，函数返回错误EAGAIN。
	 * 
	 * sem_flg：信号操作标志，可能的选择有两种
     *IPC_NOWAIT //对信号的操作不能满足时，semop()不会阻塞，并立即返回，同时设定错误信息。
	 *SEM_UNDO //程序结束时(不论正常或不正常)，保证信号值会被重设为semop()调用前的值。这样做的目的在于避免程序在异常情况下结束时未将锁定的资源解锁，造成该资源永远锁定。
	 * 
	 * 
	 * */
	struct sembuf sb = {0, -1, 0};
	int ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

int sem_v(int semid)
{
	struct sembuf sb = {0, 1, 0};
	int ret = semop(semid, &sb, 1);
	if (ret == -1)
		ERR_EXIT("semop");

	return ret;
}

//删除信号量
int sem_d(int semid)
{
	int ret = semctl(semid, 0, IPC_RMID, 0);
	/*
	if (ret == -1)
		ERR_EXIT("semctl");
	*/
	return ret;	
}


//设置信号量集中一个信号量的值
int sem_setval(int semid, int val)
{
	union semun su;
	su.val = val;
	int ret = semctl(semid, 0, SETVAL, su);
	if (ret == -1)
		ERR_EXIT("semctl");

	//printf("value updated...\n");
	return ret;
}

int sem_getval(int semid)
{
	int ret = semctl(semid, 0, GETVAL, 0);
	if (ret == -1)
		ERR_EXIT("semctl");

	//printf("current val is %d\n", ret);
	return ret;
}

int sem_getmode(int semid)
{
        union semun su;
        struct semid_ds sem;
        su.buf = &sem;
        int ret = semctl(semid, 0, IPC_STAT, su);
        if (ret == -1)
                ERR_EXIT("semctl");

        printf("current permissions is %o\n",su.buf->sem_perm.mode);
        return ret;
}

int sem_setmode(int semid,char* mode)
{
        union semun su;
        struct semid_ds sem;
        su.buf = &sem;

        int ret = semctl(semid, 0, IPC_STAT, su);
        if (ret == -1)
                ERR_EXIT("semctl");

        printf("current permissions is %o\n",su.buf->sem_perm.mode);
        sscanf(mode, "%o", (unsigned int*)&su.buf->sem_perm.mode);
        ret = semctl(semid, 0, IPC_SET, su);
        if (ret == -1)
                ERR_EXIT("semctl");

        printf("permissions updated...\n");

        return ret;
}
