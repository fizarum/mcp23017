# Description

simple implementation of mcp23017 driver for esp-idf platform

# How to use it

1. First, create MCPDevice_t structure. Here is an example how to do it:

    ```
    MCPDevice_t mcp23017 = {
        .portNumber = I2C_NUM_0,
        .sdaPin = i2cSDA,
        .sclPin = i2cSCL,
        .address = mcpAddress,
    };
    ```

    Depending on your configuraion, portNumber can have any value of: I2C_NUM_0, I2C_NUM_1, check available values in `i2c_port_t` from `i2c_types.h` file.
    `sdaPin` and `sclPin` are ports on esp device, for example for `portNumber = I2C_NUM_0`
    - sdaPin = 21
    - sclPin = 22

    `address` is MCP23017's address on i2c bus, usually its: `0x20`.

2. Now you're ready to initialize device by calling: `MCPInit()` function, for example:

    `bool initOk = MCPInit(&mcp23017);`

3. Well done! You are ready to work with device. To write something into it you can use `MCPWriteConfig()` or low level: `I2CMasterWriteReg()` functions. To read: `MCPReadPort()`, `MCPReadAllPorts()` or low level: `I2CMasterReadReg()`. Here is an example of using these functions to make MCP23017 as keyboard:

    ```
    const uint8_t xFF = 0xFF;
    bool configuredOk = false;

    if (initOk == true) {
        uint8_t value = 1 << 5
        configuredOk = MCPWriteConfig(&mcp23017, MCP_REG_IOCONA, value);
    }

    // config as input
    configuredOk &= MCPWriteConfig(&mcp23017, MCP_REG_IODIRA, xFF);
    configuredOk &= MCPWriteConfig(&mcp23017, MCP_REG_IODIRB, xFF);

    // add pullups
    configuredOk &= MCPWriteConfig(&mcp23017, MCP_REG_GPPUA, xFF);
    configuredOk &= MCPWriteConfig(&mcp23017, MCP_REG_GPPUB, xFF);

    // set polarity
    configuredOk &= MCPWriteConfig(&mcp23017, MCP_REG_IPOLA, xFF);
    configuredOk &= MCPWriteConfig(&mcp23017, MCP_REG_IPOLB, xFF);
    
    //...
    uint16_t val = 0;

    if (MCPReadAllPorts(&mcp23017, &val) == true) {
        ESP_LOGI("MCP23017", "received AB: 0x%x\n", val);
    }
    ```
