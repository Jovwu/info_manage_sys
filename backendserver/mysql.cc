#include "mysql.h"

char* SQL_CREATE_TABLE_USER = "CREATE TABLE USER("  \
                            "user_id char(16) PRIMARY KEY     NOT NULL," \
                            "user_name           char(16)    NOT NULL," \
                            "user_pwd            char(16)     NOT NULL," \
                            "user_state          int          NOT NULL);";


