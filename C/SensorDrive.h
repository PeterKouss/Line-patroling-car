#ifndef __SENSORDRIVE_H__
#define __SENSORDRIVE_H__

#include <MKL25Z4.H>
#include <stdio.h>
#include "PrintUart.h"

#define MASK(x) (1UL << (x))

#define FINDWAYDELAY 10000000

//0001000 or 0011100
#define STRAIGHTSPEED 2.5

//0011000
#define FIRST_LITTLE_CURVESPEED_HIGH 2.6
#define FIRST_LITTLE_CURVESPEED_LOW 2.5

//0010000
#define SECOND_LITTLE_CURVESPEED_HIGH 3.0
#define SECOND_LITTLE_CURVESPEED_LOW 1.0

//0110000
#define THIRD_MIDDLE_CURVESPEED_HIGH 3.5
#define THIRD_MIDDLE_CURVESPEED_LOW 0.5

//0100000
#define FOURTH_MIDDLE_CURVESPEED_HIGH 4.0
#define FOURTH_MIDDLE_CURVESPEED_LOW 0.5

//1100000
#define FIFTH_LARGE_CURVESPEED_HIGH 4.5
#define FIFTH_LARGE_CURVESPEED_LOW 0.5

//1000000
#define SIXTH_LARGE_CURVESPEED_HIGH 5.0
#define SIXTH_LARGE_CURVESPEED_LOW 0.5

//1111111 or 0111110
#define CROSSSPEED 2.5

//unknow state
#define UNKNOWNSPEED 1.0

int sensor[7];
int find_way_delay = 0;
int find_way_state = 0;

int last_state = 0;

typedef struct main_speed_set
{
	float main_speed_set_1;
	float main_speed_set_2;
} SPEEDSET;

