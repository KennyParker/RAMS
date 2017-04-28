#include <stdbool.h>
#include <stdio.h>
#include <stdatomic.h>
#include <string.h>
#include <math.h>
#include <my_global.h>
#include <mysql.h>

#include "control.h"
#include "spectrometer.h"
#include "queue.h"
#include "gimbal.h"
#include "database.h"
#include "consumer.h"


void* consumer(void *p)
{   
    struct consumeAll *all = (struct consumeAll*)p;

    struct LamportQueue *lidar = all->lidar;
    struct LamportQueue *angles = all->angles;
    struct LamportQueue *spectral = all->spectral;
    struct control *c = all->control;

    bool finished = false;

    struct laser point;
    struct angle angle1, angle2;
    struct spectral spectrum1, spectrum2;

    triInitialize( all, &point, &angle1, &angle2, &spectrum1, &spectrum2, c );
    if(*c->DEBUG)
       printf("both queues are ready\n");
    int mid_gap = (spectrum1.time + spectrum1.exposure/1.0e3 + spectrum2.time) / 2.0;
            // voxels inbetween exposures choose sides based on the mid gap

    mysql = mysql_init(NULL);

    MYSQL_STMT    *vox_stmt;
    MYSQL_BIND    vox_bind[BINDS_VOXEL];
    memset(vox_bind, 0, sizeof(vox_bind));

    MYSQL_STMT    *spec_stmt;
    MYSQL_BIND    spec_bind[BINDS_SPECTRA];
    memset(spec_bind, 0, sizeof(spec_bind));

    /* Connect to database */
    if (!mysql_real_connect(mysql, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(mysql));
        exit(0);
    }
    vox_stmt = mysql_stmt_init(mysql);
    if (!vox_stmt)
    {
        fprintf(stderr, " mysql_stmt_init(), out of memory\n");
        exit(0);
    }
    spec_stmt = mysql_stmt_init(mysql);
    if (!spec_stmt)
    {
        fprintf(stderr, " mysql_stmt_init(), out of memory\n");
    }

    // both belong to SCAN
    int id = *c->scan_id;
    if(*c->DEBUG) printf("binding voxel to scan_id %d \n", id );

    // VOXEL     
    int spectra_id = 1;
    int v_time = 1;
    int x = 1;
    int y = 1;
    int z = 1;

    struct arg_struct vox_args;
    vox_args.id = &id;
    vox_args.spec_id = &spectra_id;
    vox_args.v_time = &v_time;
    vox_args.x = &x;
    vox_args.y = &y;
    vox_args.z = &z;

    if(*c->DEBUG) printf("vox args bound\n");
    
    prepareTable( mysql, vox_stmt, vox_bind, VOXEL_TYPE, &vox_args, c );
    if(*c->DEBUG) printf("vox table preapred\n");


    //MYSQL *spec_mysql = mysql_init(NULL);

    /* Connect to database */
    /*
    if (!mysql_real_connect(spec_mysql, server, user, password, database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(spec_mysql));
        exit(0);
    }
    
    spec_stmt = mysql_stmt_init(spec_mysql);
    if (!spec_stmt)
    {
        fprintf(stderr, " mysql_stmt_init(), out of memory\n");
        exit(0);
    }
    */

    // MOCK SPECTRA
    int s_time = 1;
    int exposure = 1;
    int len = encode_length;
    unsigned char encode[encode_length];
    array2string(spectrum1.spectrum, encode);


    struct arg_struct spec_args;
    spec_args.id = &id;
    spec_args.s_time = &s_time;
    spec_args.exposure = &exposure;
    spec_args.encode = encode;
    spec_args.encode_len = &len;    


    if(*c->DEBUG) printf("spec args bound\n");

    prepareTable( mysql, spec_stmt, spec_bind, SPECTRUM_TYPE, &spec_args, c );
    if(*c->DEBUG) printf("spec table preapred\n");


    execute(spec_stmt);
    spectra_id = mysql_insert_id(mysql);
    if(*c->DEBUG) printf("inserted spectra %d\n", spectra_id);

    do{

        if(*c->DEBUG) if(*c->STOP) printf("finishing queues \n");

        if( point.time > angle2.time ){
            angle1 = angle2;
            
            angle2.time = 0;
            do{
                while(! LamportQueue_pop(angles, &angle2)) // wait for next angle
                    ;
                if( *c->STOP ){
                    finished = true ;
                    break;
                }

            } while( angle2.time < 1 ); //hack to avoid popped bas data
    
        }
        if( point.time > mid_gap ){
            spectrum1 = spectrum2;

            spectrum2.time = 0;
            do{
                while(! LamportQueue_pop(spectral, &spectrum2)) // wait for next spectra
                if( *c->STOP ){
                    finished = true ;
                    break;
                }
            } while( spectrum2.time < 1);
            
            mid_gap = (spectrum1.time + spectrum1.exposure/1.0e3 + spectrum2.time) / 2.0;
            
            if( *c->DEBUG)
                printf("spectra popped: %f \n", spectrum2.spectrum[0]);

            array2string(spectrum1.spectrum, encode);
            exposure = spectrum1.exposure;
            s_time = spectrum1.time;

            execute(spec_stmt);
            spectra_id = mysql_insert_id(mysql);


            if( *c->DEBUG)
                printf("inserted spectra %d\n", spectra_id);
        }

        if(*c->DEBUG) printf("entered vox write stage \n");

        if(*c->RUN_LASER){
            printf("laser broken\n");
            convert( &point, &angle1, &angle2, &x, &y, &z );
        }
        else flat_convert( &point, &angle1, &angle2, &x, &y, &z );

        if(*c->DEBUG) printf("passed convert\n");
        v_time = point.time;

        execute(vox_stmt);

        if( *c->OUTPUT )
            printf("dist written: %d \n", point.distance );

        point.time = 0;
        do{
            while(! LamportQueue_pop(lidar, &point)) // wait for next point
            if( *c->STOP ){
                    finished = true ;
                    break;
            }
        }while( point.time < 1);
        
    
    } while( ! finished );
    usleep(2.0e6);
    *c->FINISHED = true;

    if (mysql_stmt_close(vox_stmt))
    {
      fprintf(stderr, " failed while closing the statement\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(vox_stmt));
      exit(0);
    }
    // mysql_close(vox_mysql);

    if (mysql_stmt_close(spec_stmt))
    {
      fprintf(stderr, " failed while closing the statement\n");
      fprintf(stderr, " %s\n", mysql_stmt_error(spec_stmt));
      exit(0);
    }    
    // mysql_close(spec_mysql);

    if(*c->DEBUG) printf("consumer finished\n");
    return 0;
}

void convert(struct laser *point, struct angle *angle1, struct angle *angle2, int *x, int *y, int *z)
{
        static float normalized, yaw, pitch, roll, azimuth, inclination;

        normalized = (float)(point->time-angle1->time) / (float)(angle2->time-angle1->time); 
    
        yaw = RADS_OF ( angle1->yaw + normalized * (angle2->yaw - angle1->yaw) );
        pitch = RADS_OF ( angle1->pitch + normalized * (angle2->pitch - angle1->pitch) );
        roll = RADS_OF ( angle1->roll + normalized * (angle2->roll - angle1->roll) );

        azimuth = atan2f( roll, pitch ) - yaw - RADS_OF 90;
        inclination = sqrtf( roll*roll + pitch*pitch );

        *x = point->distance * sinf(inclination) * cosf(azimuth);
        *y = point->distance * sinf(inclination) * sinf(azimuth);
        *z = point->distance * cosf(inclination);


        printf("x%d\ty%d\tz%d    y%.2f\tp%.2f\tr%.2f\n",*x,*y,*z, DEGR_OF yaw, DEGR_OF pitch, DEGR_OF roll);
}

void flat_convert(struct laser *point, struct angle *angle1, struct angle *angle2, int *x, int *y, int *z)
{
        static float yaw, pitch, roll, azimuth, inclination, range;

        if( abs(point->time - angle1->time) < abs(angle2->time - point->time) ){
            yaw = RADS_OF angle1->yaw ;
            pitch = RADS_OF angle1->pitch ;
            roll = RADS_OF angle1->roll ;
        }
        else{
            yaw = RADS_OF angle2->yaw ;
            pitch = RADS_OF angle2->pitch ;
            roll = RADS_OF angle2->roll ;
        }
        
        *z = 160;
        azimuth = atan2f( roll, pitch ) - yaw + RADS_OF 90;
        inclination = sqrtf( roll*roll + pitch*pitch );

        range = *z / cosf(inclination);

        *x = range * sinf(inclination) * cosf(azimuth);
        *y = range * sinf(inclination) * sinf(azimuth);
        *z -= 160;

        printf("x%d\ty%d\tz%d    y%.2f\tp%.2f\tr%.2f\n",*x,*y,*z, DEGR_OF yaw, DEGR_OF pitch, DEGR_OF roll);
}

void triInitialize( struct consumeAll *all, struct laser *point, struct angle *angle1, struct angle *angle2, struct spectral *spectrum1, struct spectral *spectrum2, struct control *c)
{
    struct LamportQueue *lidar = all->lidar;
    struct LamportQueue *angles = all->angles;
    struct LamportQueue *spectral = all->spectral;

    if(*c->DEBUG)
        printf("initalization started\n");

    // FLUSH QUEUES
    while( ! LamportQueue_pop(spectral, spectrum1) ) // wait until first spectra
        ;
    if(*c->DEBUG) printf("first spectral time = %d \n", spectrum1->time);

    while( ! LamportQueue_pop(angles, angle1)) // wait until first angle
        ;
    if(*c->DEBUG) printf("first angle time = %d \n", angle1->time);
    
    while( ! LamportQueue_pop(lidar, point)) // wait until first
        ;
    if(*c->DEBUG) printf("first lidar time = %d \n", point->time);


    while( angle1->time < spectrum1->time ){ // find first angle measurement taken during spectra
        LamportQueue_pop(angles, angle1);
    }
    if(*c->DEBUG) printf("first angle within first spectra time = %d \n", angle1->time);

    while( point->time < angle1->time ) // find first laser after first angle
        LamportQueue_pop(lidar, point);
    if(*c->DEBUG) printf("first lidar within first angle time = %d \n", point->time);

    while( ! LamportQueue_pop(angles, angle2)) // wait until second angle
        ;
    if(*c->DEBUG)printf("second angle time = %d \n", angle2->time);

    while( ! LamportQueue_pop(spectral, spectrum2) ) // wait until second spectra
        ;
    if(*c->DEBUG) printf("second spectral time = %d \n", spectrum2->time);

    if(*c->DEBUG)
        printf("tri initalization finished\n");

}

