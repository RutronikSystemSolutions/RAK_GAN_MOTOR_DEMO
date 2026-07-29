/*******************************************************************************
* Copyright 2021-2024, Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
*
* This software, including source code, documentation and related
* materials ("Software") is owned by Cypress Semiconductor Corporation
* or one of its affiliates ("Cypress") and is protected by and subject to
* worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software ("EULA").
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software
* source code solely for use in connection with Cypress's
* integrated circuit products.  Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
* reserves the right to make changes to the Software without notice. Cypress
* does not assume any liability arising out of the application or use of the
* Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death ("High Risk Product"). By
* including Cypress's product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*******************************************************************************/
#pragma once

#include "MotorCtrlHWConfig.h"
#include "Controller.h"


/*Parameter Configuration for Motor 0*/
#define PARAMS_ALWAYS_OVERWRITE     (false)     // For testing only. Using this will ensure that parameters are always overwritten. (default: false)
/*******************************************************************************/
/******************Parameter Configuration for Motor 0**************************/
/*******************************************************************************/
/*Power Board configuration*/
#define ADC_VREF_GAIN                            ((3.3f)/(3.3f))               /*[V/V], voltage-reference buffer gain (e.g. scaling 5.0V down to 3.3V) (default: (5.0f)/(3.3f))*/

#define ADC_CS_CURRENT_MEASUREMENT_TYPE          (Active_Sensor)                   /*Shunt_resistance "Shunt_res" =0 or active sensor "Active_Sensor" =1 (default: Shunt_Res)*/
#define ADC_CS_CURRENT_SENSE_POLARITY            (LS_Current_Sense)            /*Low side current sense "LS_Current_Sense" =0 or High side current Sense "HS_Current_sense"=1 (default: LS_Current_Sense)*/

#define ADC_CS_SHUNT_TYPE                        (Three_Shunt)                 /*Three_Shunt =0, Single_Shunt =1, Two_Shunt =2 (default: Three_Shunt)*/

#define ADC_CS_SHUNT_RES                         (0.01)                    /*[Ohm], Applicable for Current shunt type of measurement (default: 10.0E-3f)*/

#define ADC_CS_CURRENT_SENSITIVITY               (0.022)                    /*[V/A], Applicable for active current sense type of measurement (default: 10.0E-3f)*/

#define ADC_CS_OPAMP_GAIN                        (1)                       /*[V/V], external amplifier gain for current measurement (default: 12.0f)*/

#define ADC_CS_SETTLE_RATIO                      (0.8f)                        /*[], settling ratio used for single-shunt current sampling (default: 0.8f)*/
#define ADC_CS_SS_MIN_SEGMENT_TIME               (3.0E-6f)                     /*[sec], Current measurement single shunt minimum measurable window (default: 3.0E-6f)*/

#define ADC_SCALE_VUVW                           (1)         /*[V/V] = [Ohm/Ohm] (default: (5.6f)/(56.0f+5.6f))*/
#define ADC_SCALE_VDC                            (0.0339)         /*[V/V] = [Ohm/Ohm] (default: (5.6f)/(56.0f+5.6f))*/
/*******************************************************************************/
/*******************************************************************************/
/*Parameter Controls*/
/***MCU***/
/*****System*****/
#define MOTOR_CTRL_BOOTSTRAP_TIME                  (0.25)                    /*[sec], Bootstrap capacitor charge time (default: 0.25f)*/ 
#define MOTOR_CTRL_BOOTSTRAP_MODE                  (Boot_and_Brake)            /*[], Boot_and_Brake = 0U, Boot_Only =1U (default: Boot_and_Brake)*/ 

/*******Sampling*******/
#define MOTOR_CTRL_FPWM_FS0_RATIO                  (1)                        /*[], PWM to fast-loop frequency ratio (default: 1U)*/
#define MOTOR_CTRL_FASTLOOP_FREQ                   (20000)                  /*[Hz], fast-loop frequency at least 1.2 decade above maximum electrical frequency, 1.5kHz~>25kHz (default: 15000.0f)*/
#define MOTOR_CTRL_FS0_FS1_RATIO                   (5U)                        /*[], Fast-loop to slow-loop frequency ratio (default: 5U)*/

