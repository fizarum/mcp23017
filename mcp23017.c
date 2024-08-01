#include "mcp23017.h"

#include <driver/i2c.h>

#include "esp_log.h"

// I2C master will check ack from slave
#define ACK_CHECK_EN 0x1

// I2C ack value
#define ACK_VAL 0x0

// I2C nack value
#define NACK_VAL 0x1

// I2C master clock frequency
#define I2C_FREQ_HZ 100000

static const char *TAG = "MCP23017";

const static TickType_t _500 = pdMS_TO_TICKS(500);

static uint8_t portAVal;
static uint8_t portBVal;

/**
 * @brief Start i2c session
 * @return link to session
 */
i2c_cmd_handle_t I2CCreateSession();

/**
 * @brief Executes commands and close session
 *
 * @param cmd - link to session
 * @param i2cNum - port number to close
 */
esp_err_t I2CExecuteSession(i2c_cmd_handle_t cmd, i2c_port_t i2cNum);

/**
 * @brief i2c master initialization
 */
esp_err_t I2CMasterInit(const MCPDevice_t *device) {
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = device->sdaPin,
      .scl_io_num = device->sclPin,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_FREQ_HZ,
  };

  esp_err_t result = i2c_param_config(device->portNumber, &conf);
  if (result != ESP_OK) {
    return result;
  }
  // install i2c driver without buffering
  return i2c_driver_install(device->portNumber, conf.mode, 0, 0, 0);
}

i2c_cmd_handle_t I2CCreateSession() {
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  return cmd;
}

esp_err_t I2CExecuteSession(i2c_cmd_handle_t cmd, i2c_port_t i2cNum) {
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(i2cNum, cmd, _500);
  i2c_cmd_link_delete(cmd);
  return ret;
}

esp_err_t I2CMasterWriteReg(i2c_port_t i2cPortNum, uint8_t i2cAddrress,
                            uint8_t reg, uint8_t *value, size_t size) {
  i2c_cmd_handle_t cmd = I2CCreateSession();
  // Send device address, write bit & register to be written
  i2c_master_write_byte(cmd, (i2cAddrress << 1) | I2C_MASTER_WRITE,
                        ACK_CHECK_EN);
  // send register to write in
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  // Write the data
  i2c_master_write(cmd, value, size, ACK_CHECK_EN);

  return I2CExecuteSession(cmd, i2cPortNum);
}

esp_err_t I2CMasterReadReg(i2c_port_t i2cPortNum, uint8_t i2cAddrress,
                           uint8_t reg, uint8_t *storage, size_t size) {
  if (size == 0) {
    return ESP_OK;
  }

  i2c_cmd_handle_t cmd = I2CCreateSession();

  // Send device address, write bit & register to be read
  i2c_master_write_byte(cmd, (i2cAddrress << 1), ACK_CHECK_EN);
  // Send register to read from
  i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
  // Send repeated start
  i2c_master_start(cmd);
  // Send device address (indicating read) & read data
  i2c_master_write_byte(cmd, (i2cAddrress << 1) | I2C_MASTER_READ,
                        ACK_CHECK_EN);
  if (size > 1) {
    i2c_master_read(cmd, storage, size - 1, ACK_VAL);
  }
  i2c_master_read_byte(cmd, storage + size - 1, NACK_VAL);

  return I2CExecuteSession(cmd, i2cPortNum);
}

bool MCPInit(const MCPDevice_t *device) {
  esp_err_t initResult = I2CMasterInit(device);
  if (initResult != ESP_OK) {
    ESP_LOGE(TAG, "init failed: %d", initResult);
    return false;
  } else {
    ESP_LOGD(TAG, "init ok!");
    return true;
  }
}

bool MCPWriteConfig(const MCPDevice_t *device, uint8_t reg, uint8_t value) {
  esp_err_t result =
      I2CMasterWriteReg(device->portNumber, device->address, reg, &value, 1);
  if (result == ESP_OK) {
    ESP_LOGD(TAG, ">> [0x%x] = 0x%x OK", reg, value);
    return true;
  }
  ESP_LOGE(TAG, ">> [0x%x] = 0x%x Failed: %d", reg, value, result);
  return false;
}

bool MCPReadPort(const MCPDevice_t *device, uint8_t reg, uint8_t *storage) {
  esp_err_t result =
      I2CMasterReadReg(device->portNumber, device->address, reg, storage, 1);

  if (result == ESP_OK) {
    ESP_LOGD(TAG, "<< [0x%x] = 0x%x OK", reg, *storage);
    return true;
  }
  ESP_LOGE(TAG, "<< [0x%x] = 0x%x Failed: %d", reg, *storage, result);
  return false;
}

bool MCPReadAllPorts(const MCPDevice_t *device, uint16_t *storage) {
  if (MCPReadPort(device, MCP_REG_GPIOA, &portAVal) == true) {
    if (MCPReadPort(device, MCP_REG_GPIOB, &portBVal) == true) {
      *storage = portAVal | portBVal << 8;
      return true;
    }
  }
  return false;
}
