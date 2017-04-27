#include <stdint.h>

#define A_TIME 50000.0
#define DEVICE1 "/dev/serial0"
#define BAUDRATE B115200

#define RADS_OF (M_PI/180)*
#define DEGR_OF (180/M_PI)*

#define MAX_SPAN 180

#define second (1000000.0 / A_TIME) 

#define yawPeriod 	123.0
#define pitchPeriod 19.0
#define rollPeriod	67.0

#define yawDist		90.0
#define pitchDist	25.0
#define rollDist	10.0

struct attitude{

	float yaw;
	float pitch;
	float roll;
};


void turn(struct angle *spin, int time);
int openUart();
int set_interface_attribs(int fd, int speed);
void sendCommand(int uart, uint8_t cmd, void *data, uint16_t size);
void killmotors();