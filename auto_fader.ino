#include <DmxInput.h>
#include <DmxInput.pio.h>
#include <DmxOutput.h>
#include <DmxOutput.pio.h>

#include <Arduino.h>
#include <DmxInput.h>
#include <DmxOutput.h>

#define START_CHANNEL 1
#define NUM_CHANNELS 192
#define UNIVERSE_LENGTH 193
#define USER_ADDRESS 177
#define FADE_TIME 82
#define MASTER 84
#define MIN_FRAME 9

DmxInput dmx_input;
DmxOutput dmx_output;

volatile uint8_t input_buffer[DMXINPUT_BUFFER_SIZE(START_CHANNEL, NUM_CHANNELS)];
uint8_t output_buffer[UNIVERSE_LENGTH];
unsigned long frame_time;
uint8_t fadetime;

void setup() {
  /*dmx_input.begin(5, START_CHANNEL, NUM_CHANNELS);
  // Read continuously in the background
  dmx_input.read_async(input_buffer);*/
  // Start the DMX Output on GPIO-pin 0
  dmx_output.begin(0);
  // Start the DMX Input on GPIO 5 (channel 1 to 192)
  // Set all channels in the output_buffer to 0
  for (int i = 0; i < UNIVERSE_LENGTH; i++) {
    output_buffer[i] = 0;
  }
  // Set all channels in the output_buffer to 0 (in case no input is being received)
  for (int i = 0; i < UNIVERSE_LENGTH; i++) {
    input_buffer[i] = 0;
  }
  uint8_t fadetime = 0;
}

void setup1() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  // Setup our DMX Input to read on GPIO 0, from channel 1 to 3
  dmx_input.begin(5, START_CHANNEL, NUM_CHANNELS);
}

void loop() {
  for (int i = 0; i < UNIVERSE_LENGTH; i++) {
    output_buffer[i] = step(output_buffer[i], input_buffer[i], 1);
  }
  if (input_buffer[FADE_TIME] != 0)
    fadetime = input_buffer[FADE_TIME];
  output_buffer[FADE_TIME] = 0;
  output_buffer[MASTER] = 0;
  frame_time = millis();
  dmx_output.write(output_buffer, NUM_CHANNELS);
  while (frame_time + MIN_FRAME > millis())
    ;
  while (dmx_output.busy())
    Serial.println("Bad Waiting");
}

void loop1() {
  // Wait for next DMX packet
  dmx_input.read(input_buffer);
  digitalWrite(LED_BUILTIN, LOW);

  // Print the DMX channels
  Serial.print("Sending packet: ");
  for (uint i = 0; i < sizeof(output_buffer); i++) {
    Serial.print(output_buffer[i]);
    Serial.print(", ");
  }
  Serial.println("");

  // Blink the LED to indicate that a packet was received
  digitalWrite(LED_BUILTIN, HIGH);
}

uint8_t step(uint8_t a, uint8_t b, uint8_t step_value) {
  if (a > (b + step_value))
    a -= step_value;
  else if (a < (b - step_value))
    a += step_value;
  else
    a = b;
  return a;
}

// Return the next frame time in milliseconds vary based of fadetime slider
bool not_next_frame() {
  unsigned long offset = fadetime;
  if (offset >= 240)
    return true;  // Hold current frame
  if (offset < 16)
    return false;  // Time for next frame
  // values 1-224
  offset -= 31;
  return (millis() < (frame_time + offset));
}