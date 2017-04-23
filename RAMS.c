#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <my_global.h>
#include <mysql.h>

#include "control.h"
#include "lidar.h"
#include "gimbal.h"
#include "spectrometer.h"
#include "queue.h"

void* controller(void *p);
void* laserRangefinder(void *p);
void* gimbalController(void *p);
void* spectrometer(void *p);
void* consumer(void *p);

int main()
{
    printf("size S :%d\n", SIZE_S);
    printf("size A :%d\n", SIZE_A);
    printf("size L :%d\n", SIZE_L);

    struct control control;
    init_control( &control );

    printf("controls set, DEBUG = %s \n", control.DEBUG?"on":"off");
   
    struct LamportQueue lidar;
    LamportQueue_init( &lidar, LASER_TYPE );
    struct controlQueue lidar_control = { &lidar, &control };

    struct LamportQueue angles;
    LamportQueue_init( &angles, ANGLE_TYPE );
    struct controlQueue angles_control = { &angles, &control };

    struct LamportQueue spectral;
    LamportQueue_init( &spectral, SPECTRAL_TYPE );
    struct controlQueue spectral_control = { &spectral, &control };

    struct controlAll all = { &lidar, &angles, &spectral, &control };


    pthread_t t[5];
    pthread_create(&t[0], 0, controller, &control);
	while( ! *control.READY ) // wait for control thread
        ;
    printf("READY\n");

    pthread_create(&t[1], 0, laserRangefinder, &lidar_control);
    pthread_create(&t[2], 0, gimbalController, &angles_control);
    pthread_create(&t[3], 0, spectrometer, &spectral_control);
    pthread_create(&t[4], 0, consumer, &all);

    pthread_join(t[4], 0);
    pthread_join(t[3], 0);
    pthread_join(t[2], 0);
    pthread_join(t[1], 0);
    pthread_join(t[0], 0);
}

