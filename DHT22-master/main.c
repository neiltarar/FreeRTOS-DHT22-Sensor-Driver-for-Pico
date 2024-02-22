#include <stdio.h>
#include "pico/stdio.h"
#include "DHT22.h"

void dht22_task(void)
{
    float temperature = 0.0, humidity = 0.0;
    int result;

    // Initialize the DHT22 sensor on the specified GPIO pin
    DHT_init();

    while (true)
    {
        result = DHT_read(&temperature, &humidity);
        if (result == DHT_OK)
        {
            printf("Temperature: %.2f C, Humidity: %.2f%% RH\n", temperature, humidity);
        }
        sleep_ms(2100);
    }
}
void main()
{
    stdio_init_all(); // initialise standard I/O
    while (true)
    {
        dht22_task();
    }
};