#include <Arduino.h>
#include "DmxInput.h"
#include <DmxOutput.h>

DmxInput dmxInput;

#define START_CHANNEL 1
#define NUM_CHANNELS 4

volatile uint8_t buffer[DMXINPUT_BUFFER_SIZE(START_CHANNEL, NUM_CHANNELS)];

#define UNIVERSE_LENGTH 192

// Declare an instance of the DMX Output
DmxOutput dmx;

// Create a universe that we want to send.
// The universe must be maximum 512 bytes + 1 byte of start code
uint8_t universe[UNIVERSE_LENGTH + 1];
int8_t delta_red = 1;
int8_t delta_green = -1;
int8_t delta_blue = 1;

void setup()
{
  Serial.begin(9600); 
  // Start the DMX Output on GPIO-pin 0
  dmx.begin(0);

  // Set all channels in the universe to the max allowed value (512)
  for (int i = 1; i < UNIVERSE_LENGTH + 1; i++)
  {
    universe[i] = 0;
  }
  universe[1] = 255;
  universe[2] = 78;
  universe[3] = 54;
  universe[4] = 200;
    
}

void loop()
{
  
  if (universe[2] == 255)
    delta_red = -1;
  else if (universe[2] == 1)
    delta_red = 1;
  universe[2] = universe[2] + delta_red;

  if (universe[3] == 255)
    delta_green = -1;
  else if (universe[3] == 1)
    delta_green = 1;
  universe[3] = universe[3] + delta_green;

  if (universe[4] == 255)
    delta_blue = -1;
  else if (universe[4] == 1)
    delta_blue = 1;
  universe[4] = universe[4] + delta_blue;
  
  // Send out universe on GPIO-pin 1
   dmx.write(universe, UNIVERSE_LENGTH);
  while (dmx.busy())
  {
    /* Do nothing while the DMX frame transmits */
  }

  // delay a millisecond for stability (Not strictly necessary)
  //delay(1);
  //Serial.println("sent");
}

void setup1()
{
  // Setup our DMX Input to read on GPIO 5, from channel 1 to 3
  dmxInput.begin(5, START_CHANNEL, NUM_CHANNELS);
  dmxInput.read_async(buffer);

  // Setup the onboard LED so that we can blink when we receives packets
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop1()
{
  delay(30);

  if(millis() > 100+dmxInput.latest_packet_timestamp()) {
    Serial.println("no data!");
    return;
  }
  // Print the DMX channels
  Serial.print("Received packet: ");
  for (uint i = 0; i < sizeof(buffer); i++)
  {
    Serial.print(buffer[i]);
    Serial.print(", ");
  }
  Serial.println("");

  // Blink the LED to indicate that a packet was received
  digitalWrite(LED_BUILTIN, HIGH);
  delay(10);
  digitalWrite(LED_BUILTIN, LOW);
}