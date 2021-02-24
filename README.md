# DWIN DGUS II Lcd Display Driver

Docs: https://ginge.github.io/libdgus_ii/

A C library to control the serially interfaced graphical LCD display from DWIN China.

This range of displays allow a desktop based designer to edit and construct graphical interfaces.
The display functionality is quite advanced allowing drag and drop display of variables, button presses and other io capability. In addition to the simple mode, you can draw primitives such as pixels lines, squares circles etc directly over the serial protocol.

This library allows control over the common functions of the DWIN II LCD Display.

## Features

* Update variables by address
* Text mode shortcuts and utilities
* Control over SP mode and dynamic control over widget control parameters
* Curve display and control for up to 8 channel
* Blocking / non-blockling read of variables
* Music playback control (not streaming mode) and Volume
* Brightness and standby mode control


## Feature in the works

* Primitive drawing control utilities
* Serial Flash write support for arbitary storage
* Serial-upload of flash for existing controls
* Better unit testing

## Features i'm considering

* A new desktop designer, web based.
* Instead of addressing the VAR arbitary memory by raw address, write a compiler.
* Super low level control over PWM, IO and OS mode

## Why use this screen?

Up to you really, but this came attached to the Creality Ender 6 3d Printer. Quite frankly the interface sucked. After reading the datasheet, i'm quietly impressed with the capabilties of this seemingly old LCD. It is a perfect compliment to a robotics project.

## Setup

Add c files and headers to your project.

Compile. Fix my bugs. When you find them please open an issue. (Endianness issues abound!)

## Example

Contained is main.c example code. This is entirely geared to my personal setup. Use as you wish

```c
// init the library with our callbacks
dgus_init(_serial_bytes_available_callback, _serial_recv_byte_callback, _serial_send_data_callback, _serial_recv_packet_callback);

// set the first page
dgus_set_page(2);

//update a var attached to a text control
dgus_set_text_padded(0x6000, "Hi. I'm padded with spaces", 32);

// do a partial update of the text
dgus_set_text(0x6001, "! "); // Hi. I'm padded with spaces => Hi! I'm...
```

## Arduino

TODO but tl;dr

```c
dgus_init(SERIAL1.available, SERIAL1.recv, SERIAL1.write, myapp_packet_received);

```
