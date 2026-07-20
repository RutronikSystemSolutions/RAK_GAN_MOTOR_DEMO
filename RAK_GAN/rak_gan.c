/******************************************************************************
 * @file rak_gan.c
 * @brief Board-specific helper functions for the RAK-GaN motor-control design.
 *
 * This module provides power-input handling, OCD threshold/fault processing,
 * temperature conversion, and status/fault LED updates.
 *****************************************************************************/

 /*Changes in ParamConfig.h*/



#include "rak_gan.h"
#include "cy_pdl.h"
#include "cycfg.h"
#include "HardwareIface.h"
#include "MCU.h"
#include "ParamConfig.h"



/**
 * @brief Enable power input and wait until rail and gate path are stable.
 */
void rak_gan_enable_power_input(void)
{
	Cy_GPIO_Clr(POW_EN_PORT, POW_EN_NUM);
	/* Warten, bis sich die Versorgungsspannung stabilisiert hat. */
	CyDelay(1000);
	/* Eingang wieder auf High setzen und die Reststabilisierung abwarten. */
	Cy_GPIO_Set(POW_EN_PORT, POW_EN_NUM);
	CyDelay(100);
}

/**
 * @brief Convert OCD current threshold in ampere to PWM compare value.
 *
 * @param i_limit Current threshold in ampere.
 * @return Compare value for the THPWM channel.
 */
uint32_t OCD_Current_to_PWM(float i_limit)
{
	uint32_t pwr_thpwm = OCD_TH_RES;
	float v_th = 0;

	v_th = 0.0386 * i_limit * OCD_TH_CAL - 0.0722;
	pwr_thpwm = (uint32_t)(v_th / 3.3 * OCD_TH_RES);

	return pwr_thpwm;
}


//NTC Settings
// ADC reading to temperature lookup table (temperatures in Celsius for ntcg103jf103ft1s)
const int16_t ntc_adc_table[NTC_TABLE_SIZE] = {
	3890, 	// -40°C
	3831, 	// -35°C
	3758, 	// -30°C
	3672, 	// -25°C
	3570, 	// -20°C
	3451, 	// -15°C
	3315, 	// -10°C
	3164, 	// -5°C
	2997, 	// 0°C
	2819, 	// 5°C
	2631, 	// 10°C
	2438, 	// 15°C
	2242, 	// 20°C
	2048, 	// 25°C
	1859, 	// 30°C
	1678, 	// 35°C
	1508, 	// 40°C
	1349, 	// 45°C
	1203, 	// 50°C
	1070, 	// 55°C
	950, 	// 60°C
	842, 	// 65°C
	746, 	// 70°C
	661, 	// 75°C
	586, 	// 80°C
	519, 	// 85°C
	461, 	// 90°C
	409, 	// 95°C
	364, 	// 100°C
	324, 	// 105°C
	289, 	// 110°C
	259, 	// 115°C
	231, 	// 120°C
	208, 	// 125°C
	187, 	// 130°C
	168, 	// 135°C
	152, 	// 140°C
	137, 	// 145°C
	124 	// 150°C
};

const float ntc_temp_table[NTC_TABLE_SIZE] = {
    -40.00f,  // -40.00°C
    -35.00f,  // -35.00°C
    -30.00f,  // -30.00°C
    -25.00f,  // -25.00°C
    -20.00f,  // -20.00°C
    -15.00f,  // -15.00°C
    -10.00f,  // -10.00°C
    -5.00f,   // -5.00°C
    0.00f,    // 0.00°C
    5.00f,    // 5.00°C
    10.00f,  // 10.00°C
    15.00f,  // 15.00°C
    20.00f,  // 20.00°C
    25.00f,  // 25.00°C
    30.00f,  // 30.00°C
    35.00f,  // 35.00°C
    40.00f,  // 40.00°C
    45.00f,  // 45.00°C
    50.00f,  // 50.00°C
    55.00f,  // 55.00°C
    60.00f,  // 60.00°C
    65.00f,  // 65.00°C
    70.00f,  // 70.00°C
    75.00f,  // 75.00°C
    80.00f,  // 80.00°C
    85.00f,  // 85.00°C
    90.00f,  // 90.00°C
    95.00f,  // 95.00°C
    100.00f, // 100.00°C
    105.00f, // 105.00°C
    110.00f, // 110.00°C
    115.00f, // 115.00°C
    120.00f, // 120.00°C
    125.00f, // 125.00°C
    130.00f, // 130.00°C
    135.00f, // 135.00°C
    140.00f, // 140.00°C
    145.00f, // 145.00°C
    150.00f  // 150.00°C
};

