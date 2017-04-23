/* 
	SimpleBGC Serial API  library
	More info: http://www.basecamelectronics.com

  * last edit date: 18.10.2014
  * compatible with the revision 2.4 of Serial API specification.


	Copyright (c) 2014 Aleksei Moskalenko
	All rights reserved.
	
	
  This software is free for use and redistribution under a BSD license:
   
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
	    * Redistributions of source code must retain the above copyright
	      notice, this list of conditions and the following disclaimer.
	    * Redistributions in binary form must reproduce the above copyright
	      notice, this list of conditions and the following disclaimer in the
	      documentation and/or other materials provided with the distribution.
	    * Neither the name of the Basecamelectronics company nor the
	      names of its contributors may be used to endorse or promote products
	      derived from this software without specific prior written permission.
	
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/   

#ifndef  __SBGC__
#define  __SBGC__

#include "inttypes.h" 
#include "SBGC_command.h"
#include "SBGC_rc.h"
#include "SBGC_adj_vars.h"





// Error codes
#define SBGC_ERR_CMD_SIZE 1
#define SBGC_ERR_WRONG_PARAMS 2
#define SBGC_ERR_GET_DEVICE_ID  3
#define SBGC_ERR_CRYPTO		4
#define SBGC_ERR_CALIBRATE_BAT 5
#define SBGC_ERR_UNKNOWN_COMMAND 6



// Trigger pins
#define SBGC_PIN_AUX1 16
#define SBGC_PIN_AUX2 17
#define SBGC_PIN_AUX3 18
#define SBGC_PIN_BUZZER 32


// Value passed in CMD_SERVO_OUT to disable servo output
#define SBGC_SERVO_OUT_DISABLED -1

//////////////// Units conversion /////////////////
// Conversion from degree/sec to units that command understand
#define SBGC_SPEED_SCALE  (1.0f/0.1220740379f)

// Conversions for angle in degrees to angle in SBGC 16bit representation, and back
#define SBGC_DEGREE_TO_ANGLE(d) ((int16_t)((float)(d)*(1.0f/0.02197265625f)))
#define SBGC_ANGLE_TO_DEGREE(d) ((float)(d)*0.02197265625f)







#endif //__SBGC__