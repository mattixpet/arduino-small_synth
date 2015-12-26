# arduino-small_synth

## Usage:
* You could try to recreate the physical synth itself, you would have to go through the code and look at pin numbers and recreate the voltage values for the control. But the code is here.

## Info:
###### I made a small single octave synth using nothing but arduino and a simple high/low output of the arduino to create a square wave sound at note frequencies. It even had an arpeggiator (surprisingly simple to program) with a tempo changer (variable resistor), and 2 more octaves (1 below and 1 above).

* Look at a demo video of the full fletched synth, **demo.mp4**
* I should mention, due to (that's my theory) processing time of the arduino, and nothing done to compensate, the higher notes are not in correct pitch. For example, playing C and G on the keyboard in the middle or top octave would result in an augmented 4th interval instead of a perfect 5th.

###### Written in 2013.
