#include <Arduino.h>
#include "DmxInput.h"
#include <DmxOutput.h>

#define START_CHANNEL 1
#define NUM_CHANNELS 192
#define UNIVERSE_LENGTH 193
#define RGB_AMOUNT 5
#define RGB_SIZE 4
#define WHITE_AMOUNT 2
#define USER_ADDRESS 177

DmxInput dmx_input;
DmxOutput dmx_output;

volatile uint8_t input_buffer[DMXINPUT_BUFFER_SIZE(START_CHANNEL, NUM_CHANNELS)];
uint8_t output_buffer[UNIVERSE_LENGTH];

uint8_t rgb[RGB_AMOUNT];
uint8_t white[WHITE_AMOUNT];

void loop()
{
  for (int i=0; i<RGB_AMOUNT; i++) {
    for (int j=rgb[i]; j<=RGB_SIZE; j++)
      output_buffer[j] = step(output_buffer[j], input_buffer[j], 1);
    output_buffer[rgb[i]+RGB_SIZE+1] = 0; // set strobe to off
  }
  for (int i=0; i<WHITE_AMOUNT; i++) {
    output_buffer[white[i]] = step(output_buffer[white[i]], input_buffer[white[i]], 1);
    output_buffer[white[i]+1] = 0; // set strobe to off
  }
  // Send out output_buffer on GPIO-pin 1
  dmx_output.write(output_buffer, NUM_CHANNELS);
  while (dmx_output.busy())
  {
    /* Do nothing while the DMX frame transmits */
  }

  // delay a millisecond for stability (Not strictly necessary)
  delay(1);
}

void loop1()
{  
  /*input_buffer[1] = 255;
  input_buffer[2] = 128;
  input_buffer[3] = 64;
  input_buffer[4] = 32;  
  delay(3000);
  input_buffer[1] = 255;
  input_buffer[2] = 128;
  input_buffer[3] = 42;
  input_buffer[4] = 108;
  delay(8000);*/
  /*input_buffer[1] = 255;
  input_buffer[2] = 51;
  input_buffer[3] = 179;
  input_buffer[4] = 102;
  delay(5000);
  input_buffer[1] = 255;
  input_buffer[2] = 115;
  input_buffer[3] = 75;
  input_buffer[4] = 220;
  delay(7000);
  input_buffer[1] = 255;
  input_buffer[2] = 0;
  input_buffer[3] = 0;
  input_buffer[4] = 0;
  delay(4000);*/

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

void setup()
{
  Serial.begin(9600); 
  // Start the DMX Output on GPIO-pin 0
  dmx_output.begin(0);

  // Set all channels in the output_buffer to the max allowed value (512)
  for (int i = 1; i < UNIVERSE_LENGTH; i++) {
    output_buffer[i] = 0;
  }
  rgb[0] = 1;
  rgb[1] = 17;
  rgb[2] = 33;
  rgb[3] = 49;
  rgb[4] = 65;
  white[0] = 81;
  white[1] = 83;
}

void setup1()
{
  // Setup our DMX Input to read on GPIO 5, from channel 1 to 3
  dmx_input.begin(5, START_CHANNEL, NUM_CHANNELS);
  dmx_input.read_async(input_buffer);

  // Setup the onboard LED so that we can blink when we receives packets
  pinMode(LED_BUILTIN, OUTPUT);
}