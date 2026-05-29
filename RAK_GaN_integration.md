# RAK_GaN Board Integration Documentation

## Executive Summary

This document describes the complete integration of the **RAK_GaN custom board** (Rutronik RAK-GaN motor control board) into a PSoC™ Control C3M5 motor control project. The integration includes board-specific hardware configuration, device settings, Infineon library dependencies, and application-level board abstractions.

**Key Facts:**
- **Base MCU:** PSC3M5FDS2AFQ1 (PSoC Control C3M5 with Cortex-M33)
- **BSP Target:** TARGET_RAK_GAN_rev0 (custom BSP)
- **ModusToolbox Version:** 3.7
- **Motor Control Library:** motor-ctrl-lib v3.2.0
- **Board Features:** 3-shunt current measurement, NTC temperature sensor, RGB LED control, OCD (Over-Current Detection)
- **Control Methods Supported:** Sensorless PMSM FOC, Hall-sensored FOC, Encoder-sensored FOC, TBC (Trapezoidal Block Commutation)

---

## Table of Contents

1. [Project Structure](#1-project-structure)
2. [Board Selection and Makefile Configuration](#2-board-selection-and-makefile-configuration)
3. [BSP Files and Board Support](#3-bsp-files-and-board-support)
4. [Board-Specific Hardware Configuration](#4-board-specific-hardware-configuration)
5. [Generated Files and Device Configurator](#5-generated-files-and-device-configurator)
6. [Infineon Library Dependencies](#6-infineon-library-dependencies)
7. [Board Abstraction Layer](#7-board-abstraction-layer)
8. [Build Process and ModusToolbox Integration](#8-build-process-and-modustoolbox-integration)
9. [Device Configurator Regeneration Risks](#9-device-configurator-regeneration-risks)
10. [Step-by-Step Migration Guide](#10-step-by-step-migration-guide)
11. [Reproduction from Clean Project](#11-reproduction-from-clean-project)
12. [Board Change Checklist](#12-board-change-checklist)
13. [Troubleshooting](#13-troubleshooting)
14. [Architecture Diagram](#14-architecture-diagram)

---

## 1. Project Structure

The project follows ModusToolbox v2.0 flow conventions with custom board support:

```
RAK_GAN_MOTOR_DEMO/
├── Makefile                              # Top-level build configuration
│   └── TARGET=RAK_GAN_rev0              # Selects custom BSP
│
├── bsps/TARGET_RAK_GAN_rev0/            # Custom Board Support Package
│   ├── bsp.mk                            # BSP build configuration
│   ├── props.json                        # BSP metadata & dependencies
│   ├── cybsp.c/h                         # Hardware initialization
│   ├── system_psc3.c                     # MCU setup
│   ├── startup_cat1b_cm33.c              # Cortex-M33 startup
│   ├── config/
│   │   ├── design.modus                  # Device Configurator file
│   │   └── GeneratedSource/
│   │       ├── cycfg_clocks.c/h          # Clock configuration
│   │       ├── cycfg_pins.c/h            # Pin assignments
│   │       ├── cycfg_peripherals.c/h     # ADC/PWM/MOTIF config
│   │       ├── cycfg_dmas.c/h            # DMA descriptors
│   │       └── cycfg_system.c/h          # System config
│   ├── TOOLCHAIN_GCC_ARM/
│   │   ├── linker_s_flash.ld            # Secure linker script
│   │   └── linker_ns_flash.ld           # Non-secure variant
│   └── deps/                             # Device files & headers
│
├── RAK_GAN/                             # Board abstraction layer
│   ├── rak_gan.c                        # Board implementation
│   └── rak_gan.h                        # Board API
│
├── libs/
│   ├── mtb.mk                           # Auto-generated lib paths
│   └── app.mk                           # App-specific makefile
│
└── build/                               # Build output
    └── RAK_GAN_rev0/Release/
        ├── *.elf                        # Executable
        ├── *.bin                        # Binary for programming
        └── *.hex                        # Hex format
```

---

## 2. Board Selection and Makefile Configuration

### How Board is Selected

**File: `Makefile` (line 43)**

```makefile
TARGET=RAK_GAN_rev0
```

This single variable:
- Selects BSP from `bsps/TARGET_RAK_GAN_rev0/`
- Triggers device-specific build rules from `bsps/TARGET_RAK_GAN_rev0/bsp.mk`
- Determines which linker script is used
- Selects correct debugger interface (JLink)

### Key Makefile Variables

```makefile
MTB_TYPE=COMBINED              # Single application (vs. multi-app)
TARGET=RAK_GAN_rev0            # Custom board target
APPNAME=mtb-example-...        # Output executable name
TOOLCHAIN=GCC_ARM              # Compiler (GCC, ARM, IAR)
CONFIG=Release                 # Release or Debug build
CTRL=CTRL_METHOD_RFO           # Motor control method (RFO = Field-Oriented)

# Motor control configuration
DEFINES+=MOTOR_CTRL_NO_OF_MOTOR=0x01              # 1 motor
DEFINES+=MOTOR_CTRL_NO_OF_SCOPE_CHANNELS=0x4     # 4 debug channels
DEFINES+=$(CTRL)                                  # Make CTRL available
DEFINES+=$(TARGET)                                # Make RAK_GAN_rev0 available

# Shared library paths
CY_GETLIBS_SHARED_PATH=../              # mtb_shared one level up
CY_GETLIBS_SHARED_NAME=mtb_shared       # Directory name
```

### Library Dependencies Location

The `../mtb_shared/` directory contains Infineon libraries shared across projects:

```
../mtb_shared/
├── motor-ctrl-lib/release-v3.2.0/
├── mtb-hal-psc3/release-v1.1.2/
├── mtb-pdl-cat1/release-v3.21.0/
├── recipe-make-cat1b/release-v2.8.2/
├── core-make/release-v3.8.0/
└── ... (other libraries)
```

---

## 3. BSP Files and Board Support

### BSP Directory Structure

The `TARGET_RAK_GAN_rev0` directory is the Board Support Package. All board-specific files reside here.

**Key files:**

| File | Purpose |
|------|---------|
| `bsp.mk` | Build configuration for this board |
| `props.json` | Board metadata & version info |
| `cybsp.c/h` | BSP initialization code |
| `system_psc3.c` | PSC3-specific MCU setup |
| `startup_cat1b_cm33.c` | Cortex-M33 reset handler |
| `config/design.modus` | Device Configurator project file |
| `config/GeneratedSource/cycfg_*.c/h` | Auto-generated configuration |

### Device Selection (bsp.mk)

```makefile
# Lines 71-78 of bsps/TARGET_RAK_GAN_rev0/bsp.mk
MPN_LIST:=PSC3M5FDS2AFQ1
DEVICE:=PSC3M5FDS2AFQ1
DEVICE_COMPONENTS:=CAT1 CAT1B PSC3
DEVICE_PSC3M5FDS2AFQ1_FLASH_KB:=256
DEVICE_PSC3M5FDS2AFQ1_SRAM_KB:=64
```

**Device breakdown:**
- **PSC3M5** - PSoC Control C3, M5 variant
- **FDS2** - Package variant (BGA)
- **AFQ1** - Die and speed grade
- **256KB Flash, 64KB SRAM** - Memory configuration

### Debugger Configuration

```makefile
# Lines 61-62
BSP_PROGRAM_INTERFACE=JLink
```

This enables J-Link programming (vs. cmsis-dap, etc.)

### Linker Script Selection

```makefile
# Lines 40-46
ifeq ($(TOOLCHAIN),GCC_ARM)
ifeq ($(VCORE_ATTRS),SECURE)
MTB_BSP__LINKER_SCRIPT=TOOLCHAIN_GCC_ARM/linker_s_flash.ld
else
MTB_BSP__LINKER_SCRIPT=TOOLCHAIN_GCC_ARM/linker_ns_flash.ld
endif
endif
```

For RAK_GaN: Uses **secure linker script** (VCORE_ATTRS=SECURE)

### Binary Generation

```makefile
# Lines 55-59
CY_BSP_POSTBUILD+=$(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR)/bin/arm-none-eabi-objcopy \
  -O binary $(APPNAME).elf $(APPNAME).bin
```

After linking, generates .bin file for programming.

---

## 4. Board-Specific Hardware Configuration

### Device: PSC3M5FDS2AFQ1

**Specifications:**
- **Core:** Arm Cortex-M33 (32-bit, 150 MHz)
- **DSP:** DSP extension (multiply-accumulate, motor control optimized)
- **FPU:** Floating-point unit (for FOC math)
- **Memory:** 256KB Flash, 64KB SRAM
- **Peripherals:** GPIO, UART, SPI, CAN, ADC (HPPASS), PWM (TCPWM)
- **Security:** TrustZone-M (secure/non-secure partitioning)

### Clock Configuration

From `bsps/TARGET_RAK_GAN_rev0/README.md` (lines 45-54):

| Clock | Source | Frequency | Purpose |
|-------|--------|-----------|---------|
| FLL | IMO | 100 MHz | Base clock |
| CLK_HF0 | CLK_PATH1 | 180 MHz | General high-speed |
| CLK_HF1 | CLK_PATH1 | 180 MHz | TCPWM (PWM) |
| CLK_HF2 | CLK_PATH0 | 100 MHz | General |
| CLK_HF3 | CLK_PATH2 | 240 MHz | ADC/HPPASS |
| CLK_HF4 | CLK_PATH0 | 100 MHz | Peripherals |

**Generated in:** `bsps/TARGET_RAK_GAN_rev0/config/GeneratedSource/cycfg_clocks.h`

### Power Configuration

From `README.md` (lines 56-62):

```
System Active Power Mode: Over-Drive (OD)
System Idle Power Mode: CPU Sleep
VDDA (Analog): 3.3V
VDDD (Digital): 3.3V
```

### ADC Configuration (3-Shunt Current Measurement)

From `cycfg_peripherals.h`:

**ADC Channels:**
```c
ADC_SAMP_IU_CHAN_IDX = 10       // Phase U current
ADC_SAMP_IV_CHAN_IDX = 11       // Phase V current
ADC_SAMP_IW_CHAN_IDX = 12       // Phase W current
ADC_SAMP_VBUS_CHAN_IDX = 1      // DC-link voltage
ADC_SAMP_VPOT_CHAN_IDX = 8      // Potentiometer (speed ref)
ADC_SAMP_IDCLINKAVG_CHAN_IDX = 5// DC-link average
ADC_SAMP_TEMP_CHAN_IDX = 16     // Temperature (NTC)
```

**DMA Mapping (from `rak_gan.h`):**

```
DMA_ADC_0 (Sequence 0 - High Priority):
  ├─ Desc0: ADC Ch10 (IU)    → dma_results[0]  (ADC_ISAMPA)
  ├─ Desc1: ADC Ch12 (IW)    → dma_results[1]  (ADC_ISAMPC)
  ├─ Desc2: ADC Ch8 (VPOT)   → dma_results[7]  (ADC_VPOT)
  └─ Desc3-4: Dummy

DMA_ADC_1 (Sequence 1 - Low Priority):
  ├─ Desc0: ADC Ch11 (IV)    → dma_results[5]  (ADC_ISAMPB)
  ├─ Desc1: ADC Ch1 (VBUS)   → dma_results[2]  (ADC_VBUS)
  ├─ Desc2: ADC Ch5 (IDCLINK)→ dma_results[6]  (ADC_ISAMPD)
  ├─ Desc3: ADC Ch16 (TEMP)  → dma_results[3]  (ADC_TEMP)
  └─ Desc4: Dummy
```

### PWM/Timer Configuration

**MOTIF (Motor Interface):**
- 6 synchronized PWM outputs (3-phase inverter)
- Hardware-coordinated complementary PWM generation
- Dead-time insertion for gate driver protection

**LED Control:**
- RGB LED: 3 separate PWM outputs (1 kHz typical)
- Individual brightness control via PWM duty cycle

**OCD Threshold:**
- 1 PWM output for over-current detection threshold
- Voltage DAC-like function (PWM → low-pass → analog voltage)

---

## 5. Generated Files and Device Configurator

### Device Configurator Workflow

```
design.modus (binary)
    ↓ (User edits in GUI)
Device Configurator
    ↓ (Validates & generates)
cycfg_*.c/h files (auto-generated)
    ↓ (During build)
Application includes cycfg_*.h
    ↓
BSP calls init_cycfg_all()
    ↓
Hardware configured at runtime
```

### Opening Device Configurator

```bash
# From project terminal:
make device-configurator
# or
make config

# Opens GUI to edit design.modus
```

### Critical Generated Files

**cycfg_clocks.h/c** - Clock tree initialization
```c
#define CLK_TCPWM_ENABLED 1U
#define CLK_TCPWM_HW CYHAL_CLOCK_BLOCK_PERIPHERAL5_8BIT
```

**cycfg_pins.h/c** - Pin multiplexing
```c
#define CYBSP_ECO_IN P1_0
#define CYBSP_ECO_OUT P1_1
// ... motor control pins
```

**cycfg_peripherals.h/c** - ADC/PWM/DMA configuration
```c
#define ADC_SAMP_IU_CHAN_IDX 10U
#define ADC_SAMP_IU_RSLT_PTR CY_HPPASS_SAR_CHAN_RSLT_PTR(10U)
// ... 20+ similar definitions
```

**cycfg_dmas.h/c** - DMA descriptors
```c
#define DMA_ADC_0_ENABLED 1U
#define DMA_ADC_1_ENABLED 1U
// Descriptor configurations
```

### Never Edit Generated Files

❌ **Do NOT manually edit:**
- cycfg_*.c/h files (overwritten on regeneration)
- .hardware-config-server.generated-files
- configuration/* (Motor Suite generated)

✅ **Safe to edit:**
- RAK_GAN/rak_gan.c (application board layer)
- Makefile (build config)
- Application source files

---

## 6. Infineon Library Dependencies

### Dependency Tree

Fetched into `../mtb_shared/` by `make getlibs`:

```
RAK_GAN_MOTOR_DEMO
├── core-make v3.8.0 ..................... Build system framework
├── recipe-make-cat1b v2.8.2 ............ CAT1B recipes
├── mtb-pdl-cat1 v3.21.0 ................ Peripheral Device Library
│   └── psc3.svd ........................ Register definitions
├── mtb-hal-psc3 v1.1.2 ................ PSC3 Hardware Abstraction Layer
├── motor-ctrl-lib v3.2.0 .............. Motor control algorithms
│   ├── CTRL_METHOD_RFO (Field-Oriented)
│   ├── 3-shunt current measurement
│   └── MOTIF support
├── syspm-callbacks-psc3 v1.1.0 ........ Power management
├── core-lib v1.7.0 ................... Utility functions
├── cmsis v6.1.0 ....................... ARM CMSIS-CORE
├── block-storage v1.3.2 .............. Flash storage abstraction
└── emeeprom v2.70.0 .................. Emulated EEPROM
```

### Key Library Files

**mtb-pdl-cat1** - Low-level drivers
```
cy_gpio.h                  // GPIO operations
cy_tcpwm_pwm.h            // PWM generation
cy_tcpwm_motif.h          // MOTIF motor interface
cy_hppass.h               // ADC (High-Performance Analog Subsystem)
cy_sysclk.h               // Clock management
cy_sysint.h               // Interrupt handling
```

**mtb-hal-psc3** - PSC3-specific HAL
```
cyhal_clock.h             // Clock abstraction
cyhal_gpio.h              // GPIO abstraction
cyhal_hwmgr.h             // Hardware manager
```

**motor-ctrl-lib** - Motor control algorithms
```
mtb_motor.h               // Motor control data structures
mtb_motor_field_oriented.h // FOC implementation
mtb_motor_current_*.h     // Current measurement interface
mtb_motor_encoder.h       // Encoder decoding
mtb_motor_Hall.h          // Hall sensor
mtb_motor_trap_comm.h     // Trapezoidal commutation
mtb_motor_sensorless.h    // Sensorless algorithm
```

### Downloading Libraries

```bash
cd RAK_GAN_MOTOR_DEMO
make getlibs

# This reads:
# 1. Makefile (TARGET, dependencies)
# 2. bsps/TARGET_RAK_GAN_rev0/props.json (exact versions)
# 3. Downloads all listed dependencies
# 4. Generates libs/mtb.mk with SEARCH_* paths
```

---

## 7. Board Abstraction Layer

### RAK_GaN Board Files

**Location:** `RAK_GAN/` directory

- `rak_gan.h` - Board API and calibration constants
- `rak_gan.c` - Board implementation

### Board API Functions

**Power Management:**
```c
void rak_gan_enable_power_input(void)
    // Toggles POW_EN pin to stabilize power supply
    // Called at startup
```

**LED Control:**
```c
void rak_gan_init_led_and_ocd_pwm(void)
    // Initializes RGB LED PWM and OCD threshold PWM
    
void rak_gan_update_fault_led(void)
    // Sets red LED if motor has faults
    
void rak_gan_update_status_leds(void)
    // Green: forward, Blue: reverse, Off: stopped
```

**OCD (Over-Current Detection):**
```c
uint32_t OCD_Current_to_PWM(float i_limit)
    // Converts current limit (Amps) to PWM compare value
    
bool rak_gan_is_ocd_fault_active(void)
    // Debounced overcurrent detection (active-low pin)
```

**Temperature:**
```c
float rak_gan_MCU_TempSensorCalc(void)
    // NTC temperature sensor with 39-point lookup table
    // Interpolates between calibration points
    // Range: -40°C to +150°C
```

### Temperature Sensor Details

**NTC Type:** NTCG103JF103FT1S

**Calibration Table:**
- 39 calibration points (-40°C to +150°C in 5°C steps)
- ADC readings vs. temperature
- Linear interpolation between points

**Calculation:**
1. Read ADC_TEMP channel from DMA
2. Binary search ntc_adc_table
3. Linear interpolate temperature
4. Return result in Celsius

### ADC/DMA Mapping Comment

From `rak_gan.h`:
```c
/* ADC/DMA mapping (RAK_GAN, Three_Shunt, MUXA)
   10-slot schema kept (5 samples per sequence, 2 DMA channels).
   
   DMA_ADC_0 → SEQ0: IU, IW, VPOT, dummy, dummy
   DMA_ADC_1 → SEQ1: IV, VBUS, IDCLINK, TEMP, dummy
*/
```

This maps the ADC channels to motor control library `dma_results` array indices.

---

## 8. Build Process and ModusToolbox Integration

### Build Sequence

```bash
make build
```

Steps:
1. Parse `Makefile` → discovers TARGET=RAK_GAN_rev0
2. Load `bsps/TARGET_RAK_GAN_rev0/bsp.mk`
3. Load all library makefile rules from `libs/mtb.mk`
4. Auto-discover source files:
   - `RAK_GAN/*.c`
   - `bsps/TARGET_RAK_GAN_rev0/*.c`
   - `config/GeneratedSource/cycfg*.c`
   - Motor library files matching CTRL_METHOD_RFO
5. Compile with GCC_ARM
6. Link with `linker_s_flash.ld`
7. Generate binary with `arm-none-eabi-objcopy`

### Common Make Targets

```bash
make build                  # Full rebuild
make clean build            # Clean + build
make program                # Flash device (requires J-Link)
make debug                  # Launch debugger
make config                 # Open Device Configurator
make getlibs                # Download libraries
make motor-suite-gui        # Launch Motor Suite
```

### Build Directory Structure

```
build/RAK_GAN_rev0/Release/
├── mtb-example-*.elf       # Executable (debug symbols)
├── mtb-example-*.bin       # Binary for programming
├── mtb-example-*.hex       # Hex format
├── mtb-example-*.map       # Symbol map
└── obj/                    # Object files
    ├── cybsp.o
    ├── cycfg*.o
    ├── rak_gan.o
    └── ... (many more)
```

---

## 9. Device Configurator Regeneration Risks

### ⚠️ Critical Areas

#### ADC/DMA Channel Mapping
If ADC channels reassigned, motor control reads wrong sensor values.

**Mitigation:**
- Document current mapping in rak_gan.h
- Use consistent signal names in Device Configurator
- Test immediately after regeneration

#### PWM Pin Assignments
If phase U/V/W pins change, motor phase sequence wrong or drivers damaged.

**Mitigation:**
- Lock PWM pin assignments
- Verify phase sequence after regeneration
- Test motor rotation direction

#### Clock Frequencies
If PWM clock changes, MOTIF synchronization breaks.

**Mitigation:**
- Keep CLK_HF1 = 180 MHz (TCPWM clock)
- Keep CLK_HF3 = 240 MHz (ADC clock)
- Verify no clock conflicts

#### Interrupt Priorities
Motor control timing critical; priority inversion causes jitter.

**Mitigation:**
- ADC/DMA: Highest priority
- Motor PWM: High priority
- UART/debug: Low priority

### Safe Regeneration Procedure

```bash
# 1. Backup current configuration
cp bsps/TARGET_RAK_GAN_rev0/config/design.modus design.modus.backup
git commit -m "Backup before Device Configurator"

# 2. Make changes in Device Configurator
make device-configurator
# (Make ONLY necessary changes)
# Click "Generate"

# 3. Review changes
git diff bsps/TARGET_RAK_GAN_rev0/config/GeneratedSource/

# 4. Rebuild
make clean build

# 5. Test critical functions
make program
# Test motor starts, runs, measurements correct

# 6. If tests fail, revert
cp design.modus.backup bsps/TARGET_RAK_GAN_rev0/config/design.modus
make device-configurator  # Regenerate from backup
make clean build
```

---

## 10. Step-by-Step Migration Guide

### Starting from Existing Infineon Project

To adapt this integration for a different board:

#### Step 1: Create BSP Directory

```bash
mkdir -p bsps/TARGET_NEW_BOARD/{TOOLCHAIN_GCC_ARM,config/GeneratedSource}
```

#### Step 2: Update Makefile

```makefile
# Change:
TARGET=KIT_XMC7200_DC_V1

# To:
TARGET=NEW_BOARD_NAME
```

#### Step 3: Copy and Customize BSP Files

```bash
cp -r bsps/TARGET_KIT_PSC3M5_CC2/bsp.mk bsps/TARGET_NEW_BOARD/
# Edit bsp.mk for new device MPN, linker scripts, debugger
```

#### Step 4: Create design.modus

```bash
make device-configurator
# In GUI:
# 1. Select new device
# 2. Configure clocks (FLL, HF paths)
# 3. Configure ADC channels
# 4. Configure PWM instances
# 5. Configure pins
# 6. Click "Generate"
```

#### Step 5: Create Board Abstraction Layer

```bash
mkdir -p NEW_BOARD
cp -r RAK_GAN/rak_gan.* NEW_BOARD/
# Edit for new board features (calibration constants, GPIO pins)
```

#### Step 6: Update Application Code

```c
#include "new_board.h"

int main(void) {
    cybsp_init();           // BSP init
    new_board_init();       // Board init
    
    // Main loop
    for (;;) {
        // Application code
        new_board_update();  // Board status updates
    }
}
```

#### Step 7: Build & Test

```bash
make clean getlibs build
make program
# Test critical functions
```

---

## 11. Reproduction from Clean Project

### Prerequisites

- ModusToolbox 3.7+
- Motor Suite GUI 2.9.0+
- GCC Arm Embedded 14.2+
- J-Link debugger
- RAK_GaN board
- PMSM motor (e.g., Nanotec DB42S03)

### Reproduction Steps

```bash
# 1. Clone project
git clone https://github.com/RutronikSystemSolutions/RAK_GAN_MOTOR_DEMO.git
cd RAK_GAN_MOTOR_DEMO

# 2. Download libraries
make getlibs

# 3. Build
make build

# 4. Connect J-Link and power board
# (USB connection to debugger)

# 5. Program device
make program

# 6. Launch debugger
make debug

# 7. Test in GDB
# (gdb) target remote localhost:2331
# (gdb) c
# (gdb) b main
# Monitor LED blinks, motor starts, measurements valid
```

### Expected Output

```
Building: RAK_GAN_rev0/Release
  Compiling: source/main.c
  Compiling: bsps/TARGET_RAK_GAN_rev0/cybsp.c
  Compiling: bsps/TARGET_RAK_GAN_rev0/config/GeneratedSource/cycfg*.c
  Compiling: RAK_GAN/rak_gan.c
  Linking:  build/RAK_GAN_rev0/Release/mtb-example-ce240614-motor-control-solutions.elf
  Building binary: build/RAK_GAN_rev0/Release/mtb-example-ce240614-motor-control-solutions.bin
```

---

## 12. Board Change Checklist

Use when modifying board or creating new board variant:

### Pre-Change
- [ ] Document requirement and success criteria
- [ ] Identify affected components
- [ ] Git commit current state
- [ ] Plan tests

### Makefile
- [ ] Update TARGET variable
- [ ] Update device-specific DEFINEs
- [ ] Update TOOLCHAIN if needed
- [ ] Verify build: `make clean build`

### BSP (bsp.mk)
- [ ] Update device MPN
- [ ] Update linker scripts
- [ ] Update debugger interface
- [ ] Update post-build commands

### Device Configurator (design.modus)
- [ ] Select correct device
- [ ] Configure clocks (FLL, HF paths)
- [ ] Configure ADC channels
- [ ] Configure PWM outputs
- [ ] Configure pins
- [ ] Configure DMA descriptors
- [ ] Click "Generate"

### Board Abstraction Layer
- [ ] Update calibration constants
- [ ] Update GPIO pin definitions (from cycfg_pins.h)
- [ ] Update board-specific functions
- [ ] Verify preprocessor guards

### Application Code
- [ ] Update ADC channel indices
- [ ] Update PWM handles
- [ ] Update GPIO pins
- [ ] Test critical functions

### Testing & Verification
- [ ] Compilation warnings: 0
- [ ] Motor starts and runs
- [ ] Current measurements correct
- [ ] Temperature sensor valid
- [ ] LED indicators work
- [ ] OCD protection active
- [ ] Fault detection works

### Documentation
- [ ] Update this file (RAK_GaN_integration.md)
- [ ] Update README.md
- [ ] Update code comments
- [ ] Git commit with clear message

---

## 13. Troubleshooting

### Build Fails

**Error: "Cannot find TARGET_RAK_GAN_rev0"**
```bash
# Verify BSP exists
ls bsps/TARGET_RAK_GAN_rev0/

# Check Makefile
grep "^TARGET=" Makefile
```

**Error: "motor-ctrl-lib not found"**
```bash
# Download libraries
make getlibs

# Verify
ls ../mtb_shared/motor-ctrl-lib/
```

### Programming Fails

**Error: "J-Link not detected"**
```bash
# Check USB connection
lsusb | grep SEGGER

# Install drivers
# https://www.segger.com/downloads/jlink/
```

### Runtime Issues

**Motor doesn't start:**
- Check PWM on oscilloscope (should see 16-20 kHz)
- Verify motor connections (3-phase winding)
- Check motor enable signal (motor[0].vars_ptr->en)

**Current measurements wrong:**
- Verify ADC channel assignment (cycfg_peripherals.h)
- Check DMA descriptor order
- Verify current sensor connections

**Temperature reads 0°C:**
- Check ADC_TEMP channel connection
- Verify NTC lookup table populated
- Check ACTIVE_TEMP_SENSOR define

**LED doesn't light:**
- Check PWM on oscilloscope
- Verify LED hardware connection
- Check rak_gan_init_led_and_ocd_pwm() called

**OCD not working:**
- Verify OCD threshold PWM set
- Check OCD fault pin (PWR_OCD_NUM)
- Test with excess current draw

---

## 14. Architecture Diagram

```
┌─────────────────────────────────────────┐
│      Hardware: PSC3M5FDS2AFQ1           │
│  Cortex-M33 256KB Flash 64KB SRAM       │
│  DSP, FPU, TrustZone-M                  │
└─────────────────────────────────────────┘
           ↑           ↑           ↑
    ┌──────┴─────┬─────┴────┬─────┴─────┐
    │            │          │           │
    PWM          ADC        GPIO        UART
  6 phases    7 channels   Power/OCD   Debug
    │            │          │           │
    ↓            ↓          ↓           ↓
┌─────────────────────────────────────────┐
│   bsps/TARGET_RAK_GAN_rev0/             │
│   ├─ cybsp.c/h (BSP init)              │
│   ├─ config/GeneratedSource/cycfg_*    │
│   ├─ system_psc3.c (MCU init)          │
│   └─ startup_cat1b_cm33.c (reset)      │
└─────────────────────────────────────────┘
    ↑
    │ Configures
    │
┌─────────────────────────────────────────┐
│   RAK_GAN/                              │
│   ├─ rak_gan.c/h (board abstraction)    │
│   └─ Calls motor-ctrl-lib functions     │
└─────────────────────────────────────────┘
    ↑
    │ Uses
    │
┌─────────────────────────────────────────┐
│   Motor Control Library                 │
│   ├─ FOC algorithms                     │
│   ├─ 3-shunt measurement                │
│   ├─ MOTIF support                      │
│   └─ Fault handling                     │
└─────────────────────────────────────────┘
    ↑
    │ Uses
    │
┌─────────────────────────────────────────┐
│   PDL (mtb-pdl-cat1)                    │
│   ├─ cy_gpio.h                          │
│   ├─ cy_tcpwm_pwm.h / cy_tcpwm_motif.h  │
│   ├─ cy_hppass.h (ADC)                  │
│   └─ cy_sysclk.h (clock)                │
└─────────────────────────────────────────┘
```

---

## Support & Resources

- **Project:** https://github.com/RutronikSystemSolutions/RAK_GAN_MOTOR_DEMO
- **BSP:** https://github.com/Infineon/TARGET_KIT_PSC3M5_CC2
- **Motor Library:** https://github.com/Infineon/mtb-example-ce240614-motor-control-solutions
- **ModusToolbox:** https://www.infineon.com/modustoolbox
- **Device Configurator:** Built into ModusToolbox

---

**Document Last Updated:** 2026-05-29
**Integration Version:** 1.0
**RAK_GaN BSP Version:** TARGET_RAK_GAN_rev0
