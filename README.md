# GPS Library

A small GPS library written in C and designed for use in embedded systems. The
goal of this project is to produce a minimal library for both decoding and
encoding NMEA strings quickly on systems which lack a floating point
coprocessor. This library was also designed to be fast and safe. It's fast in
the sense that the decoder functions try and execute as few instructions as
possible when parsing a NMEA sentence. It's safe in the sense that all received
input is validated against some hard-coded regular expression.

## Features

Here are the major features which this library provides.

### Decoder

The decoder is the main feature of this library. It takes in a complete NMEA
sentence, dollar sign, checksum, CRLF, and all, and decodes the sentence into a
time-position-velocity record. You may then check to see if the values stored
are valid and, if they are, you may proceed to make use of the data.

### Encoder

This is a utility function. Use it to compose commands intended for sending to
the GPS device as valid NMEA sentences.

## Installation

This library is composed of only three files; gps.c and gps.h. If you are
developing a bare metal application (i.e. Arduino), then feel free to copy the
source files directly into your project. Just take care not to modify the
copyright statements at the top of each source file.

For users who wish to generate a static or shared libgps library, API
documentation, example programs, or unit tests, then use CMake. The following
Boolean variables can be switched on or off in order to control what gets
built. Their names should be self-explanatory.

* BUILD_EXAMPLES
* BUILD_DOCUMENTATION
* BUILD_UNIT_TESTS
* BUILD_STATIC_LIB

Note that building the documentation requires [Doxygen][1] and building the
unit tests requires [cmocka][2].

## Simple API

The entire API is made up of only four functions. Please refer to the Doxygen
documentation for more detailed information on how to use the API.

* gps_init_tpv()
* gps_encode()
* gps_decode()
* gps_error_string()

## Embedded System Notes

This code was written with embedded systems (and all-around good software
design practices) in mind. There is no use of dynamic memory (i.e. malloc) in
this library. The only external dependancy which this library has is on the C
standard library. All real number data values stored in this library are scaled
up by some factor in order to maintain a certain level of accuracy. This way,
the use of floating point data types is avoided.

[1]: http://www.stack.nl/~dimitri/doxygen/
[2]: https://cmocka.org/
