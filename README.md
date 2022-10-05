# auto-fader
DMX-512 Auto Fader

Arduino code for making a basic DMX512 auto fader using a Pi Pico and a Waveshare Pico-2CH-RS485 hat.

Channel 1 is connect to the lightboard and Channel 0 is connected to the lighting daisy chain.

Uses DMX channel 82 (Slider 2 on Scanner/Fixture 6) for the fadetime slider. When outputing to the lighting daisy chain DMX channel 82 is always 0.

Warning: This code isn't in a state to be usable for others yet. But if you like at the source code you can probably make a few small changes to work for you.