#define MOTOR_CTRL_PWM_DEADTIME                    (1e-7)                   /*[sec], PWM Deadtime value (default: 0.5E-6f)*/

/*******Analog Sensors*******/
/*********Shunts*********/
/*Note:Shunt resistance and current amplifier gain value configuration at Power Board configuration*/
#define MOTOR_CTRL_SS_HMOD_KI                      (0.5)    /* (default: 0.5f) */
#define MOTOR_CTRL_SS_PS_SH_DELAY                  (0)                   /* [sec], Switch delay configuration for ADC measurement for Phase shift modulation (default: 0.0E-6f)*/


#define MOTOR_CTRL_SS_MEAS_TYPE                   (Phase_Shift)                /*   Hyb_Mod =  0U, Hybrid modulation; Phase_Shift = 1U Phase Shift modulation (default: Phase_Shift)*/  

/*********Rate Limiters*********/
#define MOTOR_CTRL_SPEED_CMD_RATE                  (4000)                    /*[RPM/sec], Speed command rate (default: 1000.0f)*/
#define MOTOR_CTRL_SPEED_CMD_RATE_OPEN_LOOP        (1000)                    /*[RPM/sec], Speed command rate during open loop state (Voltage OL and current OL) (default: 1000.0f)*/
#if defined(CTRL_METHOD_RFO) || defined(CTRL_METHOD_TBC)
#define MOTOR_CTRL_CURRENT_CMD_RATE                (54)   /*[A/sec], Current command rate (default: 10.0f*MOTOR_CURRENT_PEAK)*/
#elif defined(CTRL_METHOD_SFO)
#define MOTOR_CTRL_TORQUE_CMD_RATE                 (10.0f*MOTOR_TORQUE_MAX)     /*[Nm/sec], Torque command rate (default: 10.0f*MOTOR_TORQUE_MAX)*/
#endif

#if defined(CTRL_METHOD_RFO)
#define MOTOR_CTRL_POSITION_CMD_RATE               (180)                     /*[Deg/sec], Position command rate (default: 180.0f)*/
#endif
/*********Faults*********/
#define MOTOR_CTRL_OVER_CURRENT_THRESH             (120)                     /*[%], over current fault threshold, percentage of motor continuous current (default: 120.0f)*/
#define MOTOR_CTRL_VDC_DEBOUNCE_TIME               (0.3)                      /*[sec], VDC fault detection debouncing time (default: 0.3f)*/
#define MOTOR_CTRL_OVER_TEMP_THRESH                (90)                      /*[Celsius], over temperature fault threshold (default: 90.0f)*/

#define MOTOR_CTLR_FAULT_PHASE_LOSS_TIME           (1)                      /*[sec],phase loss time detection constant (default: 1.0f)*/
#define MOTOR_CTLR_FAULT_PHASE_LOSS_MIN_CURRENT    (0.001)                    /*[A],phase loss detection minimum current threshold (default: 0.001f)*/

#define MOTOR_CTRL_FAULT_SHORT_METHOD              (Alternate_Short)            /*During fault switch status, Low_Side_Short = 0U, High_Side_Short = 1U, Alternate_Short = 2U (default: Alternate_Short)*/
#define MOTOR_CTRL_MAX_FAULT_CLR_TRIES             (10)                        /*[], maximum number of fault clear tries (default: 10U)*/  
#define MOTOR_CTRL_FAULT_CLR_TRY_PERIOD            (10)                      /*[sec], Fault clear retry period (default: 10.0f)*/  
/*********Command*********/
#define MOTOR_CTRL_COMMAND_SOURCE                  (External)                   /*Internal= 0(From potentiometer), External =1(From GUI, UART, etc.) (default: Internal)*/

