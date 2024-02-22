#ifndef DHT22_H
#define DHT22_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "pico/stdlib.h"

// Error codes
#define DHT_OK 0
#define DHT_ERROR_TIMEOUT -1
#define DHT_ERROR_CHECKSUM -2

    // Initialize the DHT sensor
    void DHT_init();

    // Read temperature and humidity from the DHT sensor
    bool DHT_read(float *temperature, float *humidity);

#ifdef __cplusplus
}
#endif

#endif // DHT22_H
