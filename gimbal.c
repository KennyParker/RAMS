#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <math.h>
#include <termios.h>
#include <errno.h>
#include <my_global.h>
#include <mysql.h>


#include "SBGC.h"

#include "control.h"
#include "spectrometer.h"
#include "queue.h"
#include "gimbal.h"

void* gimbalController(void *p)
{
    struct controlQueue *angles_control = (struct controlQueue*)p;
    struct LamportQueue *queue = angles_control->queue;
    struct control *c = angles_control->control;

    if(*c->DEBUG)
        printf( "gimbal queue type: %d \n", queue->type );

    struct angle aim;

    int i = 1;
    struct timespec now;
    int step = 0;

    //struct attitude aim = {0};
    int basecamUart = openUart();
    if(basecamUart == -1){
        perror("basecamUart didn't open\n");
        exit(0);
    }

    SBGC_cmd_control_data cmd_control_data = {0};
    cmd_control_data.mode = SBGC_CONTROL_MODE_ANGLE;
    
    cmd_control_data.speedYAW = 40 * yawDist / yawPeriod * SBGC_SPEED_SCALE;
    cmd_control_data.speedPITCH = 40 * pitchDist / pitchPeriod * SBGC_SPEED_SCALE;
    cmd_control_data.speedROLL = 40 * rollDist / rollPeriod * SBGC_SPEED_SCALE;

    printf("ya%f \t pi%f \t ro%f \n", 2.0 * yawDist / yawPeriod, 2.0 * pitchDist / pitchPeriod, 2.0 * rollDist / rollPeriod);


    aim.yaw = 0;
    aim.pitch = 0;
    aim.roll = 0;

    sendCommand(basecamUart, SBGC_CMD_MOTORS_ON, 0, 0);

    while( !*c->STOP ){
       
        usleep( A_TIME );
	    
        if(step > (pitchPeriod*second * rollPeriod*second * yawPeriod*second) ){
            step = 0; // prevents iter overflow
            printf("OVERFLOW PREVENTED\n");
        }
        turn( &aim, step++ );

        // printf("y%f\t p%f\t r%f \n", aim.yaw, aim.pitch, aim.roll);

        cmd_control_data.angleROLL = SBGC_DEGREE_TO_ANGLE(aim.roll) ;
        cmd_control_data.anglePITCH = SBGC_DEGREE_TO_ANGLE(aim.pitch) ;
        cmd_control_data.angleYAW = SBGC_DEGREE_TO_ANGLE(aim.yaw) ;
        
        sendCommand(basecamUart, SBGC_CMD_CONTROL, &cmd_control_data, sizeof(cmd_control_data));
        
        clock_gettime(CLOCK_REALTIME, &now);
        aim.time = (now.tv_sec - *c->start_time) * 1000 + (now.tv_nsec) / 1.0e6 ;

        if(LamportQueue_push(queue, (void*)&aim) )  // all's well
            i++;
        else if( *c->DEBUG ) printf( "angle queue full\n");
    }

    usleep( A_TIME );
    cmd_control_data.angleROLL = cmd_control_data.anglePITCH = cmd_control_data.angleYAW = 0;

    sendCommand(basecamUart, SBGC_CMD_CONTROL, &cmd_control_data, sizeof(cmd_control_data));
    usleep( 5.0e6 );
    sendCommand(basecamUart, SBGC_CMD_MOTORS_OFF, 0, 0);

    if(*c->DEBUG) printf("gimbal finished\n");
    
    while( ! *c->FINISHED ) // wait up
      ;
    return 0; 
} 

void turn(struct angle *spin, int step ){

    // int yawState = step % (int)(yawPeriod * second);
    int pitchState = step % (int)(pitchPeriod * second);
    int rollState = step % (int)(rollPeriod * second);
 
    // spin->yaw = yawDist * sinf( 2 * M_PI * yawState / (yawPeriod * second) );
    spin->pitch = pitchDist * sinf( 2 * M_PI * pitchState / (pitchPeriod * second) );
    spin->roll = rollDist * sinf( 2 * M_PI * rollState / (rollPeriod * second) );

    spin->yaw = 0;
    //spin->roll = 0;
    //spin->pitch = 0;

}

int openUart()
{
    int fd = open(DEVICE1, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Error opening %s \n", DEVICE1 );
        return -1;
    }
    /*baudrate 115200, 8 bits, no parity, 1 stop bit */
    set_interface_attribs(fd, BAUDRATE);

    return fd;
}

int set_interface_attribs(int fd, int speed)
{
    struct termios tty;

    if (tcgetattr(fd, &tty) < 0) {
        printf("Error from tcgetattr \n");
        return -1;
    }

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;         /* 8-bit characters */
    tty.c_cflag &= ~PARENB;     /* no parity bit */
    tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error from tcsetattr \n");
        return -1;
    }
    return 0;
}

void sendCommand(int uart, uint8_t cmd, void *data, uint16_t size)
{
    int i=0,j=0;
    uint8_t buff[SBGC_CMD_MAX_BYTES];
    uint8_t checksum=0;

    if( size <= (SBGC_CMD_MAX_BYTES - SBGC_CMD_PAYLOAD_BYTES) ) {
        
        // printf("size small enough\n");

        buff[j++] = (uint8_t)'>';
        buff[j++] = (uint8_t)cmd;
        buff[j++] = (uint8_t)size;
        buff[j++] = (uint8_t)(size + cmd) % 256;
        
        for(i=0; i < size; i++){
            checksum += ((uint8_t*)data)[i];
            buff[j++] = ((uint8_t*)data)[i];
        }
        buff[j++] = checksum % 256;
/*
        printf("\n ——— sending command:\n");
        for(int k=0; k<j; k++ ){
            printf( "%X ", buff[k] );
        }
        printf("\n ——— \n");
*/      
        write(uart,buff,j);
        
    }
    else{
        printf( "too large\n" );
    }
}

void killmotors(){

    int basecamUart = openUart();
    if(basecamUart == -1){
        perror("basecamUart didn't open\n");
        exit(0);
    }
    char ch = 'f';
    while(ch!='K'){
        sendCommand(basecamUart, SBGC_CMD_MOTORS_OFF, 0, 0);
        usleep(200000);
        ch = getchar();
    }

}