#define MOTOR_CTRL_COMMAND_MAX_SPEED               (MOTOR_NORM_SPEED)           /*[RPM], maximum speed command (default: MOTOR_NORM_SPEED)*/
#if defined(CTRL_METHOD_RFO) || defined(CTRL_METHOD_TBC)
#define MOTOR_CTRL_COMMAND_MAX_CURRENT             (0.895)    /*[A], maximum current command (default: MOTOR_CURRENT_CONT*0.5f)*/
#elif defined(CTRL_METHOD_SFO)
#define MOTOR_CTRL_COMMAND_MAX_TORQUE              (MOTOR_TORQUE_MAX*0.5f)      /*[Nm], Maximum torque command (default: MOTOR_TORQUE_MAX*0.5f)*/
#endif
#if defined(CTRL_METHOD_RFO)
#define MOTOR_CTRL_COMMAND_MAX_POSITION             (360.f)                    /*[deg], maximum current command (default: 360.f)*/
#endif
/*******Feedback*******/
/*********Hall Sensor*********/
#define MOTOR_CTRL_HALL_ANGLE_OFFSET               (0)                      /*[deg], Hall angle offset (default: 0.0f)*/
#define MOTOR_CTRL_HALL_ANGLE_OFFSET_COMP          (Dis)                       /*Dis =0, En=1, Enable/disable offset compensation (default: Dis)*/

/*********Incremental Encoder*********/
#define MOTOR_CTRL_ENCODER_CPR                     (16384)                    /*[], Edges per revolution (default: 16384U)*/
#define MOTOR_CTRL_ENCODER_TIME_CONST              (1.5)                      /*[sec], Time constant ratio (default: 1.5f)*/
#define MOTOR_CTRL_ENCODER_ZERO_SPEED_THRESH       (32)   /*[RPM], Zero speed detection threshold (default: MOTOR_NORM_SPEED*0.008f)*/

/*******************************************************************************/
/*******Observer*******/
#define MOTOR_CTRL_OBS_SPEED_THRESH                (480)      /*[RPM], Observer activation speed threshold (default: MOTOR_NORM_SPEED*0.2f)*/
#define MOTOR_CTRL_OBS_MIN_LOCK_TIME               (0.5)                       /*[sec], Observer minimum lock time (default: 0.5f)*/
#define MOTOR_CTRL_OBS_MAX_SPEED                   (9000)      /*[RPM], Observer PLL maximum speed limit (default: MOTOR_MAX_SPEED*1.5f)*/
/*******************************************************************************/
/*******Filter*******/
#define MOTOR_CTRL_TORQUE_FILT_BW                  (1)                        /*[Hz], Estimated torque filter bandwidth (default: 1.0f)*/

/*********SPEED Anti_Resonant Filter*********/
#define MOTOR_CTRL_SPEED_AR_FILTER                 (En)                          /*Dis =0, En=1, Enable/disable speed anti-resonant filter (default: En)*/

/*******************************************************************************/
/*******Control*******/
#if defined(CTRL_METHOD_RFO) || defined(CTRL_METHOD_SFO)
#define MOTOR_CTRL_CTRL_MODE                       (Speed_Mode_FOC_Sensorless_Curr_Startup)/*Control mode (default: Speed_Mode_FOC_Sensorless_Volt_Startup)*/
#elif defined(CTRL_METHOD_TBC)
#define MOTOR_CTRL_CTRL_MODE                       (Speed_Mode_FOC_Sensorless_Curr_Startup)  /*Control mode (default: Speed_Mode_Block_Comm_Hall)*/
#endif

/*********SPEED Controller*********/
#define MOTOR_CTRL_SPEED_BW                        (50)                      /*[Hz], Speed loop bandwidth (default: 5.0f)*/ //(15.0f)
#define MOTOR_CTRL_SPEED_OL_CL_TR_COEFF            (25)                     /*[%], Open-loop to closed-loop transition coefficient (default: 100.0f)*/
#define MOTOR_CTRL_SPEED_KI_MULTIPLE               (10)                      /*[], Ki multiple for speed loop (default: 10.0f)*/
#define MOTOR_CTRL_SPEED_FF_COEFF                  (60)                       /*[%], Speed loop feed-forward coefficient (default: 100.0f)*/ 

#if defined(CTRL_METHOD_TBC)    
#define MOTOR_CTRL_CURRENT_BYPASS                   (false)                     /* Current Control bypass switch (default: false)*/
#endif

#if defined(CTRL_METHOD_RFO) || defined(CTRL_METHOD_TBC)
/*********Current Controller*********/
#define MOTOR_CTRL_CURRENT_BW                      (6000)                      /*[Hz], Current loop bandwidth (default: 750.0f)*/
#define MOTOR_CTRL_CURRENT_FF_COEFF                (50)                      /*[%], Current loop feed-forward coefficient (default: 100.0f)*/
#define MOTOR_CTRL_CURRENT_STARTUP_THRESH          (0.179)    /*[A], Current control startup threshold (default: MOTOR_CURRENT_CONT*0.15f)*/
#define MOTOR_CTRL_CURRENT_OPEN_LOOP_CMD           (2)      /*[A], Current control open loop command value (default: MOTOR_CURRENT_CONT*0.4f)*/

