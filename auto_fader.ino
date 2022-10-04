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
unsigned long frame_start_ms;
uint8_t fadetime;
uint8_t master;

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
  set_output();
  dmx_output.write(output_buffer, NUM_CHANNELS);
  while (dmx_output.busy())
    ;
}

void loop1() {
  // Wait for next DMX packet
  dmx_input.read(input_buffer);
  if (input_buffer[FADE_TIME] != 0)
    fadetime = input_buffer[FADE_TIME];
  if (input_buffer[MASTER] != 0 || master < 6)
    master = input_buffer[MASTER];
  digitalWrite(LED_BUILTIN, LOW);

  // Print the DMX channels
  Serial.print("Packet (I,O): ");
  for (uint i = 5; i <= 8; i++) {
    Serial.print("(");
    Serial.print(input_buffer[i]);
    Serial.print(",");
    Serial.print(output_buffer[i]);
    Serial.print("), ");
  }
  Serial.println("");
  if (fadetime < 16)
  Serial.print(0);
  else
  Serial.print(fadetime >> 1);
  Serial.print(" ms, ");

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

/*uint8_t apply_master_fader(uint8_t a) {
  return (a * (256 - master)) >> 8;
}*/

// Return the next frame time in milliseconds vary based of fadetime slider
void set_output() {
  // Hold current frame
  if (fadetime >= 240)
    return;
  // Copy input_buffer into output buffer
  if (fadetime < 16) {
    for (int i = 0; i < UNIVERSE_LENGTH; i++) {
      output_buffer[i] = input_buffer[i];
    }
    output_buffer[FADE_TIME] = 0;
    output_buffer[MASTER] = 0;
  }
  // Step output buffer once per frame toward input buffer
  else if ((millis() > (frame_start_ms + (fadetime >> 1) - 31))) {
    frame_start_ms = millis();
    for (int i = 0; i < UNIVERSE_LENGTH; i++) {
      output_buffer[i] = step(output_buffer[i], input_buffer[i], 1);
    }
    output_buffer[FADE_TIME] = 0;
    output_buffer[MASTER] = 0;
  }
}