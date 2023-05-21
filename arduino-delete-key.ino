//-----------------------------------------------------------------------------
// Copyright (c) 2023 LucAce
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------
//
//   Adruino Delete Key Button
//   Filename: delete_key.ino
//
//   Functional Description:
//
//       Issue a DEL keyboard button press when an input pin goes low.
//
//   Board Compatibility:
//
//      - Adafruit QT Py RP2040 (https://www.adafruit.com/product/4900)
//
//   Arduino Library Dependencies:
//
//      - Keyboard (Included with Arduino)
//      - Adafruit NeoPixel (Tested with version 1.11.0)
//
//   Wiring:
//
//      - Connect pin A2 to one side of a NO (Normally Open) switch/button
//        and connect the other side of the switch/button to Ground.  When
//        the switch/button is pressed A2 will become ground and seen as
//        low.  When A2 is seen low a single DEL keypress will be sent to
//        the connected system over USB.
//
//-----------------------------------------------------------------------------

#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>

// Pin to monitor for changes
#define BUTTON_INPUT_PIN  A2

// Adafruit NeoPixel
Adafruit_NeoPixel pixels(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// Time in milliseconds to use for debouncing
#define DEBOUNCE_DELAY    50

// Time in milliseconds to hold the pressed key (DEL)
#define BUTTON_HOLD_TIME  100

// States of the button (Button is Active-Low)
#define BUTTON_PRESSED    LOW
#define BUTTON_RELEASED   HIGH

// Enable the delete key instead of a test word
#define ENABLE_DELETE     1

// Global Attributes
int           keyboard_enabled       = 0;
int           button_state_current   = BUTTON_RELEASED;
int           button_state_previous  = BUTTON_RELEASED;
unsigned long state_change_timestamp = 0;


//*****************************************************************************
// Function: setup
// Configure the device.
//*****************************************************************************
void setup() {
  // Configure the monitored pin to be an input with a pullup
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);

  // Enable NeoPixel Power pin
  #if defined(NEOPIXEL_POWER)
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  #endif

  // Enable the keywords library
  Keyboard.begin();

  // Enable NeoPixel library
  pixels.begin();

  // Set a low NeoPixel brightness
  pixels.setBrightness(10);

  // Set NeoPixel color to Magenta
  pixels.fill(0xFF00FF);
  pixels.show();

  // Wait before starting
  delay(5000);

  // Capture the current time (in milliseconds)
  state_change_timestamp = millis();
}


//*****************************************************************************
// Function: loop
// Main processing loop.
//*****************************************************************************
void loop() {
  // Read the input pin's state
  button_state_current = digitalRead(BUTTON_INPUT_PIN);

  // Set NeoPixel color to Green
  pixels.fill(0x00FF00);
  pixels.show();

  // Record the time and enable the keyboard logic if there was a change
  if (button_state_current != button_state_previous) {
    state_change_timestamp = millis();
    keyboard_enabled       = 1;
  }

  // If currently pressed and the debounce time has been exceeded
  if ( (keyboard_enabled     == 1)              &&
       (button_state_current == BUTTON_PRESSED) &&
      ((millis() - state_change_timestamp) > DEBOUNCE_DELAY) ) {

    // Set NeoPixel color to Red
    pixels.fill(0xFF0000);
    pixels.show();

    // Press the DEL key
    #if ENABLE_DELETE != 1
    Keyboard.println("TEST WORD");
    #else
    Keyboard.press(KEY_DELETE);
    #endif

    // Hold the pressed key
    delay(BUTTON_HOLD_TIME);

    // Release the key
    Keyboard.releaseAll();

    // Disable issuing of another key press until the pin changes
    keyboard_enabled = 0;
  }

  // Update previous state
  button_state_previous = button_state_current;
}