#elif defined(CTRL_METHOD_SFO)
/*********Torque Controller*********/
#define MOTOR_CTRL_TORQUE_BW                       (150.0f)                      /*[Hz], Torque controller bandwidth (default: 150.0f)*/
#define MOTOR_CTRL_POLE_ZERO_RATIO                 (25.0f)                       /*[%], Pole to zero ratio (default: 25.0f)*/
#define MOTOR_CTRL_MAX_LOAD_ANGLE                  (120.0f)                      /*[Deg], Maximum load angle (default: 120.0f)*/
#define MOTOR_CTRL_STARTUP_THRESH                  (MOTOR_TORQUE_MAX*0.2f)                    /*[Nm], Startup threshold (default: MOTOR_TORQUE_MAX*0.2f)*/
#define MOTOR_CTRL_STARTUP_THRESH_HYS              (MOTOR_CTRL_STARTUP_THRESH*0.9f)           /*[Nm], Startup threshold hysteresis (default: MOTOR_CTRL_STARTUP_THRESH*0.9f)*/
#define MOTOR_CTRL_TIME_SM_CURRENT                 (0.04f)                       /*[sec], Reach time for sliding-mode current limiter (default: 0.04f)*/

/*********Flux Controller*********/
#define MOTOR_CTRL_FLUX_BW                         (300.0f)                      /*[Hz], Flux loop bandwidth (default: 300.0f)*/
#define MOTOR_CTRL_FLUX_POLE_SEP_RATIO             (1.1f)                        /*[#], Pole separation ratio (default: 1.1f)*/

/*********Load Angle Controller*********/
#define MOTOR_CTRL_LOAD_ANGLE_BW                   (750.0f)                      /*[Hz], Load angle bandwidth (default: 750.0f)*/
#define MOTOR_CTRL_BW_MUL                          (1.5f)                        /*[], Bandwidth multiplier (default: 1.5f)*/
#define MOTOR_CTRL_BW_MUL_LS_THRESH                (150.0f)                      /*[Hz], Bandwidth multiplier low speed threshold (default: 150.0f)*/
#define MOTOR_CTRL_BW_MUL_HS_THRESH                (300.0f)                      /*[Hz], Bandwidth multiplier high speed threshold (default: 300.0f)*/
#define MOTOR_CTRL_LOAD_POLE_SEP_RATIO             (1.01f)                       /*[], Pole separation ration (default: 1.01f)*/
#endif

#if defined(CTRL_METHOD_RFO)
#define MOTOR_CTRL_POSITION_BW                     (5)                       /*[Hz], Position loop bandwidth (default: 5.0f)*/  
#define MOTOR_CTRL_POSITION_POLE_SEP_RATIO         (1.001)                     /*[#], Pole separation ration (default: 1.001f)*/
#define MOTOR_CTRL_POSITION_FF_COEFF               (100)                    /*[%], Position loop feed-forward coefficient (default: 100.0f)*/ 
#define MOTOR_CTRL_POSITION_PI_LIMIT               (1000)                   /*[RPM], Position PI  output limit (default: 1000.0f)*/
#endif

/*********Voltage Controller*********/
#define MOTOR_CTRL_VOLT_STARTUP_THRESH             (200)      /*[RPM], startup threshold (default: MOTOR_NORM_SPEED*0.05f)*/
#define MOTOR_CTRL_VOLT_VF_OFFSET                  (0.15)                       /*[V], V/F ramp voltage offset (default: 0.15f)*/
#define MOTOR_CTRL_VOLT_VF_RATIO                   (0.0075)                     /*[V/(Rad/sec], V/f ramp slope (default: 7.5E-3f)*/

#define MOTOR_CTRL_VOLT_MOD_SCHEME                 (Space_Vector_Modulation)    /*Neutral_Point_Modulation =0,Space_Vector_Modulation =1 (default: Neutral_Point_Modulation)*/

