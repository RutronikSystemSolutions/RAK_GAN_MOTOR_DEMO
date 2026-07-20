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

/**
 * @brief Enable the RAK-GaN power input and wait for supply stabilization.
 */
void rak_gan_enable_power_input(void);

/**
 * @brief Convert an over-current threshold in ampere to THPWM compare value.
 *
 * @param i_limit Current limit in ampere.
 * @return PWM threshold compare value for OCD threshold generation.
 */
uint32_t OCD_Current_to_PWM(float i_limit);

/**
 * @brief Check whether the OCD input indicates an active over-current fault.
 *
 * The input is active-low. The implementation uses a second sample to reject
 * short glitches.
 *
 * @return true if fault is active, false otherwise.
 */
bool rak_gan_is_ocd_fault_active(void);

/**
 * @brief Retry power-input enable after a cleared undervoltage fault.
 *
 * Tracks whether an undervoltage fault was latched. Once the GUI-side fault
 * clear removes the latched `uv_vdc` flag, the function toggles `POW_EN` to
 * re-enable the protected input stage.
 */
void rak_gan_undervoltage_fault_check(void);

/**
 * @brief Initialize and start LED PWM channels and OCD threshold PWM channel.
 */
void rak_gan_init_led_and_ocd_pwm(void);

/**
 * @brief Update the fault LED state based on latched motor fault flags.
 */
void rak_gan_update_fault_led(void);

/**
 * @brief Update status LEDs according to motor enable and direction state.
 */
void rak_gan_update_status_leds(void);

/**
 * @brief Compute MCU temperature in degree Celsius from configured sensor input.
 *
 * Uses either an active temperature sensor transfer function or passive NTC
 * LUT interpolation depending on build-time configuration.
 *
 * @return Measured temperature in degree Celsius.
 */
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
