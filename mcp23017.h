#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_err.h"
#include "hal/i2c_types.h"

typedef struct MCPDevice_t {
  i2c_port_t portNumber;
  uint8_t sdaPin;
  uint8_t sclPin;
  uint8_t address;
} MCPDevice_t;

/** Registers */
// Controls the direction of the data I/O for port A.
static const uint8_t MCP_REG_IODIRA = 0x00;

// Controls the direction of the data I/O for port B.
static const uint8_t MCP_REG_IODIRB = 0x01;

// Configures the polarity on the corresponding GPIO_ port bits for port A.
static const uint8_t MCP_REG_IPOLA = 0x02;

// Configures the polarity on the corresponding GPIO_ port bits for port B.
static const uint8_t MCP_REG_IPOLB = 0x03;

// Controls the interrupt-on-change for each pin of port A.
static const uint8_t MCP_REG_GPINTENA = 0x04;

// Controls the interrupt-on-change for each pin of port B.
static const uint8_t MCP_REG_GPINTENB = 0x05;

// Controls the default comparaison value for interrupt-on-change for port A.
static const uint8_t MCP_REG_DEFVALA = 0x06;

// Controls the default comparaison value for interrupt-on-change for port B.
static const uint8_t MCP_REG_DEFVALB = 0x07;

// Controls how the associated pin value is compared for the interrupt-on-change
// for port A.
static const uint8_t MCP_REG_INTCONA = 0x08;

// Controls how the associated pin value is compared for the interrupt-on-change
// for port B.
static const uint8_t MCP_REG_INTCONB = 0x09;

// Controls the device.
static const uint8_t MCP_REG_IOCONA = 0x0A;

// Controls the device. Usually used only MCP_REG_IOCONA
static const uint8_t MCP_REG_IOCONB = 0x0B;

// Controls the pull-up resistors for the port A pins.
static const uint8_t MCP_REG_GPPUA = 0x0C;

// Controls the pull-up resistors for the port B pins.
static const uint8_t MCP_REG_GPPUB = 0x0D;

// Reflects the interrupt condition on the port A pins.
static const uint8_t MCP_REG_INTFA = 0x0E;

// Reflects the interrupt condition on the port B pins.
static const uint8_t MCP_REG_INTFB = 0x0F;

// Captures the port A value at the time the interrupt occured.
static const uint8_t MCP_REG_INTCAPA = 0x10;

// Captures the port B value at the time the interrupt occured.
static const uint8_t MCP_REG_INTCAPB = 0x11;

// Reflects the value on the port A.
static const uint8_t MCP_REG_GPIOA = 0x12;

// Reflects the value on the port B.
static const uint8_t MCP_REG_GPIOB = 0x13;

// Provides access to the port A output latches.
static const uint8_t MCP_REG_OLATA = 0x14;

// Provides access to the port B output latches.
static const uint8_t MCP_REG_OLATB = 0x15;

esp_err_t I2CMasterInit(const MCPDevice_t* device);

/**
 * @brief Master device write data to slave.
 * @param i2cPortNum - number of i2c port on master(I2C_NUM_0, I2C_NUM_1, etc.)
 * @param i2cAddrress - address of slave device on i2c bus, for example MCP23017
 * has 0x20
 * @param reg - device specific register to read, check 'registers'
 * section in header
 * @param value - value to write into register
 * @param size - data size in bytes
 *
 * start                                                                   stop
 * __|______________________________________________________________________|__
 * |   | slave addr + wr_bit + ack | register + ack | write n bytes + ack  |  |
 * ----|---------------------------|----------------|----------------------|--|
 *
 */
esp_err_t I2CMasterWriteReg(i2c_port_t i2cPortNum, uint8_t i2cAddrress,
                            uint8_t reg, uint8_t* value, size_t size);

/**
 * @brief Master device read registers from slave.
 * @param i2cPortNum - number of i2c port on master(I2C_NUM_0, I2C_NUM_1, etc.)
 * @param i2cAddrress - address of slave device on i2c bus, for example MCP23017
 * has 0x20
 * @param reg - device specific register to read, check 'registers'
 * section in header
 * @param storage - container where store read data from register
 * @param size - data size in bytes
 *
 * start                    register + ack       read 1 byte + nack    stop
 * _|______________________________|__________________________|_________|__
 * |  | slave addr + rd_bit +ack |   | read n-1 bytes + ack |          |  |
 * ---|--------------------------|---|----------------------|----------|--|
 *
 */
esp_err_t I2CMasterReadReg(i2c_port_t i2cPortNum, uint8_t i2cAddrress,
                           uint8_t reg, uint8_t* storage, size_t size);

// mcp23017 high level API
bool MCPInit(const MCPDevice_t* device);
bool MCPWriteConfig(const MCPDevice_t* device, uint8_t reg, uint8_t value);

bool MCPReadPort(const MCPDevice_t* device, uint8_t reg, uint8_t* storage);
bool MCPReadAllPorts(const MCPDevice_t* device, uint16_t* storage);

#ifdef __cplusplus
}
#endif