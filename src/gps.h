/*
The MIT License (MIT)

Copyright (c) 2015 Jacob McGladdery

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

/**
 * @file gps.h
 * @brief The GPS library interface file.
 *
 * This is the interface header file for the GPS library. It defines functions
 * and data structures for extracting time-position-velocity (TPV) reports
 * from NMEA 0183 sentences. This library also provides an encoder function in
 * case the user wishes to send a valid NMEA sentence to the GPS device. Note
 * that in order to be more compatible with embedded systems, this library
 * does not make use of floating point data types. Instead, all parsed data is
 * stored as an integer value, in the correct units for the type of data being
 * stored, and raised to some power of ten. This way, the fractional component
 * of the data being stored is still preserved and is accessible by dividing
 * by some scale factor.
 */

#ifndef _GPS_H_
#define _GPS_H_

#include <stdint.h>

/* String sizes */
#define GPS_TIME_STRING_SIZE (25) /**< The size of the timestamp string including the NUL terminator */
#define GPS_TALKER_ID_SIZE   (3)  /**< The size of the talker ID string including the NUL terminator */

/* Data markers */
#define GPS_INVALID_VALUE (0x7FFFFFFF) /**< Used to indicate a value is invalid or unset */

/* Scale factors */
#define GPS_VALUE_FACTOR   (1000)    /**< The scale factor for use with most data values */
#define GPS_LAT_LON_FACTOR (1000000) /**< The scale factor for use with latitude and longitude values */

/* Result codes */
#define GPS_OK                (0) /**< Indicates no error has occured */
#define GPS_ERROR_HEAD        (1) /**< The header is missing from the NMEA sentence */
#define GPS_ERROR_FOOT        (2) /**< The footer is missing from the NMEA sentence */
#define GPS_ERROR_CHECKSUM    (3) /**< The checksum did not match the computer value */
#define GPS_ERROR_TRUNCATED   (4) /**< The input NMEA sentence is incomplete */
#define GPS_ERROR_UNSUPPORTED (5) /**< An unsupported operation was requested */

/**
 * @brief NMEA fix mode.
 */
enum gps_mode
{
    GPS_MODE_UNKNOWN, /**< Have not yet received a message containing fix information */
    GPS_MODE_NO_FIX,  /**< No fix with GPS satellites */
    GPS_MODE_2D_FIX,  /**< Valid fix but altitude is a pseudo value */
    GPS_MODE_3D_FIX   /**< Valid fix including a good altitude value */
};

/**
 * @brief Time-Position-Velocity (TPV) data.
 */
struct gps_tpv
{
    enum gps_mode mode; /**< NMEA fix mode */
    int32_t altitude;   /**< Altitude in meters times 10e3 */
    int32_t latitude;   /**< Latitude in degrees times 10e6 */
    int32_t longitude;  /**< Longitude in degrees times 10e6 */
    int32_t track;      /**< Course over ground, degrees from true north times 10e3 */
    int32_t speed;      /**< Speed over ground, meters per second times 10e3 */
    char time[GPS_TIME_STRING_SIZE];    /**< Time stamp in ISO8601 format, UTC */
    char talker_id[GPS_TALKER_ID_SIZE]; /**< Device talker ID */
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes a TPV data structure to a known state.
 *
 * Sets all members of @p tpv to default values. For gps_tpv.mode this is
 * GPS_MODE_UNKNOWN. For gps_tpv.altitude, gps_tpv.latitude,
 * gps_tpv.longitude, gps_tpv.track, and gps_tpv.speed this is
 * GPS_INVALID_VALUE. For gps_tpv.time this is an ISO8601 string containing
 * all zeros. For gps_tpv.talker_id this is a null string.
 *
 * @param[out] tpv The data structure to initialize.
 *
 * @pre The pointer @p tpv must not be NULL.
 * @post The data in @p tpv is modified.
 */
void gps_init_tpv(struct gps_tpv *tpv);

/**
 * @brief Encodes a NMEA sentence.
 *
 * Encodes the string @p message in a NMEA sentence. The string @p message
 * must only contain the comma separated fields which make up the NMEA
 * sentence. It must not contain the message header, checksum, or footer as
 * those will be what this function adds. The buffer @p destination must
 * always be larger than the size of @p message. Given the current
 * implementation, @p destination will always be 7 characters longer than
 * @p message.
 *
 * @param[out] destination The buffer where the encoded string will be stored.
 * @param[in] message The string to encode in the NMEA sentence.
 * @return A pointer to the end of the string @p destination.
 *
 * @pre The pointer @p destination must not be NULL.
 * @pre The pointer @p message must not be NULL.
 * @post The data in @p destination is modified.
 */
char *gps_encode(char *destination, const char *message);

/**
 * @brief Decodes a NMEA sentence.
 *
 * Decodes a full NMEA sentence and stores the processed information in
 * @p tpv. If a value could not be successfully decoded, but the rest of the
 * sentence is valid, then the failed value will be stored as either
 * GPS_INVALID_VALUE or GPS_MODE_UNKNOWN depending on the type of value which
 * failed to decode.
 *
 * @param[out] tpv The data structure where the decoded values will be stored.
 * @param[in,out] nmea The NMEA sentence to decode.
 * @return A result code indicating what happened after the NMEA sentence was
 *         decoded.
 * @retval GPS_OK If and only if the NMEA sentence was valid. Any other return
 *         code indicates some error.
 *
 * @pre The pointer @p tpv must not be NULL.
 * @pre The string @p nmea must be NUL terminated.
 * @post The data in @p tpv is modified.
 * @post The data in @p nmea is modified.
 */
int gps_decode(struct gps_tpv *tpv, char *nmea);

/**
 * @brief Produces an error string.
 *
 * Produces a pointer to the error string which maps to the error code @p e.
 *
 * @param[in] e The error code.
 * @return A string which tells in detail what the code represents.
 */
const char *gps_error_string(const int e);

#ifdef __cplusplus
}
#endif

#endif /* _GPS_H_ */
