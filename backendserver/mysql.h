#ifndef _MYSQL_
#define _MYSQL_

/*表*/
extern char* SQL_CREATE_TABLE_USER;/*用户表*/
extern char* SQL_CREATE_TABLE_PHOTO;/*图片表*/
extern char* SQL_CREATE_TABLE_VIDEO;/*视频表*/
extern char* SQL_CREATE_TABLE_SYSTEM;/*系统信息表*/
extern char* SQL_CREATE_TABLE_SERIAL;/*流水号表*/

/*业务*/
extern char* SQL_USER_LOGIN;/*用户登录*/
extern char* SQL_USER_REG;/*用户注册*/

extern char* SQL_PHOTO_SAVE;/*图片保存*/
extern char* SQL_PHOTO_RETURN;/*图片信息返回*/

extern char* SQL_VIDEO_SAVE;/*视频保存*/
extern char* SQL_VIDEO_HISTORY;/*视频近期历史记录*/
extern char* SQL_VIDEO_END_RECORD;/*视频最后一次观看记录*/

extern char* SQL_SERIAL_SAVE;/*流水号保存*/
extern char* SQL_SERIAL_SELECT;/*流水号查询*/






#endif // _MYSQL_