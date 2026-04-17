#ifndef RAK_GAN_H
#define RAK_GAN_H

#include <stdbool.h>
#include <stdint.h>

#define RAK_GAN_BOARD 1

#define NTC_TABLE_SIZE 39
/* Counters values for LED operation */
#define CLR_LED    		(0)
#define SET_FULL_LED    (1000)
#define SET_DIM_LED 	(100)
#define FAULT_LED_LVL 	(200)

#define OCD_TH_RES 	    (10000)
#define OCD_TH_CAL 	    (1.77f)
#define OCD_I_AMP 	    (10.0f) /*Power Input Current Limit*/

/* Schaltet den Leistungseingang ein und wartet auf stabile Versorgung. */
void rak_gan_enable_power_input(void);
uint32_t OCD_Current_to_PWM(float i_limit);
bool rak_gan_is_ocd_fault_active(void);
void rak_gan_init_led_and_ocd_pwm(void);
void rak_gan_update_fault_led(void);
void rak_gan_update_status_leds(void);
float rak_gan_MCU_TempSensorCalc(void);



/*

ADC/DMA mapping (RAK_GAN, Three_Shunt, MUXA)
10-slot schema is kept (5 samples per sequence, 2 DMA channels).
Unused slots are mapped to ADC_SAMP_UNUSED (dummy source).

DMA_ADC_0 -> SEQ0 (TRIG_0, high priority):

Desc0: ADC Ch10 (IU)    -> dma_results[ADC_ISAMPA]  (idx 0)
Desc1: ADC Ch12 (IW)    -> dma_results[ADC_ISAMPC]  (idx 1)
Desc2: ADC Ch8 (VPOT)   -> dma_results[ADC_VPOT]    (idx 7)
Desc3: Dummy (unused)   -> dma_results[ADC_VU]      (idx 8)
Desc4: Dummy (unused)   -> dma_results[ADC_VW]      (idx 9)

DMA_ADC_1 -> SEQ1 (TRIG_1, low priority):

Desc0: ADC Ch11 (IV) ->         dma_results[ADC_ISAMPB] (idx 5)
Desc1: ADC Ch1 (VBUS) ->        dma_results[ADC_VBUS]   (idx 2)
Desc2: ADC Ch5 (IDCLINKAVG) ->  dma_results[ADC_ISAMPD] (idx 6)
Desc3: ADC Ch16 (TEMP) ->       dma_results[ADC_TEMP]   (idx 3)
Desc4: Dummy (unused) ->        dma_results[ADC_VV]     (idx 4)
*/

#endif /* RAK_GAN_H */