/***********Five Segment Modulation***********/
#define MOTOR_CTRL_FIVE_SEG_MOD                    (Dis)                         /*Dis =0, En=1, Enable/disable five segment modulation (default: Dis)*/
#define MOTOR_CTRL_FIVE_SEG_MOD_ACT_THRESH         (10)                       /*[%], Five segment modulation activation threshold (default: 10.0f)*/
#define MOTOR_CTRL_FIVE_SEG_MOD_INACT_THRESH       (5)                        /*[%], Five segment modulation deactivation threshold (default: 5.0f)*/

#if defined(CTRL_METHOD_TBC)
/***********Trapezoidal Commutation***********/
#define MOTOR_CTRL_TBC_MODE                        (Block_Commutation)           /*Block_Commutation =0, Trapezoidal_Commutation =1 (default: Block_Commutation)*/
#define MOTOR_CTRL_TBC_TRAP_RAMP_COUNT             (5U)                          /*[#], Current ramp's sample count (default: 5U)*/
#define MOTOR_CTRL_TBC_TRAP_RAMP_BW_RATIO          (0.80f)                       /*[#], Ramp to main controller bandwidth ratio (default: 0.80f)*/
#define MOTOR_CTRL_TBC_TRAP_RAMP_FF_COEF           (4.0f)                        /*[V/A], Ramp feed-forware coefficient (default: 4.0f)*/
#define MOTOR_CTRL_TBC_TRAP_MAIN_FF_COEF           (1.0f)                        /*[#], Main feed-formware coefficient (default: 1.0f)*/
#endif

/*********Flux Weakening*********/
#if defined(CTRL_METHOD_RFO) || defined(CTRL_METHOD_SFO)
#define MOTOR_CTRL_FLUX_WEAKEN                     (En)                          /*Dis =0, En=1, Enable/disable flux weakening (default: En)*/
#define MOTOR_CTRL_FLUX_WEAKEN_VOLT_MARGIN         (0.8)                        /*[Hz], Flux weakening voltage margin (default: 0.8f)*/
#if defined(CTRL_METHOD_RFO)
#define MOTOR_CTRL_FLUX_WEAKEN_BW                  (3)                        /*[Hz], Flux weakening loop bandwidth (default: 3.0f)*/
#endif
/*********Rotor Pre-Alignment*********/
#define MOTOR_CTRL_ALIGN_TIME                      (1)                        /*[sec], Alignment time (default: 1.0f)*/
#define MOTOR_CTRL_ALIGN_VOLTAGE                   (0.67125)  /*[Vpk], Alignment voltage (default: 0.5f * (MOTOR_R * MOTOR_CURRENT_CONT))*/

/*********Six-Pulse Injection*********/
#define MOTOR_CTRL_SIX_PULSE_INJ_MAX_CURRENT       (3.5)                        /*[A}, Maximum current during  six pulse injection (default: 3.5f)*/

/*********High Frequency Injection*********/
#define MOTOR_CTRL_HFI_TYPE                        (0)                   /*[], Type: sine-wave or square-wave (default: Sine_Wave)*/
#define MOTOR_CTRL_HFI_MAX_CURRENT                 (1.75)                       /*[A], Maximum excitation current d-axis (default: 1.75f)*/
#define MOTOR_CTRL_HFI_INJECT_FREQ                 (1500)                     /*[Hz], Injection Frequency (default: 1500.0f)*/
#define MOTOR_CTRL_HFI_SEPARATION_FREQ             (150)                      /*[Hz], Separation frequency (default: 150.0f)*/
#endif

#if defined(CTRL_METHOD_RFO)
/*********Catch free running motor*********/
#define MOTOR_CTRL_CATCH_SPIN_MODE                (0)       /*[], mode:Zero_Current_Control=0,Direct_Bemf_Measure =1 (default: Zero_Current_Control)*/
#define MOTOR_CTRL_CATCH_SPIN_TIME                (0)                       /*[sec], Catch Spin time (default: 0.0f)*/
#define MOTOR_CTRL_CATCH_SPIN_SPEED_THRESH        (800)/*[RPM], Catch spin motor minimum speed threshold (default: MOTOR_CTRL_OBS_SPEED_THRESH)*/
#endif