/**
 * @brief Calculate MCU temperature from active sensor or passive NTC input.
 *
 * @return Temperature in degree Celsius.
 */
float rak_gan_MCU_TempSensorCalc(void)
{
    // Local result holder
    float result = 0.0f;

#if (ACTIVE_TEMP_SENSOR)  // Active IC
    // Active temperature sensor path
    // result = (adc_scale.temp_ps * SAR_read) - (TEMP_SENSOR_OFFSET / TEMP_SENSOR_SCALE)
    {
        uint16_t sar_result = (uint16_t)vres_0_motor_0_T_POWER_get_result();
        float scale       = mcu[0].adc_scale.temp_ps;
        result = (scale * sar_result) - (TEMP_SENSOR_OFFSET / TEMP_SENSOR_SCALE);
    }

#else  // Passive NTC ntcg103jf103ft1s
    // Passive NTC path using LUT
    {
		uint16_t adc_reading = (uint16_t)mcu[0].dma_results[ADC_TEMP];
    	// Check bounds
    	if (adc_reading >= ntc_adc_table[0]) 
		{
        	return ntc_temp_table[0];
    	}

    	if (adc_reading <= ntc_adc_table[NTC_TABLE_SIZE - 1]) 
		{
        	return ntc_temp_table[NTC_TABLE_SIZE - 1];
    	}
		
    	// Binary search for the right interval
    	int left = 0;
    	int right = NTC_TABLE_SIZE - 1;

    	while (right - left > 1) 
		{
        	int mid = (left + right) / 2;
        	if (ntc_adc_table[mid] < adc_reading) 
			{
            	right = mid;
        } else
			{
            	left = mid;
        	}
    	}

    	// Linear interpolation between two points
    	float adc_diff = ntc_adc_table[right] - ntc_adc_table[left];
    	float temp_diff = ntc_temp_table[right] - ntc_temp_table[left];
    	float adc_offset = adc_reading - ntc_adc_table[left];

    	result = ntc_temp_table[left] + (temp_diff * adc_offset / adc_diff);
    }
#endif

    return result;
}

/* Prueft den OCD-Pin (Over-Current Detection) mit doppelter Bestaetigung
 * gegen kurze Glitches und liefert den aktuellen Fault-Zustand zurück
 * (aktiv-Low: 0 = Fehler).
 * return true = Fehler aktiv, false = kein Fehler */
bool rak_gan_is_ocd_fault_active(void)
{
    /* Ersten Messwert lesen (PWR_OCD ist aktiv-Low: 0 = Ueberstrom). */
    uint32_t ocd = Cy_GPIO_Read(PWR_OCD_PORT, PWR_OCD_NUM);

    if (!ocd)
    {
        /* Zweite Messung zur Gltich-Unterdrueckung. */
        ocd = Cy_GPIO_Read(PWR_OCD_PORT, PWR_OCD_NUM);

		return !ocd;
    }

	return false;
}

/**
 * @brief Retry power-input enable after a cleared undervoltage fault.
 *
 * The function memorizes that an undervoltage fault occurred. After the GUI
 * clears the latched `uv_vdc` flag, it briefly toggles `POW_EN` to reset the
 * input protection path and allow the supply to come back up.
 */
void rak_gan_undervoltage_fault_check(void)
{
	/*Variable for undervoltage fault clearing */
	static _Bool uv_fault = false;
	
	/*Detect the undervoltage event first, presume that this came from 7th switch off*/
	if((bool)(motor[0].faults_ptr->flags_latched.sw.uv_vdc) && !uv_fault)
	{
		uv_fault = true;
	}
	/*Try to clear the undervoltage fault if requested from GUI*/
	if(!(bool)(motor[0].faults_ptr->flags_latched.sw.uv_vdc) && uv_fault)
	{
		/* Reset the power input protection*/
		Cy_GPIO_Clr(POW_EN_PORT, POW_EN_NUM);
		CyDelay(1);
    	Cy_GPIO_Set(POW_EN_PORT, POW_EN_NUM);
		uv_fault = false;
	}
}