//Judge Speed main
SPEEDSET Judge_Speed()
{
	SPEEDSET judge_speed_result;

	//judge speed

	//hei wei 1
	sensor[0] = (PTC->PDIR & MASK(17)) >> (17);
	sensor[1] = (PTC->PDIR & MASK(10)) >> (10);
	sensor[2] = (PTC->PDIR & MASK(11)) >> (11);
	sensor[3] = (PTC->PDIR & MASK(12)) >> (12);
	sensor[4] = (PTC->PDIR & MASK(13)) >> (13);
	sensor[5] = (PTC->PDIR & MASK(16)) >> (16);
	sensor[6] = (PTA->PDIR & MASK(16)) >> (16);

	if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 1 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) //0001000 straight
	{
		judge_speed_result.main_speed_set_1 = STRAIGHTSPEED;
		judge_speed_result.main_speed_set_2 = STRAIGHTSPEED;
		last_state = 0; //straight
	}
	else if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 0 && sensor[6] == 0) //0011100 straight
	{
		judge_speed_result.main_speed_set_1 = STRAIGHTSPEED;
		judge_speed_result.main_speed_set_2 = STRAIGHTSPEED;
		last_state = 0; //straight
	}
	else if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) //0011000 1_little_left
	{
		judge_speed_result.main_speed_set_1 = FIRST_LITTLE_CURVESPEED_LOW;
		judge_speed_result.main_speed_set_2 = FIRST_LITTLE_CURVESPEED_HIGH;
		last_state = 1; //1_little_left
	}
	else if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) //0010000 2_little_left
	{
		judge_speed_result.main_speed_set_1 = SECOND_LITTLE_CURVESPEED_LOW;
		judge_speed_result.main_speed_set_2 = SECOND_LITTLE_CURVESPEED_HIGH;
		last_state = 2; //2_little_left
	}
	else if ((sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) || //0110000 3_middle_left
			 (sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) || //0111000 3_middle_left
			 (sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 0 && sensor[6] == 0))   //0111100 3_middle_left
	{
		judge_speed_result.main_speed_set_1 = THIRD_MIDDLE_CURVESPEED_LOW;
		judge_speed_result.main_speed_set_2 = THIRD_MIDDLE_CURVESPEED_HIGH;
		last_state = 3; //3_middle_left
	}
	else if (sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) //0100000 4_middle_left
	{
		judge_speed_result.main_speed_set_1 = FOURTH_MIDDLE_CURVESPEED_LOW;
		judge_speed_result.main_speed_set_2 = FOURTH_MIDDLE_CURVESPEED_HIGH;
		last_state = 4; //4_middle_left
	}
	else if (sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) //1100000 5_mlarge_left
	{
		judge_speed_result.main_speed_set_1 = FIFTH_LARGE_CURVESPEED_LOW;
		judge_speed_result.main_speed_set_2 = FIFTH_LARGE_CURVESPEED_HIGH;
		last_state = 5; //5_mlarge_left
	}
	else if ((sensor[0] == 1 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) || //1000000 6_mlarge_left
			 (sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) || //1110000
			 (sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) || //1111000
			 (sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 0 && sensor[6] == 0) || //1111100
			 (sensor[0] == 1 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0) ||
			 (sensor[0] == 1 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 0)) //1011000
	{
		judge_speed_result.main_speed_set_1 = SIXTH_LARGE_CURVESPEED_LOW;
		judge_speed_result.main_speed_set_2 = SIXTH_LARGE_CURVESPEED_HIGH;
		last_state = 6; //6_mlarge_left
	}
	else if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 0 && sensor[6] == 0) //0001100 1_little_right
	{
		judge_speed_result.main_speed_set_1 = FIRST_LITTLE_CURVESPEED_HIGH;
		judge_speed_result.main_speed_set_2 = FIRST_LITTLE_CURVESPEED_LOW;
		last_state = 7; //1_little_right
	}
	else if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 1 && sensor[5] == 0 && sensor[6] == 0) //0000100 2_little_right
	{
		judge_speed_result.main_speed_set_1 = SECOND_LITTLE_CURVESPEED_HIGH;
		judge_speed_result.main_speed_set_2 = SECOND_LITTLE_CURVESPEED_LOW;
		last_state = 8; //2_little_right
	}
	else if ((sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 0) || //0000110 3_middle_right
			 (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 0) ||
			 ((sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 0))) //0011110
	{
		judge_speed_result.main_speed_set_1 = THIRD_MIDDLE_CURVESPEED_HIGH;
		judge_speed_result.main_speed_set_2 = THIRD_MIDDLE_CURVESPEED_LOW;
		last_state = 9; //3_middle_right
	}
	else if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 0) //0000010 4_middle_right
	{
		judge_speed_result.main_speed_set_1 = FOURTH_MIDDLE_CURVESPEED_HIGH;
		judge_speed_result.main_speed_set_2 = FOURTH_MIDDLE_CURVESPEED_LOW;
		last_state = 10; //4_middle_right
	}
	else if (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 1 && sensor[6] == 1) //0000011 5_mlarge_right
	{
		judge_speed_result.main_speed_set_1 = FIFTH_LARGE_CURVESPEED_HIGH;
		judge_speed_result.main_speed_set_2 = FIFTH_LARGE_CURVESPEED_LOW;
		last_state = 11; //5_mlarge_right
	}
	else if ((sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 0 && sensor[5] == 0 && sensor[6] == 1) || //0000001 6_mlarge_right
			 (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1) || //0000111
			 (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1) || //0001111
			 (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1) || //0011111
			 (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 0 && sensor[4] == 1 && sensor[5] == 0 && sensor[6] == 1) ||
			 (sensor[0] == 0 && sensor[1] == 0 && sensor[2] == 0 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 0 && sensor[6] == 1))
	{
		judge_speed_result.main_speed_set_1 = SIXTH_LARGE_CURVESPEED_HIGH;
		judge_speed_result.main_speed_set_2 = SIXTH_LARGE_CURVESPEED_LOW;
		last_state = 12; //6_mlarge_right
	}
	else if (sensor[0] == 1 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 1) //111111 cross
	{
		judge_speed_result.main_speed_set_1 = CROSSSPEED;
		judge_speed_result.main_speed_set_2 = CROSSSPEED;
		last_state = 13; //cross
	}
	else if (sensor[0] == 0 && sensor[1] == 1 && sensor[2] == 1 && sensor[3] == 1 && sensor[4] == 1 && sensor[5] == 1 && sensor[6] == 0) //011110 cross
	{
		judge_speed_result.main_speed_set_1 = CROSSSPEED;
		judge_speed_result.main_speed_set_2 = CROSSSPEED;
		last_state = 13; //cross
	}
	else
	{
		if (last_state == 0)
		{
			judge_speed_result.main_speed_set_1 = STRAIGHTSPEED;
			judge_speed_result.main_speed_set_2 = STRAIGHTSPEED;
			last_state = 0; //straight
		}
		else if (last_state == 1)
		{
			judge_speed_result.main_speed_set_1 = FIRST_LITTLE_CURVESPEED_LOW;
			judge_speed_result.main_speed_set_2 = FIRST_LITTLE_CURVESPEED_HIGH;
			last_state = 1; //1_little_left
		}
		else if (last_state == 2)
		{
			judge_speed_result.main_speed_set_1 = SECOND_LITTLE_CURVESPEED_LOW;
			judge_speed_result.main_speed_set_2 = SECOND_LITTLE_CURVESPEED_HIGH;
			last_state = 2; //2_little_left
		}
		else if (last_state == 3)
		{
			judge_speed_result.main_speed_set_1 = THIRD_MIDDLE_CURVESPEED_LOW;
			judge_speed_result.main_speed_set_2 = THIRD_MIDDLE_CURVESPEED_HIGH;
			last_state = 3; //3_middle_left
		}
		else if (last_state == 4)
		{
			judge_speed_result.main_speed_set_1 = FOURTH_MIDDLE_CURVESPEED_LOW;
			judge_speed_result.main_speed_set_2 = FOURTH_MIDDLE_CURVESPEED_HIGH;
			last_state = 4; //4_middle_left
		}
		else if (last_state == 5)
		{
			judge_speed_result.main_speed_set_1 = FIFTH_LARGE_CURVESPEED_LOW;
			judge_speed_result.main_speed_set_2 = FIFTH_LARGE_CURVESPEED_HIGH;
			last_state = 5; //5_mlarge_left
		}
		else if (last_state == 6)
		{
			judge_speed_result.main_speed_set_1 = SIXTH_LARGE_CURVESPEED_LOW;
			judge_speed_result.main_speed_set_2 = SIXTH_LARGE_CURVESPEED_HIGH;
			last_state = 6; //6_mlarge_left
		}
		else if (last_state == 7)
		{
			judge_speed_result.main_speed_set_1 = FIRST_LITTLE_CURVESPEED_HIGH;
			judge_speed_result.main_speed_set_2 = FIRST_LITTLE_CURVESPEED_LOW;
			last_state = 7; //1_little_right
		}
		else if (last_state == 8)
		{
			judge_speed_result.main_speed_set_1 = SECOND_LITTLE_CURVESPEED_HIGH;
			judge_speed_result.main_speed_set_2 = SECOND_LITTLE_CURVESPEED_LOW;
			last_state = 8; //2_little_right
		}
		else if (last_state == 9)
		{
			judge_speed_result.main_speed_set_1 = THIRD_MIDDLE_CURVESPEED_HIGH;
			judge_speed_result.main_speed_set_2 = THIRD_MIDDLE_CURVESPEED_LOW;
			last_state = 9; //3_middle_right
		}
		else if (last_state == 10)
		{
			judge_speed_result.main_speed_set_1 = FOURTH_MIDDLE_CURVESPEED_HIGH;
			judge_speed_result.main_speed_set_2 = FOURTH_MIDDLE_CURVESPEED_LOW;
			last_state = 10; //4_middle_right
		}
		else if (last_state == 11)
		{
			judge_speed_result.main_speed_set_1 = FIFTH_LARGE_CURVESPEED_HIGH;
			judge_speed_result.main_speed_set_2 = FIFTH_LARGE_CURVESPEED_LOW;
			last_state = 11; //5_mlarge_right
		}
		else if (last_state == 12)
		{
			judge_speed_result.main_speed_set_1 = SIXTH_LARGE_CURVESPEED_HIGH;
			judge_speed_result.main_speed_set_2 = SIXTH_LARGE_CURVESPEED_LOW;
			last_state = 12; //6_mlarge_right
		}
		else if (last_state == 13)
		{
			judge_speed_result.main_speed_set_1 = CROSSSPEED;
			judge_speed_result.main_speed_set_2 = CROSSSPEED;
			last_state = 13; //cross
		}
		else if (last_state == 14)
		{
			judge_speed_result.main_speed_set_1 = UNKNOWNSPEED;
			judge_speed_result.main_speed_set_2 = UNKNOWNSPEED;
			last_state = 14; //unknown
		}
		else
		{
			judge_speed_result.main_speed_set_1 = UNKNOWNSPEED;
			judge_speed_result.main_speed_set_2 = UNKNOWNSPEED;
			last_state = 14; //unknown
		}
	}

	return judge_speed_result;
}

void Init_Sensor()
{
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	;

	//PTC10 sensor1 (left)
	//PTC11 sensor2
	//PTC12 sensor3
	//PTC13 sensor4
	//PTC16 sensor5 (right)
	//PTC17(most left)
	//PTA16(most right)
	PORTC->PCR[10] |= PORT_PCR_MUX(1);
	PTC->PDDR &= ~MASK(10);
	PORTC->PCR[11] |= PORT_PCR_MUX(1);
	PTC->PDDR &= ~MASK(11);
	PORTC->PCR[12] |= PORT_PCR_MUX(1);
	PTC->PDDR &= ~MASK(12);
	PORTC->PCR[13] |= PORT_PCR_MUX(1);
	PTC->PDDR &= ~MASK(13);
	PORTC->PCR[16] |= PORT_PCR_MUX(1);
	PTC->PDDR &= ~MASK(16);
	PORTC->PCR[17] |= PORT_PCR_MUX(1);
	PTC->PDDR &= ~MASK(17);
	PORTA->PCR[16] |= PORT_PCR_MUX(1);
	PTA->PDDR &= ~MASK(16);
}

#endif