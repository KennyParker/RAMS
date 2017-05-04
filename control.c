#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <string.h>

#include <my_global.h>
#include <mysql.h>

#include "control.h"
#include "database.h"
#include "spectrometer.h"
#include "queue.h"
#include "gimbal.h"
 

void* controller(void *p)
{
    struct control *c = (struct control*)p;

    //MYSQL *mysql = mysql_init(NULL);
    MYSQL_STMT    *stmt;
    MYSQL_BIND    bind[BINDS_SCAN];
    memset(bind, 0, sizeof(bind));
    
    /* Connect to database */
    if (!mysql_real_connect(mysql, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql));
        exit(0);
    }

    stmt = mysql_stmt_init(mysql);
    if (!stmt)
    {
        fprintf(stderr, "mysql_stmt_init(), out of memory\n");
        exit(0);
    }

    if(*c->CLEAR){
        destroyAll(mysql);
        printf("tables cleared!\n");
    }


    // WHITE BALANCE 
    double white_bal[PIXELS];
    if( *c->RUN_SPECTROMETER ) auto_correct(white_bal, c);


    struct arg_struct args;

    int len = encode_length;
    args.encode_len = &len;

    unsigned char encode[encode_length];
    array2string(white_bal, encode);
    args.encode = encode;

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    *c->start_time = (int) now.tv_sec; // global read by all devices

    int time = (int) now.tv_sec;
    args.v_time = &time;

    prepareTable( mysql, stmt, bind, SCAN_TYPE, &args, c );

    execute(stmt);
    *c->scan_id = mysql_insert_id(mysql);
    if (*c->DEBUG)
        printf("scans stmt for %d exectued\n", *c->scan_id);
    *c->READY = true;

    /* Close the statement */
    if (mysql_stmt_close(stmt))
    {
      fprintf(stderr, " failed while closing the statement\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
      exit(0);
    }
    else if (*c->DEBUG)
        printf("scans stmt, mysql closed\n");
    // mysql_close(mysql);

    while( getchar() != ' ' )
        /*chill*/;
    *c->STOP = true;
    
    if(*c->DEBUG) printf("control finished\n");
    
    while( ! *c->FINISHED ) // wait up
      ;
    return 0;
}


void init_control( struct control *control, int argc, char *argv[] ){

    control->exposure = (int*)malloc(sizeof(int));
    *control->exposure = 1000000; // 1s

    control->scan_id = (int*)malloc(sizeof(int));    // null
    control->start_time = (int*)malloc(sizeof(int)); // null

    control->STOP = (bool*)malloc(sizeof(bool)); 
    *control->STOP = false;

    control->DEBUG = (bool*)malloc(sizeof(bool));
    *control->DEBUG = false;

    control->OUTPUT = (bool*)malloc(sizeof(bool));
    *control->OUTPUT = false;

    control->READY = (bool*)malloc(sizeof(bool));
    *control->READY = false;

    control->FINISHED = (bool*)malloc(sizeof(bool));
    *control->FINISHED = false;

    control->CLEAR = (bool*)malloc(sizeof(bool)); // IF DESTROY IS TRUE, ALL DATA IS CLEARED on every run.
    *control->CLEAR = false;

    control->RUN_LASER = (bool*)malloc(sizeof(bool));
    *control->RUN_LASER = false;

    control->RUN_SPECTROMETER = (bool*)malloc(sizeof(bool));
    *control->RUN_SPECTROMETER = true;

    if( parse_commands( control, argc, argv ) ){
        printf("error in parsing commands\n");
        printf("Usage:\n"
        "d = DEBUG\n"
        "c = CLEAR DATABASE\n"
        "b = blind (spectrometer)\n"
        "s = safe (laser off)\n" );
        exit(1);    
    }
}

int parse_commands( struct control *control, int argc, char *argv[] )
{
    if( argc > 1 ){
        printf("Based on command line arguments...\n\n");

        for( int i=1; i<argc; i++ ){
            if( argv[i][0] == 'd' ){
                *control->DEBUG = true;
                printf("DEBUG ON \n");
            }
            else if( argv[i][0] == 'c' ){
                *control->CLEAR = true;
                printf("CLEAR DATABASE ON \n");
            }
            else if( argv[i][0] == 'b' ){
                *control->RUN_SPECTROMETER = false;
                printf("SPECTROMETER OFF \n");
            }
            else if( argv[i][0] == 'l' ){
                *control->RUN_LASER = true;
                printf("LASER ON \n");
            }
            else if( argv[i][0] == 'f' ){
                killmotors();
                exit(0);
            }
            else return 1; 
        }
    }
    return 0;
}
    



