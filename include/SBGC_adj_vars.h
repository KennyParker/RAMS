/*
	SimpleBGC Serial API  library - Adjustable Variables
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

#ifndef  __SBGC_adj_vars__
#define  __SBGC_adj_vars__

#include <inttypes.h>

// Definitions of all available variables
enum {
	ADJ_VAR_P_ROLL
	,	ADJ_VAR_P_PITCH
	,	ADJ_VAR_P_YAW
	,	ADJ_VAR_I_ROLL
	,	ADJ_VAR_I_PITCH
	,	ADJ_VAR_I_YAW
	,	ADJ_VAR_D_ROLL
	,	ADJ_VAR_D_PITCH
	,	ADJ_VAR_D_YAW
	,	ADJ_VAR_POWER_ROLL
	,	ADJ_VAR_POWER_PITCH
	,	ADJ_VAR_POWER_YAW
	,	ADJ_VAR_ACC_LIMITER
	,	ADJ_VAR_FOLLOW_SPEED_ROLL
	,	ADJ_VAR_FOLLOW_SPEED_PITCH
	,	ADJ_VAR_FOLLOW_SPEED_YAW
	,	ADJ_VAR_FOLLOW_LPF_ROLL
	,	ADJ_VAR_FOLLOW_LPF_PITCH
	,	ADJ_VAR_FOLLOW_LPF_YAW
	,	ADJ_VAR_RC_SPEED_ROLL
	,	ADJ_VAR_RC_SPEED_PITCH
	,	ADJ_VAR_RC_SPEED_YAW
	,	ADJ_VAR_RC_LPF_ROLL
	,	ADJ_VAR_RC_LPF_PITCH
	,	ADJ_VAR_RC_LPF_YAW
	,	ADJ_VAR_RC_TRIM_ROLL
	,	ADJ_VAR_RC_TRIM_PITCH
	,	ADJ_VAR_RC_TRIM_YAW
	,	ADJ_VAR_RC_DEADBAND
	,	ADJ_VAR_RC_EXPO_RATE
	,	ADJ_VAR_FOLLOW_MODE
	,	ADJ_VAR_RC_FOLLOW_YAW
	,	ADJ_VAR_FOLLOW_DEADBAND
	,	ADJ_VAR_FOLLOW_EXPO_RATE
	,	ADJ_VAR_FOLLOW_ROLL_MIX_START
	,	ADJ_VAR_FOLLOW_ROLL_MIX_RANGE
	,	ADJ_VAR_GYRO_TRUST
};


#endif // __SBGC_adj_vars__
