
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPiI2C.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <unistd.h>
#include <my_global.h>
#include <mysql.h>

#include "control.h"
#include "spectrometer.h"
#include "lidar.h"
#include "queue.h"


void* laserRangefinder(void *p)
{
  struct controlQueue *lidar_control = (struct controlQueue*)p;
  struct LamportQueue *queue = lidar_control->queue;
  struct control *c = lidar_control->control;

    if(*c->DEBUG)
        printf( "laser queue type: %d \n", queue->type );

    struct laser rangefinder;
    
    int init = lidar_init(false);
    int i=1;
    struct timespec now;

    while( ! *c->STOP ){
        usleep( L_TIME );

        clock_gettime(CLOCK_REALTIME, &now);
        rangefinder.time = (now.tv_sec - *c->start_time) * 1000 + (now.tv_nsec) / 1.0e6 ;
        
        if(*c->RUN_LASER) rangefinder.distance = 9 + lidar_read(init);  // 9cm from lidar end to axis
        else rangefinder.distance = 42;


        if( rangefinder.distance > 6000 || rangefinder.distance < 1 )
            printf("absurd laser: %d \n", rangefinder.distance );
        else{
            if( *c->DEBUG && *c->OUTPUT ) printf("laser: %d \n", rangefinder.distance );

            if(LamportQueue_push(queue, (void*)&rangefinder) ) // all's well
                i++;
            else if( *c->DEBUG ){
                printf( "laser queue full\n");
                exit(0);
            }
        }
    }
    if(*c->DEBUG) printf("lidar finished \n");

    while( ! *c->FINISHED ) // wait up
      ;
    return 0;
}

int lidar_init(bool DEBUG) {
	int fd;
    if (DEBUG) printf("LidarLite V0.1\n\n");
    fd = wiringPiI2CSetup(LIDAR_LITE_ADRS);
    if (fd != -1) {
        lidar_status(fd);  // Dummy request to wake up device
        usleep(100);
        }
    return(fd);
}        

// Read distance in cm from LidarLite
int lidar_read(int fd) {
       
       int hiVal, loVal;

       // send "measure" command
       hiVal = wiringPiI2CWriteReg8(fd, MEASURE_REG, MEASURE_VAL);
          // printf("write res=%d\n", hiVal);
       usleep(20);
   
       // Read second byte and append with first 
       loVal = _read_byteNZ(fd, DISTANCE_REG_LO) ;        
          // printf(" Lo=%d\n", loVal);
       
       // read first byte 
       hiVal = _read_byte(fd, DISTANCE_REG_HI) ;             
          // printf ("Hi=%d ", hiVal);
       
       return ( (hiVal << 8) + loVal);
}

unsigned char lidar_version(int fd) {
    return( (unsigned char) _read_byteNZ(fd, VERSION_REG) );
}
         
unsigned char lidar_status(int fd) {
    return( (unsigned char) wiringPiI2CReadReg8(fd, STATUS_REG) );
}
            
void lidar_status_print(unsigned char status) {
    if (status != 0) 
        printf("STATUS BYTE: 0x%x ", (unsigned int) status);

	if (status & STAT_BUSY) printf("busy \n");              
	if (status & STAT_REF_OVER) printf("reference overflow \n");            
	if (status & STAT_SIG_OVER) printf("signal overflow \n");            
	if (status & STAT_PIN) printf("mode select pin \n");                 
	if (status & STAT_SECOND_PEAK) printf("second peak \n");         
	if (status & STAT_TIME) printf("active between pairs \n");                
	if (status & STAT_INVALID) printf("no signal \n");             
	if (status & STAT_EYE) printf(" eye safety \n");                 
}    
    
    // Read a byte from I2C register.  Repeat if not ready
unsigned char  _read_byte(int fd, int reg){
	return _read_byte_raw(fd, reg, true);
}
    
        // Read Lo byte from I2C register.  Repeat if not ready or zero
unsigned char  _read_byteNZ(int fd, int reg) {
	return _read_byte_raw(fd, reg, false);
}
    
    // Read byte from I2C register.  Special handling for zero value
unsigned char  _read_byte_raw(int fd, int reg, bool allowZero) {
    int i;
    unsigned char val;
    usleep(1);
    while (true) {
        val = wiringPiI2CReadReg8(fd, reg);
        
        // Retry on error
        if (val == ERROR_READ || (val==0 && !allowZero) ) {
            usleep(20) ;		// ms
           // if (DEBUG) printf(".");
            if (i++ > 50) {
               // Timeout
               printf("Timeout");
               return (ERROR_READ);
               }
          }
          else return(val);
    }   
}

void lidar()     // unused, complete and prints status
{
	int fd, res, del=1000;
    unsigned char st;
	// unsigned char ver;

	fd = lidar_init(true);

  //printf("attempting reset\n");
  //wiringPiI2CWrite(fd, 0x00);
  //printf("fpga should be cleared\n");
  //usleep(2000000);

int lo, hi;
   // Read second byte and append with first 
  lo = _read_byteNZ(fd, 0x17) ;        
    // printf(" Lo=%d\n", loVal);
  // read first byte 
  hi = _read_byte(fd, 0x16) ;             
    // printf ("Hi=%d ", hiVal);

  printf("serial = %d\n", ( (hi << 8) + lo) );

    if (fd == -1) {
        printf("initialization error\n");
    }
    else {
          res = lidar_read(fd);
          st = lidar_status(fd);
          //ver = lidar_version(fd);
          
          printf("%3.0d cm \n", res);
          lidar_status_print(st);
          
          usleep(del);
    }
}
