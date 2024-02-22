#include "DHT22.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include <stdio.h>

// Define the GPIO pin where the DHT sensor is connected
#define DHT_PIN 22

// Minimum time to wait before reading from the sensor again
#define MIN_INTERVAL 2000

// Function prototypes
void DHT_init();
bool DHT_read(float *temperature, float *humidity);
uint32_t expectPulse(bool level);

void DHT_init()
{
  gpio_init(DHT_PIN);
  gpio_set_dir(DHT_PIN, GPIO_IN);
  gpio_pull_up(DHT_PIN); // Ensure the pull-up is enabled if required by your sensor
}

bool DHT_read(float *temperature, float *humidity)
{
  uint32_t currentMillis = to_ms_since_boot(get_absolute_time());
  static uint32_t lastReadTime = 0;

  if ((currentMillis - lastReadTime) < MIN_INTERVAL)
  {
    // Waiting for the minimum interval to pass
    printf("Waiting for the minimum interval\n");
    return false;
  }
  lastReadTime = currentMillis;

  // Buffer to store the pulses widths
  uint32_t pulses[80] = {0};

  // Start signal
  gpio_set_dir(DHT_PIN, GPIO_OUT);
  gpio_put(DHT_PIN, 0);
  sleep_ms(20); // DHT22 start signal must be at least 18ms
  gpio_put(DHT_PIN, 1);
  sleep_us(40);
  gpio_set_dir(DHT_PIN, GPIO_IN);

  // Acknowledge or "response" signal
  if (expectPulse(0) == UINT32_MAX || expectPulse(1) == UINT32_MAX)
  {
    printf("DHT timeout error\n");
    return false;
  }

  // Read output - 40 bits => 5 bytes or [16 bits Temp][16 bits Humidity][8 bit checksum]
  for (int i = 0; i < 80; i += 2)
  {
    pulses[i] = expectPulse(0);
    pulses[i + 1] = expectPulse(1);

    if (pulses[i] == UINT32_MAX || pulses[i + 1] == UINT32_MAX)
    {
      printf("DHT bit read timeout\n");
      return false;
    }
  }

  // Convert pulse length into bits and store them
  uint8_t bits[5] = {0};
  for (int i = 0; i < 40; ++i)
  {
    uint32_t lowCycles = pulses[2 * i];
    uint32_t highCycles = pulses[2 * i + 1];
    bits[i / 8] <<= 1;
    if (highCycles > lowCycles)
    {
      // If high cycles are greater than low cycles it is a 1
      bits[i / 8] |= 1;
    }
  }

  // Conversion and checksum verification
  uint16_t rawHumidity = (bits[0] << 8) | bits[1];
  uint16_t rawTemperature = (bits[2] << 8) | bits[3];
  uint8_t checkSum = bits[4];

  // Calculate temperature and humidity
  *humidity = rawHumidity * 0.1;
  *temperature = rawTemperature * 0.1;

  // Checksum calculation
  if (checkSum == ((bits[0] + bits[1] + bits[2] + bits[3]) & 0xFF))
  {
    printf("Temperature: %.2f C, Humidity: %.2f%%\n", *temperature, *humidity);
    return true;
  }
  else
  {
    printf("Checksum error\n");
    return false;
  }
}

uint32_t expectPulse(bool level)
{
  uint32_t count = 0;
  while (gpio_get(DHT_PIN) == level)
  {
    if (++count >= 10000)
    {
      return UINT32_MAX; // Timeout
    }
    tight_loop_contents();
  }
  return count;
}
