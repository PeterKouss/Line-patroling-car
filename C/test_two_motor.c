#include <MKL25Z4.H>
#include <stdio.h>
#include "PrintUart.h"
#include "IIRfilter.h"

#define MASK(x) (1UL << (x))

#define PWM_MAX_COUNT (6000)       //6000 means 500Hz zhou qi
#define READ_PWM_MAX_COUNT (24000) //6000 means 500Hz zhou qi

#define BUS_CLOCK_FREQUENCY (DEFAULT_SYSTEM_CLOCK)
#define TICK_FREQUENCY (1)

#define PWM_CYCLE (50) //zhan kong bi

#define SAMPLE_NUMBER 1000

int count_overflow_1 = 0;
int count_overflow_2 = 0;
int count_number_1[2];
int count_number_2[2];

float speed_buffer_1[IIR_ODER];
float speed_result_1[IIR_ODER];
float speed_buffer_2[IIR_ODER];
float speed_result_2[IIR_ODER];

int sampling_finished_flag_1 = 0;
int sampling_finished_flag_2 = 0;
float speed_sample_1[SAMPLE_NUMBER];
int pwm_control_1[SAMPLE_NUMBER];
float speed_sample_2[SAMPLE_NUMBER];
int pwm_control_2[SAMPLE_NUMBER];

float speed_pre_set = 0.5;
float k_1=19.87;
float kp_1=35.4650;
float k_2=17.98;
float kp_2=44.91;

void Init_PMW()
{
    //turn on clock to TPM1
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;

    //set multiplexer to connect
    //TPM1 ch0 to  PTA12
    //TPM1 ch1 to  PTA13
    PORTA->PCR[12] |= PORT_PCR_MUX(3);
    PORTA->PCR[13] |= PORT_PCR_MUX(3);

    //set clock source for tpm
    //SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

    //load the counter and mod
    TPM1->MOD = PWM_MAX_COUNT;

    //set channels to center-aligned Low-true PWM
    TPM1->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
    TPM1->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;

    //set TPM to up-down and divide by 8 prescaler and clock mode
    TPM1->SC = (TPM_SC_CPWMS_MASK | TPM_SC_CMOD(1) | TPM_SC_PS(3));

    //set trigger mode
    //TPM1->CONF |= TPM_CONF_TRGSEL(8);

    TPM1->CONTROLS[0].CnV = PWM_MAX_COUNT / 2;
    TPM1->CONTROLS[1].CnV = PWM_MAX_COUNT / 2;
}

void Init_read_PMW()
{
    //turn on clock to TPM0
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    //set multiplexer to connect
    //TPM0 ch4 to  PTE31
    PORTE->PCR[31] |= PORT_PCR_MUX(3);

    //set clock source for tpm
    //SIM->SOPT2 |= (SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK);

    //load the counter and mod
    TPM0->MOD = READ_PWM_MAX_COUNT;

    //set channels to center-aligned Low-true PWM
    TPM0->CONTROLS[4].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_ELSA_MASK;

    //set TPM to up-down and divide by 8 prescaler and clock mode
    TPM0->SC = ((~TPM_SC_CPWMS_MASK & TPM_SC_CMOD(1)) | TPM_SC_PS(3));

    //set trigger mode
    //TPM1->CONF |= TPM_CONF_TRGSEL(8);

    //Enable IRQn
    NVIC_SetPriority(TPM0_IRQn, 128); // 0, 64, 128 or 192
    NVIC_ClearPendingIRQ(TPM0_IRQn);
    NVIC_EnableIRQ(TPM0_IRQn);

    //turn on clock to TPM2
    SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;

    //set multiplexer to connect
    //TPM2 ch0 to  PTE22
    PORTE->PCR[22] |= PORT_PCR_MUX(3);

    //load the counter and mod
    TPM2->MOD = READ_PWM_MAX_COUNT;

    //set channels to center-aligned Low-true PWM
    TPM2->CONTROLS[0].CnSC = TPM_CnSC_CHIE_MASK | TPM_CnSC_ELSA_MASK;

    //set TPM to up-down and divide by 8 prescaler and clock mode
    TPM2->SC = ((~TPM_SC_CPWMS_MASK & TPM_SC_CMOD(1)) | TPM_SC_PS(3));

    //Enable IRQn
    NVIC_SetPriority(TPM2_IRQn, 128); // 0, 64, 128 or 192
    NVIC_ClearPendingIRQ(TPM2_IRQn);
    NVIC_EnableIRQ(TPM2_IRQn);
}

