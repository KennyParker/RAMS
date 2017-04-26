#include <stdint.h>

#define A_TIME 25000
#define DEVICE1 "/dev/serial0"
#define BAUDRATE B115200

#define RADS_OF (M_PI/180)*
#define DEGR_OF (180/M_PI)*

#define MAX_SPAN 180

#define second (1000000 / A_TIME) 
#define yawPeriod (37 * second)
#define pitchPeriod (13 * second)
#define rollPeriod (2 * second)

struct attitude{

	float yaw;
	float pitch;
	float roll;
};


void turn(struct angle *spin, int time);
int openUart();
int set_interface_attribs(int fd, int speed);
void sendCommand(int uart, uint8_t cmd, void *data, uint16_t size);