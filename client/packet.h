#ifndef PRJ_CUSTOMEDATA_PACKET_
#define PRJ_CUSTOMEDATA_PACKET_

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <openssl/md5.h>

/*接入层(32)*/
typedef struct access_data
{
    char packet_serial[24];/*包流水*/
    int packet_total;/*包总数*/
    int packet_num;/*包序号*/
   
}ACCESS_DATA_T;

/*业务层(1024)*/
typedef struct service_data
{
    int fd;
    int type;
    char context[1024 - sizeof(int)*2];

}SERVICE_DATA_T;

/*总包*/
typedef struct net_data
{
    char head;/*包头：0x04*/
	ACCESS_DATA_T access_data;/*接入层*/
	SERVICE_DATA_T service_data;/*业务层*/
    int error_code;//错误代码，正确0
	char error_msg[30];//错误信息
	char md5[16];/*MD5校验码*/
	char tail;/*包尾：0x05*/

}NET_DATA_T;

/*****************************************具体业务*************************************/

/*功能号*/

/*请求*/
#define REQ_LOGIN 001 /*登录请求*/
#define REQ_REG 002 /*注册请求*/
#define REQ_VIDEO 003   //上传视频记录
#define REQ_PIC 004     //图片信息
#define REQ_PLAY_DOWN 005  //视频记录获取
#define REQ_VIDEO_DOWN 006 //视频记录下载请求

/*响应*/
#define RES_LOGIN 501 /*登录响应*/
#define RES_REG 502 /*注册响应*/
#define RES_VIDEO 503   //上传视频记录
#define RES_PIC 504     //图片信息
#define RES_PLAY_DOWN 505 //视频记录获取
#define REQ_VIDEO_DOWN 506 //视频记录下载响应

#define FAIL 0   //失败
#define SUCCESS 1  //成功

/*登录请求包*/
typedef struct req_login
{
	char soft_ver[16];/*软件版本号*/
	char user_id[16];/*用户id*/
	char user_pwd[16];/*用户密码*/

}REQ_LOGIN_T;

/*登录回应包*/
typedef struct res_login
{
    int res;/*登陆结果：1，成功  0，失败*/
	char user_id[16]; /*用户id（账号：用于登录）*/
	char user_name[16]; /*用户名*/
	
}RES_LOGIN_T;

/*注册请求包*/
typedef struct req_reg
{
    char user_name[16];/*用户名*/
	char user_id[16];/*用户id*/
	char user_pwd[16];/*用户密码*/

}REQ_REG_T;

/*注册响应包*/
typedef struct res_reg
{
	int res;/*注册结果：1，成功  0，失败*/
	char user_id[16]; /*用户id*/

}RES_REG_T;

/*特征图片上传请求包*/
typedef struct req_img
{
	char user_id[16];//用户ID
	char video_id[16];//视频ID
	char typefile[20];//图片名
	char now_time[23];//当前上传的本地时间
	int type;      //图片类型

}REQ_IMG_T;

/*特征图片上传响应包*/
typedef struct res_img
{
	char user_id[16];//用户ID
	char typefile[16];//图片ID
	int res;//上传结果：1，成功  0，失败
	char update_flowid[23];//上传数据流水；格式：YYYYMMDDHHMISS+fd+5位随机数

}RES_IMG_T;


/*上传视频播放记录包*/
typedef struct req_record_upload
{
	char user_id[16];//用户ID
	char video_id[32];//视频ID
	char video_seek[16];//播放时长
	char video_duration[16];//视频总时长
	char update_flowid[23];//上传数据流水；格式：YYYYMMDDHHMISS+fd+5位随机数
	
}REQ_RECORD_UPLOAD_T;

/*上传视频播放记录响应包*/
typedef struct res_record_upload
{
	int res;//上传结果：1，成功  0，失败
	char user_id[16];//用户ID
	char video_id[16];//视频ID
	char update_flowid[23];//上传数据流水；格式：YYYYMMDDHHMISS+fd+5位随机数

}RES_RECORD_UPLOAD_T;

/*获取视频播放记录请求包*/
typedef struct req_getvideo_record
{
	char user_id[16];//用户ID
	char update_flowid[23];//上传数据流水；格式：YYYYMMDDHHMISS+fd+5位随机数

}REQ_GETVIDEO_T;

/*获取视频播放记录响应包*/
typedef struct res_getvideo_record
{
	int res;
	char user_id[16];//用户ID
	char video_id[16];//视频ID
	char video_seek[16];//播放时长
	char video_duration[16];//视频总时长
	char update_flowid[23];//上传数据流水；格式：YYYYMMDDHHMISS+fd+5位随机数

}RES_GETVIDEO_T;


void printfNetData(NET_DATA_T data);/*打印网络包*/

NET_DATA_T setNetData(ACCESS_DATA_T access,SERVICE_DATA_T server,int errnum,char errmsg[30]);/*设置网络数据包*/

ACCESS_DATA_T setAccessData(char packet_serial[24],int packet_total,int packet_num);/*设置接入包*/

SERVICE_DATA_T setServiceData(int fd,int type,char context[1024 - sizeof(int)*2]);/*设置业务包*/

bool isDataIntact(const NET_DATA_T& net_data);/*判断数据是否正确*/

void getDataMd5(void* data,char* target,int data_length);//获得md5(16位)


#endif // PRJ_CUSTOMEDATA_PACKET_