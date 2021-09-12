#include <stdio.h>
#include <string.h> 
#include <unistd.h> 
#include <netinet/in.h> 

#include <sys/socket.h>
#include <arpa/inet.h>

#include "packet.h"
#include <string>
#include <iostream>
#include <stdlib.h>


#include <pthread.h>


#define MAXLINE 80 
#define SERV_PORT 8000
using namespace std;


int main(int argc, char *argv[])
{ 
	struct sockaddr_in servaddr; 
	char buf[MAXLINE]; 
	int sockfd, n;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(8000);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	/*登陆测试*/
	NET_DATA_T rcv;
	REQ_LOGIN_T reqlogin;
	memcpy(reqlogin.soft_ver,"0",16);
	memcpy(reqlogin.user_id,"1912114250",16);
	memcpy(reqlogin.user_pwd,"dhwau",16);
	ACCESS_DATA_T ad = setAccessData("0",0,0);
	SERVICE_DATA_T st = setServiceData(0,REQ_LOGIN,(char*)&reqlogin);
	NET_DATA_T nt = setNetData(ad,st,0,NULL);
	write(sockfd, &nt, sizeof(NET_DATA_T));
	read(sockfd,&rcv,sizeof(NET_DATA_T));
	std::cout<<"登陆结果："<<((RES_LOGIN_T*)rcv.service_data.context)->res<<std::endl;
	std::cout<<"用户名为"<<((RES_LOGIN_T*)rcv.service_data.context)->user_name<<std::endl;
	std::cout<<"用户id为"<<((RES_LOGIN_T*)rcv.service_data.context)->user_id<<std::endl;
	close(sockfd);

	/*注册测试*/
	// NET_DATA_T rcv;
	// REQ_REG_T reqreg;
	// memcpy(reqreg.user_id,"191211422251",16);
	// memcpy(reqreg.user_name,"xiaozhang",16);
	// memcpy(reqreg.user_pwd,"zsyuuuuu",16);
	// ACCESS_DATA_T ad = setAccessData("0",0,0);
	// SERVICE_DATA_T st = setServiceData(0,REQ_REG,(char*)&reqreg);
	// NET_DATA_T nt = setNetData(ad,st,0,NULL);
	// write(sockfd, &nt, sizeof(NET_DATA_T));
	// read(sockfd,&rcv,sizeof(NET_DATA_T));
	// std::cout<<"注册结果："<<((RES_REG_T*)rcv.service_data.context)->res<<std::endl;
	// std::cout<<"用户id为"<<((RES_LOGIN_T*)rcv.service_data.context)->user_id<<std::endl;
	// close(sockfd);

	/*特征图片上传测试*/
	// NET_DATA_T rcv;
	// REQ_IMG_T reqreg;
	// memcpy(reqreg.user_id,"191211422251",16);
	// memcpy(reqreg.video_id,"myvideo_id",16);
	// memcpy(reqreg.typefile,"mytype_file",20);
	// memcpy(reqreg.now_time,"my_mowtime",23);
	// reqreg.type = 1;
	// ACCESS_DATA_T ad = setAccessData("0",0,0);
	// SERVICE_DATA_T st = setServiceData(0,REQ_PIC,(char*)&reqreg);
	// NET_DATA_T nt = setNetData(ad,st,0,NULL);
	// write(sockfd, &nt, sizeof(NET_DATA_T));
	// read(sockfd,&rcv,sizeof(NET_DATA_T));
	// std::cout<<"user_id:"<<((RES_IMG_T*)rcv.service_data.context)->user_id<<std::endl;
	// std::cout<<"typefile:"<<((RES_IMG_T*)rcv.service_data.context)->typefile<<std::endl;
	// std::cout<<"res:"<<((RES_IMG_T*)rcv.service_data.context)->res<<std::endl;
	// std::cout<<"update_flowid:"<<((RES_IMG_T*)rcv.service_data.context)->update_flowid<<std::endl;
	// close(sockfd);

	/*视频上传测试*/
	// NET_DATA_T rcv;
	// REQ_RECORD_UPLOAD_T reqreg;
	// memcpy(reqreg.user_id,"191211422251",16);
	// memcpy(reqreg.video_id,"myvideo_id",32);
	// memcpy(reqreg.video_seek,"mytype_file",16);
	// memcpy(reqreg.video_duration,"myvideo_durati",16);
	// memcpy(reqreg.update_flowid,"myupdate_flowid",23);
	// ACCESS_DATA_T ad = setAccessData("0",0,0);
	// SERVICE_DATA_T st = setServiceData(0,REQ_VIDEO,(char*)&reqreg);
	// NET_DATA_T nt = setNetData(ad,st,0,NULL);
	// write(sockfd, &nt, sizeof(NET_DATA_T));
	// read(sockfd,&rcv,sizeof(NET_DATA_T));
	// std::cout<<"user_id:"<<((RES_RECORD_UPLOAD_T*)rcv.service_data.context)->user_id<<std::endl;
	// std::cout<<"video_id:"<<((RES_RECORD_UPLOAD_T*)rcv.service_data.context)->video_id<<std::endl;
	// std::cout<<"res:"<<((RES_RECORD_UPLOAD_T*)rcv.service_data.context)->res<<std::endl;
	// std::cout<<"update_flowid:"<<((RES_RECORD_UPLOAD_T*)rcv.service_data.context)->update_flowid<<std::endl;
	// close(sockfd);

	/*视频记录获取测试*/
	// NET_DATA_T rcv;
	// REQ_GETVIDEO_T reqreg;
	// memcpy(reqreg.user_id,"1912251",16);
	// memcpy(reqreg.update_flowid,"update_flowid",23);
	// ACCESS_DATA_T ad = setAccessData("0",0,0);
	// SERVICE_DATA_T st = setServiceData(0,REQ_PLAY_DOWN,(char*)&reqreg);
	// NET_DATA_T nt = setNetData(ad,st,0,NULL);
	// write(sockfd, &nt, sizeof(NET_DATA_T));
	// read(sockfd,&rcv,sizeof(NET_DATA_T));
	// std::cout<<"res:"<<((RES_GETVIDEO_T*)rcv.service_data.context)->res<<std::endl;
	// std::cout<<"user_id:"<<((RES_GETVIDEO_T*)rcv.service_data.context)->user_id<<std::endl;
	// std::cout<<"video_id:"<<((RES_GETVIDEO_T*)rcv.service_data.context)->video_id<<std::endl;
	// std::cout<<"video_seek:"<<((RES_GETVIDEO_T*)rcv.service_data.context)->video_seek<<std::endl;
	// std::cout<<"video_duration:"<<((RES_GETVIDEO_T*)rcv.service_data.context)->video_duration<<std::endl;
	// std::cout<<"update_flowid:"<<((RES_GETVIDEO_T*)rcv.service_data.context)->update_flowid<<std::endl;
	// close(sockfd);



	
    return 0;
}
