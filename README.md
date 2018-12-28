# KeyBow4JoyBonnet
Adaptation of KeyBow main program for JoyBonnet hardware

# Responsability

Please do not contact Pimoroni support for problem you have with this software or Keybow software used without their hardware. Pimoroni is not responsible for the possible bug I have introduced by forking their code. Also Adafruit is not responsible for the C code I have written reading their JoyBonnet Python code.

# Original Keybow product

Keybow is a product by Pimoroni that you can aquire from [here](https://shop.pimoroni.com/products/keybow)

Keybow is an easy-to-build, solderless, DIY mini mechanical keyboard, Raspberry Pi-powered, with twelve illuminated keys, hot-swap clicky or linear switches, clear keycaps, and awesome customisable layouts and macros. It's the ultimate macro pad.

This Keybow OS is RAM-disk-based and built upon a stripped-down Raspbian, with C bindings that setup and run the USB HID, and a series of Lua-based scripts to customise the key layouts and lighting.

# Testing Keybow software without Keybow hardware

If you own a JoyBonnet from Adafruit, you can test the concept of Keybow without owning the keybow hardware. This works out of the box with the official software from Pimoroni as the eight physical button of JoyBonnet all use GPIO that are also used by Keybow.

I guess you will be missing a lot of the pleasure from the Keybow:
(1) You only have 8 working button and not 12.
(2) You don't have the rainbow color from the Keybow, actually you have no LED.
(3) You don't have the click from a mechanical keyboard.

This piece of software (KeyBow4JoyBonnet) fix the first problem by replacing the missing button by the analog joystic four main direction. You still have no LED, no real keyboard key.

# Adapting Keybow distribution for JoyBonnet

Just follow the official Keybow micro-SD card installation. Once you have everything setup, you can replace the "keybow" binary by a modified version.

# Source code, Copyright, Licence, Credit

Original code is copyrighted 2018 by Pimoroni and under MIT Licence.
Code modification is copyrighted 2018 by David Glaude under the same Licence.

My code is strongly inspired by JoyBonnet Python code and i2c example code:

Credit to Adafruit for Python code of JoyBonnet under MIT Licence.
Credit to Shahrooz Shahparnia (sshahrooz@gmail.com) for i2c example code (Copyright 2013 under GNU GPLv3) of bcm2823 library.

