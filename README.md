# FordFocus Steering Monitor

Real-time steering wheel alignment monitoring system for Ford Focus MK3.5 BR via CAN Bus.

## ⚠️ EDUCATIONAL PURPOSE ONLY

**IMPORTANT NOTICE**: This project is intended **EXCLUSIVELY FOR EDUCATIONAL PURPOSES**. It is designed to demonstrate CAN Bus communication, Arduino programming, and automotive electronics concepts. 

**DO NOT USE** this project for:
- Commercial applications
- Production vehicles
- Safety-critical systems
- Any purpose that could interfere with vehicle operation

The authors assume **NO RESPONSIBILITY** for any damage, injury, or legal issues arising from the use of this project.

## 📋 Description

This project monitors in real-time the steering wheel angle and orientation of a Ford Focus MK3.5 BR, displaying the information on an LCD screen. The system reads data directly from the vehicle's CAN Bus and calculates steering alignment.

## 🚗 Compatibility

- **Vehicle**: Ford Focus MK3.5 BR
- **Protocol**: CAN Bus HSCAN
- **CAN ID**: 0x010 (Steering Wheel Position)

## 🔧 Required Hardware

### Main Components
- **Arduino Uno/Nano** or compatible
- **CAN Module MCP2515** with TJA1050 transceiver
- **16x2 LCD Display** with parallel interface
- **Jumpers** and breadboard
- **12V Power Supply** (optional, can use USB)

### Connections

#### CAN Module (MCP2515)
| MCP2515 | Arduino |
|---------|---------|
| VCC     | 5V      |
| GND     | GND     |
| CS      | Pin 10  |
| SO      | Pin 12  |
| SI      | Pin 11  |
| SCK     | Pin 13  |
| INT     | Pin 2   |

#### LCD Display
| LCD | Arduino |
|-----|---------|
| VSS | GND     |
| VDD | 5V      |
| V0  | Potentiometer |
| RS  | Pin 3   |
| E   | Pin 4   |
| D4  | Pin 5   |
| D5  | Pin 6   |
| D6  | Pin 7   |
| D7  | Pin 8   |

## 📚 Required Libraries

Install the following libraries through Arduino IDE:

```cpp
#include <mcp_can.h>        // For CAN communication
#include <mcp_can_dfs.h>    // CAN definitions
#include <SPI.h>            // SPI communication (already included)
#include <LiquidCrystal.h>  // LCD control
```

### Library Installation
1. Open Arduino IDE
2. Go to `Sketch` → `Include Library` → `Manage Libraries`
3. Search and install:
   - `mcp_can` by Cory J. Fowler
   - `LiquidCrystal` (already included in Arduino IDE)

## 📖 Library References