/**
 * @brief Initialize and start RGB LED PWM channels and OCD threshold PWM.
 */
void rak_gan_init_led_and_ocd_pwm(void)
{
#if defined(PWM_LED_RED_HW) && defined(PWM_LED_RED_NUM) && \
	defined(PWM_LED_GREEN_HW) && defined(PWM_LED_GREEN_NUM) && \
	defined(PWM_LED_BLUE_HW) && defined(PWM_LED_BLUE_NUM) && \
	defined(THPWM_PWM_HW) && defined(THPWM_PWM_NUM)
	cy_rslt_t result;

	/* Initializes the timer used for status LED. */
	result = Cy_TCPWM_PWM_Init(PWM_LED_RED_HW, PWM_LED_RED_NUM, &PWM_LED_RED_config);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initializes the timer used for status LED. */
	result = Cy_TCPWM_PWM_Init(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, &PWM_LED_GREEN_config);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Initializes the timer used for status LED. */
	result = Cy_TCPWM_PWM_Init(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, &PWM_LED_BLUE_config);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Enable LED PWMs now */
	Cy_TCPWM_PWM_Enable(PWM_LED_RED_HW, PWM_LED_RED_NUM);
	Cy_TCPWM_PWM_Enable(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM);
	Cy_TCPWM_PWM_Enable(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM);

	/* Starts the TCPWM for LEDs. */
	Cy_TCPWM_TriggerStart_Single(PWM_LED_RED_HW, PWM_LED_RED_NUM);
	Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_RED_HW, PWM_LED_RED_NUM, CLR_LED);
	Cy_TCPWM_TriggerStart_Single(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM);
	Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, CLR_LED);
	Cy_TCPWM_TriggerStart_Single(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM);
	Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);

	/* Initializes the timer used for OCD Threshold output control. */
	result = Cy_TCPWM_PWM_Init(THPWM_PWM_HW, THPWM_PWM_NUM, &THPWM_PWM_config);
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}
	Cy_TCPWM_PWM_Enable(THPWM_PWM_HW, THPWM_PWM_NUM);

	/* Starts the TCPWM and set the OCD Threshold */
	Cy_TCPWM_TriggerStart_Single(THPWM_PWM_HW, THPWM_PWM_NUM);
	Cy_TCPWM_PWM_SetCompare0Val(THPWM_PWM_HW, THPWM_PWM_NUM, OCD_Current_to_PWM(OCD_I_AMP));

	CyDelay(50 /*Wait for output voltage to settle*/);
#endif
}

/**
 * @brief Drive the fault LED output from current latched fault state.
 */
void rak_gan_update_fault_led(void)
{
#if defined(PWM_LED_RED_HW) && defined(PWM_LED_RED_NUM) && \
    defined(PWM_LED_GREEN_HW) && defined(PWM_LED_GREEN_NUM) && \
    defined(PWM_LED_BLUE_HW) && defined(PWM_LED_BLUE_NUM)
	if ((bool)(motor[0].faults_ptr->flags_latched.all))
	{
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_RED_HW, PWM_LED_RED_NUM, FAULT_LED_LVL);
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, CLR_LED);
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);
	}
	else
	{
		Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_RED_HW, PWM_LED_RED_NUM, CLR_LED);
		//Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, CLR_LED);
		//Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);
	}
#endif
}


/**
 * @brief Drive green/blue status LEDs from enable and direction state.
 */
void rak_gan_update_status_leds(void)
{
#if defined(PWM_LED_GREEN_HW) && defined(PWM_LED_GREEN_NUM) && \
    defined(PWM_LED_BLUE_HW) && defined(PWM_LED_BLUE_NUM)
	if (!(bool)(motor[0].faults_ptr->flags_latched.all))
	{
		if ((motor[0].vars_ptr->en) == 1)
		{
			if ((motor[0].vars_ptr->dir) == 1)
			{
				Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, SET_DIM_LED);
				Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);
			}
			else
			{
				Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, CLR_LED);
				Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, SET_DIM_LED);
			}
		}
		else
		{
			Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_GREEN_HW, PWM_LED_GREEN_NUM, CLR_LED);
			Cy_TCPWM_PWM_SetCompare0Val(PWM_LED_BLUE_HW, PWM_LED_BLUE_NUM, CLR_LED);
		}
	}
#endif
}