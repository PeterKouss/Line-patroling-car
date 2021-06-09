#include <MKL25Z4.H>
#include <stdio.h>
#include "MotorDrive.h"
#include "SensorDrive.h"

int main()
{
    SPEEDSET judge_speed_result;
		
    Init_MotorDrive();//init motor

		Init_Sensor();//Init sensor

    //main loop               
    while(1){
        judge_speed_result=Judge_Speed(); //judge speed by sensor
        Set_MotorSpeed(judge_speed_result.main_speed_set_1, judge_speed_result.main_speed_set_2);//PID control speed
    }
		return 0;
}