void TPM0_IRQHandler(void)
{
    //clear pending IRQ
    NVIC_ClearPendingIRQ(TPM0_IRQn);

    // check to see which channel triggered interrupt
    if ((TPM0->SC & TPM_SC_TOF_MASK) != 0)
    {
        // clear status flag for timer channel 0
        TPM0->SC |= TPM_SC_TOF_MASK;

        // Do ISR work
        // overflow count
        if (sampling_finished_flag_1 == 0)
        {
            count_overflow_1++;
        }
    }
    else if ((TPM0->CONTROLS[4].CnSC & TPM_CnSC_CHF_MASK) != 0)
    {
        // read cnv
        TPM0->CONTROLS[4].CnSC |= TPM_CnSC_CHF_MASK;
        if (sampling_finished_flag_1 == 0)
        {
            count_number_1[0] = count_number_1[1];
            count_number_1[1] = TPM0->CONTROLS[4].CnV;
            sampling_finished_flag_1 = 1;
        }
    }
}

void TPM2_IRQHandler(void)
{
    //clear pending IRQ
    NVIC_ClearPendingIRQ(TPM2_IRQn);

    // check to see which channel triggered interrupt
    if ((TPM2->SC & TPM_SC_TOF_MASK) != 0)
    {
        // clear status flag for timer channel 0
        TPM2->SC |= TPM_SC_TOF_MASK;

        // Do ISR work
        // overflow count
        if (sampling_finished_flag_2 == 0)
        {
            count_overflow_2++;
        }
    }
    else if ((TPM2->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK) != 0)
    {
        // read cnv
        TPM2->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
        if (sampling_finished_flag_2 == 0)
        {
            count_number_2[0] = count_number_2[1];
            count_number_2[1] = TPM2->CONTROLS[0].CnV;
            sampling_finished_flag_2 = 1;
        }
    }
}

void Set_PWM_Value(uint8_t duty_cycle, uint8_t channel)
{
    uint16_t n;

    n = duty_cycle * PWM_MAX_COUNT / 100;
    if (channel == 0)
    {
        TPM1->CONTROLS[0].CnV = n;
    }
    else if (channel == 1)
    {
        TPM1->CONTROLS[1].CnV = n;
    }
}

void Init_GPIO(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    //PTB9 set as GPIO high

    PORTB->PCR[9] |= PORT_PCR_MUX(1);
    PTB->PDDR |= MASK(9);
    PTB->PSOR |= MASK(9);

    PORTB->PCR[8] |= PORT_PCR_MUX(1);
    PTB->PDDR |= MASK(8);
    PTB->PSOR |= MASK(8);
}

float Speed_Caculate(int which_wheel)
{
		/*
    float speed_now = 0;
    if (which_wheel == 1)
    {
        speed_now = (count_number_1[1] + (READ_PWM_MAX_COUNT - count_number_1[0]) + (count_overflow_1 - 1) * READ_PWM_MAX_COUNT);
    }
    else if (which_wheel == 2)
    {
        speed_now = (count_number_2[1] + (READ_PWM_MAX_COUNT - count_number_2[0]) + (count_overflow_2 - 1) * READ_PWM_MAX_COUNT);
    }
    return speed_now;
    */
		if (which_wheel == 1)
    {
        float speed_now = 0;
        speed_now = 1.0 / ((count_number_1[1] + (READ_PWM_MAX_COUNT - count_number_1[0]) + (count_overflow_1 - 1) * READ_PWM_MAX_COUNT) * 390 / 6000000.0);

        //filter
        for (int i = 0; i < IIR_ODER - 1; i++)
        {
            speed_buffer_1[i] = speed_buffer_1[i + 1];
        }
        speed_buffer_1[IIR_ODER - 1] = speed_now;

        for (int i = 0; i < IIR_ODER - 1; i++)
        {
            speed_result_1[i] = speed_result_1[i + 1];
        }
        speed_result_1[IIR_ODER - 1] = 0;

        for (int i = 0; i < IIR_ODER; i++)
        {
            speed_result_1[IIR_ODER - 1] += -IIR_A[i] * speed_result_1[IIR_ODER - 1 - i];
        }
        for (int i = 0; i < IIR_ODER; i++)
        {
            speed_result_1[IIR_ODER - 1] += IIR_B[i] * speed_buffer_1[IIR_ODER - 1 - i];
        }
        return speed_result_1[IIR_ODER - 1];
    }
    else if (which_wheel == 2)
    {
        float speed_now = 0;
        speed_now = 1.0 / ((count_number_2[1] + (READ_PWM_MAX_COUNT - count_number_2[0]) + (count_overflow_2 - 1) * READ_PWM_MAX_COUNT) * 390 / 6000000.0);

        //filter
        for (int i = 0; i < IIR_ODER - 1; i++)
        {
            speed_buffer_2[i] = speed_buffer_2[i + 1];
        }
        speed_buffer_2[IIR_ODER - 1] = speed_now;

        for (int i = 0; i < IIR_ODER - 1; i++)
        {
            speed_result_2[i] = speed_result_2[i + 1];
        }
        speed_result_2[IIR_ODER - 1] = 0;

        for (int i = 0; i < IIR_ODER; i++)
        {
            speed_result_2[IIR_ODER - 1] += -IIR_A[i] * speed_result_2[IIR_ODER - 1 - i];
        }
        for (int i = 0; i < IIR_ODER; i++)
        {
            speed_result_2[IIR_ODER - 1] += IIR_B[i] * speed_buffer_2[IIR_ODER - 1 - i];
        }
        return speed_result_2[IIR_ODER - 1];
    }
    
}