/*******************************************************************************/
/*******DC Supply*******/
#define MOTOR_CTRL_VDC_NOM_VOLT                    (24)                       /*[V], Nominal DC bus voltage (default: 24.0f)*/

/*******************************************************************************/
/*******Motor*******/
#define MOTOR_POLE                                 (8)                       /*[],  motor poles (default: 8.0f)*/
#define MOTOR_LQ                                   (0.00105)                  /*[H], Stator q-axis inductance (default: 670.0E-6f)*/
#define MOTOR_LD                                   (0.00105)                  /*[H], Stator d-axis inductance (default: 670.0E-6f)*/
#define MOTOR_I_AM                                 (0.00338)                    /*[Wb],  Rotor flux linkage (default: 6.0E-3f)*/
#define MOTOR_R                                    (1.1)                  /*{Ohm],  stator resistance (default: 450.0E-3f)*/
#define MOTOR_TORQUE_MAX                           (0.19)                     /*[Nm],  maximum torque (default: 0.390f)*/
#define MOTOR_CURRENT_PEAK                         (5.4)                     /*[A],  peak current rating (default: 10.80f)*/
#define MOTOR_CURRENT_CONT                         (1.79)                      /*[A],  continuous current rating (default: 3.50f)*/
#define MOTOR_ID_MAX                               (0.447)                      /*[A], maximum d-axis current (default: 1.75f)*/
#define MOTOR_VOLTAGE                              (24)                       /*[V], motor voltage (default: 24.0f)*/
#define MOTOR_NORM_SPEED                           (4000)                     /*[RPM], nominal speed (default: 4000.0f)*/
#define MOTOR_MAX_SPEED                            (6000)                     /*[RPM],  maximum no load speed (default: 6000.0f)*/
#if defined(CTRL_METHOD_SFO)
#define MOTOR_MTPV_TORQUE_MARGIN                   (90.0f)                      /*[%],  MTPV torque margin (default: 90.0f)*/
#endif

/*******I2T Protection*******/
#define MOTOR_CTRL_I2T_THERM_TAU                   (2.5)                       /*[sec], Thermal time constant (default: 2.5f)*/

/*******Profiler*******/
#if defined(CTRL_METHOD_RFO) || defined(CTRL_METHOD_SFO)
#define MOTOR_CTRL_PROFILER_PARAM_OVERWRITE        (Dis)                       /*Write the parameter value calculated from profiler (default: Dis)*/
#define MOTOR_CTRL_PROFILER_CMD_THRESH             (5)                       /*[%], Activation command threshold (default: 5.0f)*/
#define MOTOR_CTRL_PROFILER_CMD_HYST               (2.5)                     /*[%], Activation command hysteresis (default: 2.5f)*/
#define MOTOR_CTRL_PROFILER_I_CMD_DC   (0.895)             /*[A}, Target DC current (default: MOTOR_CURRENT_CONT * 0.5f)*/
#define MOTOR_CTRL_PROFILER_I_CMD_AC   (0.4475)            /*[A}, Target AC current (default: MOTOR_CURRENT_CONT * 0.25f)*/
#define MOTOR_CTRL_PROFILER_SPEED_CMD_MIN  (1600) /*[RPM], Initial electrical speed (default: 2.0f * MOTOR_CTRL_OBS_SPEED_THRESH)*/
#define MOTOR_CTRL_PROFILER_SPEED_CMD_MAX   (3000)         /*[RPM], Final electrical speed (default: 0.75f * MOTOR_NORM_SPEED)*/
#define MOTOR_CTRL_PROFILER_ROTOR_LOCK_TIME        (1)                      /*[sec], Rotor locking time (default: 1.0f)*/
#define MOTOR_CTRL_PROFILER_FLUX_EST_TIME          (1.5)                        /*[sec], Flux estimation time (default: 1.5f)*/
#endif
/*******************************************************************************/

/*******Mechanical Load*******/
#define MECH_INERTIA                              (0.000011)                    /*[kg.m^2],  Inertia (default: 1.1E-5f)*/
#define MECH_VISCOUS                              (0.000012)                    /*[kg.m^2/sec], Viscous Damping (default: 1.2E-5f)*/
#define MECH_FRICTION                             (0.006)                    /*[kg.m^2/sec^2], Friction (default: 6.0E-3f)*/

/*******************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/

