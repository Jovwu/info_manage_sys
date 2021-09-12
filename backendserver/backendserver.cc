#include "packetthreadpool.h"
#include "businessthreadpool.h"


int select_cb(void *data, int cNum, char *column_values[], char *column_names[]) //回调函数,处理sql语句执行结果
{
	//int *p = (int *)data;
	int i;
  // std::cout<<"结果有"<<cNum<<"列"<<std::endl;
	// if ( *p )
	// {
	// 	for (i = 0; i < cNum; i++)
	// 	{
	// 		printf("%s\t", column_names[i]);
	// 	}
	// 	printf("\n");
	// 	*p = 0;
	// }
	for (i = 0; i < cNum; i++)
	{
		
      
      strcpy((char*)data,column_values[i]);
      printf("%s\t",(char*)data);
	}
	printf("\n");
   //(*p)++;
	return 0;
}

int main(){

    PacketThreadPool * pool = new PacketThreadPool();
    BusinessThreadPool * pool2 = new BusinessThreadPool();

    char *zErrMsg1 = 0;
    int rc1;
    /*创建表*/
    sqlite3* sqlite = DBConnectionPool::get_connection();
    /*用户表*/
    char *sql1 = 
    "CREATE TABLE IF NOT EXISTS user ("\
    "user_id char(16) NOT NULL,"\
    "user_name char(16),"\
    "user_pwd char(16),"\
    "user_state integer,"\
     "PRIMARY KEY (user_id));";
    sqlite3_exec(sqlite,sql1,NULL,NULL,NULL);

    
    /*视频表*/
    char* sql3 = 
    "CREATE TABLE IF NOT EXISTS video ("\
    "user_id char(16) NOT NULL,"\
    "video_id char(16) NOT NULL,"\
    "video_seek char(16),"\
    "video_duration char(16),"\
    "update_flowid char(23),"\
    "PRIMARY KEY (user_id, video_id),"\
    "CONSTRAINT VDEO2 FOREIGN KEY (user_id) REFERENCES user (user_id) ON DELETE CASCADE ON UPDATE CASCADE);";
    sqlite3_exec(sqlite,sql3,NULL,NULL,NULL);

      /*图片表*/
    char *sql2 =
    "CREATE TABLE IF NOT EXISTS photo ("\
    "photo_id char(20) NOT NULL,"\
    "video_id char(16) NOT NULL,"\
    "user_id char(16) NOT NULL,"\
    "now_time char(23),"\
    "type  integer,"\
    "PRIMARY KEY (photo_id),"\
    "CONSTRAINT PIC2d FOREIGN KEY (user_id) REFERENCES user (user_id) ON DELETE CASCADE ON UPDATE CASCADE);";
    rc1=sqlite3_exec(sqlite,sql2,NULL,NULL,NULL);
    if( rc1 != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg1);
      sqlite3_free(zErrMsg1);
   }else{
      fprintf(stdout, "Records created successfully\n");
   }


    /*流水表*/
    char* sql4 = 
    "CREATE TABLE serialinfo ("\
    "fd integer,"\
    "serial char(24) NOT NULL);";
    sqlite3_exec(sqlite,sql4,NULL,NULL,NULL);
    DBConnectionPool::return_connection(sqlite);


   








//     int rc = 0;
//     char *zErrMsg = NULL;
//     //sqlite3* sqlite1 = DBConnectionPool::get_connection();
//   sqlite3 *db = DBConnectionPool::get_connection();
//    // rc = sqlite3_open("/home/zhongshiyu/chuanyi/prj/test.db", &db);
//    if( rc ){
//       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//       exit(0);
//    }else{
//       fprintf(stderr, "Opened database successfully******************\n");
//    }


//    char sql[128]={0};
//    int first = 1;
   /*插入测试*/
   //  char user_id[16] = "1912114250";
   //  char user_name[16] = "zhognshiyu";
   //  char user_pwd[16] = "dhwau";
   //  int user_state = 1;
   //  const char* data = "Callback function called";
    
   //  sprintf(sql,"INSERT INTO user VALUES('%s','%s','%s',%d) ;", user_id, user_name, user_pwd, user_state);
   //  rc = sqlite3_exec(db,sql,NULL,NULL,NULL);
   

   /*查询测试*/
   // char user_name[16] = {0};
   // sprintf(sql,"select user_name from user", NULL);
   
  //rc = sqlite3_exec(db,sql,select_cb,(void*)user_name,NULL);
//   rc = sqlite3_exec(db,sql,
//   [](void *data, int cNum, char *column_values[], char *column_names[]) mutable->int{

//    for (int i = 0; i < cNum; i++){

//      strcpy((char*)data,column_values[i]);
// 	}
//    return 0;
//   }
//   ,(void*)user_name,NULL);
//    if (rc != SQLITE_OK)
// 	{
// 		perror("sqlite3_prepare_v2 error:");
//       std::cout<<rc<<std::endl;
// 		return -1;
// 	}
//    std::cout<<"rc="<<rc<<std::endl;
//    std::cout<<"name="<<user_name<<std::endl;



   // char res1[125] = {'\0'};
   //  std::cout<<res1[0]<<std::endl;
   // if(res1[0] == '\0'){

   //    std::cout<<"无结果"<<std::endl;
   // }

   // strcpy(res1,"有结果");
   // std::cout<<res1<<std::endl;



    while (1){}
    return 0;
}