#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdatomic.h>
#include <my_global.h>
#include <mysql.h>

#include "control.h"
#include "spectrometer.h"
#include "database.h"
#include "queue.h"

void execute( MYSQL_STMT *stmt )
{

    my_ulonglong affected_rows;

    /* Execute the INSERT statement */
    if (mysql_stmt_execute(stmt))
    {
      fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
      exit(0);
    }

    /* Get the number of affected rows */
    affected_rows= mysql_stmt_affected_rows(stmt);

    if (affected_rows != 1) /* validate affected rows */
    {
      fprintf(stderr, " invalid affected rows by MySQL\n");
      exit(0);
    }
}

void prepareTable( MYSQL *mysql, MYSQL_STMT *stmt, MYSQL_BIND *bind, enum data_type type, struct arg_struct *args, struct control *c)
{

    char* insert;
    int binds;
    int param_count;

    // SET UP DATABASE
    if(*c->DEBUG)
        printf("MySQL client version: %s\n", mysql_get_client_info());

    switch( type ){
        case(SCAN_TYPE):
            insert = INSERT_SCAN;
            binds = BINDS_SCAN;
        break;
        case(SPECTRUM_TYPE): 
            insert = INSERT_SPECTRA;
            binds = BINDS_SPECTRA;
        break;
        case(VOXEL_TYPE): 
            insert = INSERT_VOXEL;
            binds = BINDS_VOXEL;
        break;
    }

    if(*c->DEBUG) printf("inserting %s\n", insert);

    if (mysql_stmt_prepare(stmt, insert, strlen(insert)))
    {
      fprintf(stderr, " mysql_stmt_prepare(), insert failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
      exit(0);
    }

    if(*c->DEBUG)
        fprintf(stdout, " prepare, insert successful\n");

    /* Get the parameter count from the statement */
    param_count= mysql_stmt_param_count(stmt);
    
    if(*c->DEBUG)
        fprintf(stdout, " total parameters in INSERT: %d\n", param_count);

    if (param_count != binds) /* validate parameter count */
    {
      fprintf(stderr, " wrong parameter count returned by MySQL\n");
      exit(0);
    }
    /* Bind the data for all  parameters */

    if(*c->DEBUG) printf("switching between bind types\n");

    switch( type ){

        case(SCAN_TYPE):

            // datetime
            bind[0].buffer_type = MYSQL_TYPE_LONG;
            bind[0].buffer= (char *)args->v_time;
            bind[0].is_null= 0;
            bind[0].length= 0;

            // spectra string
            bind[1].buffer_type = MYSQL_TYPE_STRING;
            //bind[1].buffer= (char *)args->encode;
            bind[1].buffer= args->encode;
            bind[1].buffer_length= *(args->encode_len);
            bind[1].is_null= 0;
            bind[1].length= 0;

        break;

        case(SPECTRUM_TYPE): 

            // parent scan
            bind[0].buffer_type = MYSQL_TYPE_LONG;
            bind[0].buffer= (char *)args->id;
            bind[0].is_null= 0;
            bind[0].length= 0;

            // long time
            bind[1].buffer_type = MYSQL_TYPE_LONG;
            bind[1].buffer= (char *)args->s_time;
            bind[1].is_null= 0;
            bind[1].length= 0;

            // int exposure
            bind[2].buffer_type = MYSQL_TYPE_LONG;
            bind[2].buffer= (char *)args->exposure;
            bind[2].is_null= 0;
            bind[2].length= 0;

            // spectra string
            bind[3].buffer_type = MYSQL_TYPE_STRING;
            //bind[3].buffer= (char *)args->encode;
            bind[3].buffer= args->encode;
            bind[3].buffer_length= *(args->encode_len);
            bind[3].is_null= 0;
            bind[3].length= 0;
        break;

        case(VOXEL_TYPE): 

            // PARENT SCAN
            bind[0].buffer_type = MYSQL_TYPE_LONG;
            bind[0].buffer= (char *)args->id;
            bind[0].is_null= 0;
            bind[0].length= 0;

            // PARENT SPEC
            bind[1].buffer_type = MYSQL_TYPE_LONG;
            bind[1].buffer= (char *)args->spec_id;
            bind[1].is_null= 0;
            bind[1].length= 0;

            // INT time
            bind[2].buffer_type= MYSQL_TYPE_LONG;
            bind[2].buffer= (char *)args->v_time;
            bind[2].is_null= 0;
            bind[2].length= 0;

            // INT x
            bind[3].buffer_type= MYSQL_TYPE_SHORT;
            bind[3].buffer= (char *)args->x;
            bind[3].is_null= 0;
            bind[3].length= 0;

            // INT y
            bind[4].buffer_type= MYSQL_TYPE_SHORT;
            bind[4].buffer= (char *)args->y;
            bind[4].is_null= 0;
            bind[4].length= 0;

            // INT z
            bind[5].buffer_type= MYSQL_TYPE_SHORT;
            bind[5].buffer= (char *)args->z;
            bind[5].is_null= 0;
            bind[5].length= 0;

        break;
    }

        /* Bind the buffers */
    if (mysql_stmt_bind_param(stmt, bind))
    {
      fprintf(stderr, " mysql_stmt_bind_param() failed\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
      exit(0);
    }
    else if(*c->DEBUG)
        printf("bound!\n");

    // DATABASE PREPARED FOR ENTRY

}

void array2string( double* spectrum, unsigned char* string )
{

    memset( string, 0, sizeof(encode_length*sizeof(unsigned char)));
    unsigned int temp;

    for(int i=0; i<PIXELS; i++){

        temp = spectrum[i];
            // printf("%f -> %d\n",spectrum[i],temp);

        string[4*i]  = encode_start + ( (temp>>12) & 0xF); 
        string[4*i+1] = encode_start + ( (temp>>8) & 0xF);
        string[4*i+2] = encode_start + ( (temp>>4) & 0xF); 
        string[4*i+3] = encode_start +  ( temp   & 0xF);
            // printf("%d~%d",string[2*i],string[2*i+1]);
    }
}

void destroyAll( MYSQL *mysql )
{
    char buffer[50];
    printf("If you need to clear all data and \n"
           "reinitialize the tables, enter a K...\n"
           "Otherwise, rethink... \n");
    fgets( buffer, 50, stdin );
    if( buffer[0] != 'K' )
        exit(0);

    if (mysql_query(mysql, DROP_VOXELS))
    {
      fprintf(stderr, " DROP voxels TABLE failed\n");
      fprintf(stderr, " %s\n", mysql_error(mysql));
      exit(0);
    }
        if (mysql_query(mysql, DROP_SPECTRA))
    {
      fprintf(stderr, " DROP spectra TABLE failed\n");
      fprintf(stderr, " %s\n", mysql_error(mysql));
      exit(0);
    }
        if (mysql_query(mysql, DROP_SCANS))
    {
      fprintf(stderr, " DROP scans TABLE failed\n");
      fprintf(stderr, " %s\n", mysql_error(mysql));
      exit(0);
    }
    printf("...dropped...");


    if (mysql_query(mysql, CREATE_SCANS))
    {
      fprintf(stderr, " CREATE scans TABLE failed\n");
      fprintf(stderr, " %s\n", mysql_error(mysql));
      exit(0);
    }

    if (mysql_query(mysql, CREATE_SPECTRA))
    {
      fprintf(stderr, " CREATE spectra TABLE failed\n");
      fprintf(stderr, " %s\n", mysql_error(mysql));
      exit(0);
    }

    if (mysql_query(mysql, CREATE_VOXELS))
    {
      fprintf(stderr, " CREATE voxels TABLE failed\n");
      fprintf(stderr, " %s\n", mysql_error(mysql));
      exit(0);
    }
    printf("...renewed...");

}




