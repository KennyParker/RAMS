/* 
	SimpleBGC Serial API  library - RC (Remote Control) definitions
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
#ifndef  __SBGC_rc__
#define  __SBGC_rc__


// Hardware RC inputs, as labeled on the board
#define SBGC_RC_INPUT_NO 0
#define SBGC_RC_INPUT_ROLL 1
#define SBGC_RC_INPUT_PITCH 2
#define SBGC_RC_INPUT_EXT_ROLL 3
#define SBGC_RC_INPUT_EXT_PITCH 4
#define SBGC_RC_INPUT_YAW 5 // not connected in 1.0 board

// Analog inputs (board v.3.0)
#define SBGC_RC_INPUT_ADC1 33
#define SBGC_RC_INPUT_ADC2 34
#define SBGC_RC_INPUT_ADC3 35



// Bit indicates input is in analog mode
#define SBGC_RC_INPUT_ANALOG_BIT (1<<5) // 32
// Bit indicates input is a virtual channel
#define SBGC_RC_INPUT_VIRT_BIT (1<<6) // 64
// Bit indicates input is a API virtual channel
#define SBGC_RC_INPUT_API_VIRT_BIT (1<<7) // 128

// Mask to separate input channel number from input mode
#define SBGC_RC_INPUT_CH_MASK ((1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4)) 
#define SBGC_RC_INPUT_MODE_MASK ((1<<5) | (1<<6) | (1<<7))


// Number of virtual channels for RC serial input (s-bus, spektrum, Sum-PPM)
#define SBGC_VIRT_NUM_CHANNELS 32
// Number of virtual channels for API serial input
#define SBGC_API_VIRT_NUM_CHANNELS 32


// Normal range of RC signal. Real signal may go outside this range
#define SBGC_RC_MIN_VAL -500
#define SBGC_RC_MAX_VAL 500
// Value to encode 'RC no signal'
#define SBGC_RC_UNDEF -10000


// Control modes
#define SBGC_CONTROL_MODE_NO          0
#define SBGC_CONTROL_MODE_SPEED       1
#define SBGC_CONTROL_MODE_ANGLE       2
#define SBGC_CONTROL_MODE_SPEED_ANGLE 3
#define SBGC_CONTROL_MODE_RC          4


#endif // __SBGC_rc__