### MCP_CAN Library
- **Author**: Cory J. Fowler
- **Repository**: [GitHub - coryjfowler/MCP_CAN_lib](https://github.com/coryjfowler/MCP_CAN_lib)
- **Purpose**: Interface with MCP2515 CAN controller
- **Documentation**: Provides comprehensive CAN Bus communication functions
- **Version**: Compatible with Arduino IDE 1.8.x and 2.x

### LiquidCrystal Library
- **Author**: Arduino Team
- **Repository**: [Arduino Core Libraries](https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/LiquidCrystal)
- **Purpose**: Control LCD displays with parallel interface
- **Documentation**: Standard Arduino library for 16x2 LCD displays
- **Version**: Built-in with Arduino IDE

### SPI Library
- **Author**: Arduino Team
- **Repository**: [Arduino Core Libraries](https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/SPI)
- **Purpose**: Serial Peripheral Interface communication
- **Documentation**: Standard Arduino SPI communication library
- **Version**: Built-in with Arduino IDE

### Additional Resources
- **CAN Bus Specification**: ISO 11898-1:2015
- **MCP2515 Datasheet**: Microchip Technology Inc.
- **Ford Focus CAN Database**: Proprietary Ford documentation

## 🚀 Installation and Usage

### 1. Preparation
1. Clone or download this repository
2. Connect hardware according to connection diagram
3. Connect Arduino to computer via USB

### 2. CAN Bus Configuration
1. Connect CAN module to vehicle bus:
   - **CAN_H**: Vehicle CAN High pin
   - **CAN_L**: Vehicle CAN Low pin
   - **GND**: Common ground with vehicle

### 3. Operation
1. Power on the system
2. LCD will display:
   - **First line**: Orientation (LEFT/RIGHT/CENTERED)
   - **Second line**: Numerical angle with degree symbol

## 📊 How It Works

### CAN Message ID 0x010 Analysis

The Ford Focus MK3.5 BR uses **CAN ID 0x010** to transmit steering wheel position data on the HSCAN bus. This message contains critical information about the vehicle's steering system.

#### Message Structure
```
CAN ID: 0x010 (16-bit identifier)
Data Length: 8 bytes
Bus: HSCAN (High Speed CAN)
Update Rate: ~50Hz (every 20ms)
```

#### Data Byte Breakdown
| Byte | Purpose | Description |
|------|---------|-------------|
| 0-3 | System Status | Vehicle state and diagnostics |
| 4 | Orientation | Steering direction indicator |
| 5 | Reserved | System reserved data |
| 6-7 | Angle Data | 16-bit steering angle value |

#### Byte 4 - Orientation Logic
```
Bit 7-4: Orientation Code
- 0x0: Left turn detected
- 0x8: Right turn detected
- Other values: Reserved/Error states

```

#### Bytes 6-7 - Angle Calculation
The steering angle is encoded as a 16-bit signed integer:
```cpp
// Raw data extraction
hex_combine_angle = word(buf[6], buf[7]);

// Conversion formula (Ford-specific)
angle = ((hex_combine_angle - 32768) * 1.5);
```

**Example Values:**
- `0x8000` (32768) = 0° (centered)
- `0x7FFF` (32767) = -1.5° (slight left)
- `0x8001` (32769) = +1.5° (slight right)
- `0x0000` (0) = -49.152° (hard left)
- `0xFFFF` (65535) = +49.152° (hard right)

#### Real-World Examples
```
Example 1: Straight driving
CAN ID: 0x010
Data: [0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x80, 0x00]
Angle: 0° (centered)

Example 2: Left turn
CAN ID: 0x010  
Data: [0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x7F, 0xFF]
Angle: -1.5° (left)

Example 3: Right turn
CAN ID: 0x010
Data: [0x12, 0x34, 0x56, 0x78, 0x80, 0x00, 0x80, 0x01] 
Angle: +1.5° (right)
```

### Angle Calculation
The system combines bytes 6 and 7 from CAN message and applies the formula:

```cpp
hex_combine_angle = word(buf[6], buf[7]);
angle = ((hex_combine_angle - 32768) * 1.5);
```

### Orientation Detection
- **LEFT**: `can_orientation == 0` and `angle != 0`
- **RIGHT**: `can_orientation == 8` and `angle != 0`
- **CENTERED**: `angle == 0`

### Update Frequency
- Frequency: Every 20ms
- Serial communication: 115200 baud (for debugging)

## 🔍 Serial Monitoring

For debugging and monitoring, connect Arduino to computer and open Serial Monitor (115200 baud). You will see:

```
CAN0: Init OK!
Angle: -2.5
CAN Bus Hex Value: 7FFF
```

## ⚠️ Important Warnings

- **Safety**: This project is for monitoring only. Do not interfere with critical vehicle systems.
- **Compatibility**: Works only with Ford Focus MK3.5 BR. Other models may have different CAN IDs.
- **CAN Connection**: Ensure proper connection to vehicle CAN Bus.
- **Power Supply**: Use adequate power supply for CAN module (usually 12V).
- **Educational Use**: This project is intended for learning purposes only.
