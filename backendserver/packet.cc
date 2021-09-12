#include "packet.h"
void printfNetData(NET_DATA_T data){

    
    // std::cout<<"fd为"<<data.service_data.fd<<std::endl;
    // std::cout<<"type为"<<data.service_data.type<<std::endl;
    // //std::cout<<"context为"<<data.service_data.context<<std::endl;
    // /*登录*/
    // std::cout<<"res为"<<((RES_REG_T*)data.service_data.context)->res<<std::endl;
    // std::cout<<"user_id为"<<((RES_REG_T*)data.service_data.context)->user_id<<std::endl;
    // //std::cout<<"user_name为"<<((RES_REG_T*)data.service_data.context)->user_name<<std::endl;

}

NET_DATA_T setNetData(ACCESS_DATA_T access,SERVICE_DATA_T server,int errnum,char errmsg[30]){

    NET_DATA_T net_data;
    /*设置包头*/
    net_data.head = 0x40;
    /*设置md5*/
    getDataMd5((void*)&server.context,net_data.md5,sizeof(server.context));
    /*写入内容*/
    memcpy(&net_data.access_data,&access,sizeof(ACCESS_DATA_T));
    memcpy(&net_data.service_data,&server,sizeof(SERVICE_DATA_T));
    net_data.error_code = errnum;
    memcpy(&net_data.error_msg,&errmsg,30);
    /*设置包尾*/
    net_data.tail = 0x50;

    return net_data;
}

ACCESS_DATA_T setAccessData(char packet_serial[24],int packet_total,int packet_num){

    ACCESS_DATA_T access_data;
    memcpy(access_data.packet_serial,packet_serial,24);
    access_data.packet_total = packet_total;
    access_data.packet_num = packet_num;
    return access_data;
}

SERVICE_DATA_T setServiceData(int fd,int type,char context[1024 - sizeof(int)*2]){

    SERVICE_DATA_T service_data;
    service_data.fd = fd;
    service_data.type = type;
    memcpy(service_data.context,context,1024-sizeof(int)*2);
    strcpy(service_data.context,context);
    return service_data;
}

bool isDataIntact(const NET_DATA_T& net_data)
{
    /*先判断包头包尾是否相等*/
    if(net_data.head == 0x40 && net_data.tail == 0x50)
    {
        char md5[16];
        getDataMd5((void*)&net_data.service_data.context,md5,1024-sizeof(int)*2);
        /*再判断是否通过MD5校验*/
        if(!memcmp(md5,net_data.md5,16))
        {
            return true;
        }else
        {
            std::cout<<"MD5校验失败!"<<std::endl;
            return false;
        }
    }else
    {
        std::cout<<"不完整包!"<<std::endl;
        return false;
    }
}

void getDataMd5(void* data,char* target,int data_length){

    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, data, data_length);
    MD5_Final((unsigned char*)target, &md5_ctx); 
}

void setSerial(char serial[24]){

    char tmp[24] = {'\0'};
    memcpy(serial,tmp,24);
}