#include "myshm.h"
#include <iostream>
using namespace std;
int main(){

    shm_t t;
    cout<<"已连接数"<<'\t'<<"登陆处理"<<'\t'<<"注册处理"<<'\t'<<"视频上传"<<'\t'<<"图片上传"<<'\t'<<"视频获取"<<endl;
    while(1){

        t = g_shm.getInfo();
        cout<<'\r'<<t.connection_count<<'\t'<<'\t'<<t.login<<'\t'<<'\t'<<t.reg<<'\t'<<'\t'<<t.video<<'\t'<<'\t'<<t.photo<<'\t'<<'\t'<<t.get_video<<flush;
        sleep(1);
    }

    return 0 ;
}