int main(void)
{
    __enable_irq();
    init_board();
    Init_PMW();
    Init_GPIO();
    while (TPM1->CONTROLS[0].CnV != PWM_CYCLE * PWM_MAX_COUNT / 100)
    {
        Set_PWM_Value(PWM_CYCLE, 0);
    }
    while (TPM1->CONTROLS[1].CnV != PWM_CYCLE * PWM_MAX_COUNT / 100)
    {
        Set_PWM_Value(PWM_CYCLE, 1);
    }
    Init_read_PMW();

    int pwm_set_1 = 50;
    int pwm_set_2 = 50;

    for (int i = 0; i < IIR_ODER; i++)
    {
        speed_buffer_1[i] = 0;
        speed_buffer_2[i] = 0;
    }

    for (int i = 0; i < SAMPLE_NUMBER; i++)
    {
			
        if (sampling_finished_flag_1 == 1)
        {
            speed_sample_1[i] = Speed_Caculate(1);
            pwm_control_1[i] = (k_1 * speed_pre_set) + (kp_1 * (speed_pre_set - speed_sample_1[i]));

            //pwm_control_1[i] = 50;

            if (pwm_control_1[i] > 99)
            {
                pwm_set_1 = 99;
            }
            else if (pwm_control_1[i] < 1)
            {
                pwm_set_1 = 1;
            }
            else
            {
                pwm_set_1 = pwm_control_1[i];
            }
            Set_PWM_Value(pwm_set_1, 0);
            count_overflow_1 = 0;
            sampling_finished_flag_1 = 0;
        }
        else 
				{
					i--;
				}
				/*
				if (sampling_finished_flag_2 == 1)
        {
            speed_sample_2[i] = Speed_Caculate(2);
            pwm_control_2[i] = (k_2 * speed_pre_set) + (kp_2 * (speed_pre_set - speed_sample_2[i]));
					
            //pwm_control_2[i] = 50;
					
            if (pwm_control_2[i] > 99)
            {
                pwm_set_2 = 99;
            }
            else if (pwm_control_2[i] < 1)
            {
                pwm_set_2 = 1;
            }
            else
            {
                pwm_set_2 = pwm_control_2[i];
            }
            Set_PWM_Value(pwm_set_2, 1);
            count_overflow_2 = 0;
            sampling_finished_flag_2 = 0;
        }
        else
        {
            i--;
        }
				*/       
    }
    while (TPM1->CONTROLS[0].CnV != 0 * PWM_MAX_COUNT / 100)
    {
        Set_PWM_Value(0, 0);
    }
    while (TPM1->CONTROLS[1].CnV != 0 * PWM_MAX_COUNT / 100)
    {
        Set_PWM_Value(0, 1);
    }
    for (int i = 0; i < SAMPLE_NUMBER; i++)
    {
        printf("%f,%d,%f,%d\n", speed_sample_1[i], pwm_control_1[i], speed_sample_2[i], pwm_control_2[i]);
    }
    while (1)
    {
        ;
    }
}