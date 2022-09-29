#include <DmxInput.h>
#include <DmxInput.pio.h>
#include <DmxOutput.h>
#include <DmxOutput.pio.h>

#include <Arduino.h>
#include <DmxInput.h>
#include <DmxOutput.h>

#define START_CHANNEL 1
#define NUM_CHANNELS 196
#define UNIVERSE_LENGTH 197
#define USER_ADDRESS 177
#define FADE_TIME 82
#define MASTER 84

DmxInput dmx_input;
DmxOutput dmx_output;

volatile uint8_t input_buffer[DMXINPUT_BUFFER_SIZE(START_CHANNEL, NUM_CHANNELS)];
uint8_t output_buffer[UNIVERSE_LENGTH];

void setup()
{
  // Start the DMX Output on GPIO-pin 0
  dmx_output.begin(0);
  // Start the DMX Input on GPIO 5 (channel 1 to 192)
  dmx_input.begin(5, START_CHANNEL, NUM_CHANNELS);
  // Read continuously in the background
  dmx_input.read_async(input_buffer);  
  // Set all channels in the output_buffer to 0
  for (int i = 0; i < UNIVERSE_LENGTH; i++) {
    output_buffer[i] = 0;
  }
  // Set all channels in the output_buffer to 0 (in case no input is being received)
  for (int i = 0; i < UNIVERSE_LENGTH; i++) {
    input_buffer[i] = 0;
  }
  Serial.begin(9600);   
  pinMode(LED_BUILTIN, OUTPUT);  
}

void loop()
{
  for (int i = 0; i < UNIVERSE_LENGTH; i++) {
    output_buffer[i] = step(output_buffer[i], input_buffer[i], 1);
  }
  output_buffer[FADE_TIME] = 0;
  output_buffer[MASTER] = 0;
  dmx_output.write(output_buffer, NUM_CHANNELS);
  while (dmx_output.busy());
  // delay a millisecond for stability (Not strictly necessary)
  delay(1);
}

void loop1()
{  
  delay(30);

  if(millis() > 100+dmx_input.latest_packet_timestamp()) {
    Serial.println("no data!");
    return;
  }
  // Print the DMX channels
  Serial.print("Received packet: ");
  for (uint i = 0; i < sizeof(input_buffer); i++)
  {
    Serial.print(input_buffer[i]);
    Serial.print(", ");
  }
  Serial.println("");

  // Blink the LED to indicate that a packet was received
  digitalWrite(LED_BUILTIN, HIGH);
  delay(10);
  digitalWrite(LED_BUILTIN, LOW);
}

uint8_t step(uint8_t a, uint8_t b, uint8_t step_value)
{
  if (a > (b+step_value))
    a -= step_value;
  else if (a < (b-step_value))
    a += step_value;
  else
    a = b;
  return a;
}