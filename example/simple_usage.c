/* Simple Usage Example
 *
 * Takes a NMEA string as input and displays information about what could be
 * decoded from the string.
 */

#include "gps.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PROGNAME "simple-usage"

void print_tpv_value(const char *name, const char *format, const int32_t value, const int32_t scale_factor)
{
    printf("%s: ", name);
    if (GPS_INVALID_VALUE != value)
    {
        printf(format, (double)value / scale_factor);
    }
    else
    {
        puts("INVALID");
    }
}

int main(int argc, char **argv)
{
    struct gps_tpv tpv;
    int result;
    char str[256];

    if (argc != 2)
    {
        fputs("Usage: " PROGNAME " NMEA\n", stderr);
        return EXIT_FAILURE;
    }

    /* Append CRLF to the user supplied string */
    snprintf(str, sizeof(str), "%s\r\n", argv[1]);

    /* Sets the data to a known state */
    gps_init_tpv(&tpv);

    /* Attempt to decode the user supplied string */
    result = gps_decode(&tpv, str);
    if (result != GPS_OK)
    {
        fprintf(stderr, "Error (%d): %s\n", result, gps_error_string(result));
        return EXIT_FAILURE;
    }

    /* Go through each TPV value and show what information was decoded */
    printf("Talker ID: %s\n", tpv.talker_id);
    printf("Time Stamp: %s\n", tpv.time);
    print_tpv_value("Latitude", "%.6f\n", tpv.latitude, GPS_LAT_LON_FACTOR);
    print_tpv_value("Longitude", "%.6f\n", tpv.longitude, GPS_LAT_LON_FACTOR);
    print_tpv_value("Altitude", "%.3f\n", tpv.altitude, GPS_VALUE_FACTOR);
    print_tpv_value("Track", "%.3f\n", tpv.track, GPS_VALUE_FACTOR);
    print_tpv_value("Speed", "%.3f\n", tpv.speed, GPS_VALUE_FACTOR);

    printf("Mode: ");
    switch (tpv.mode)
    {
    case GPS_MODE_UNKNOWN:
        puts("Unknown");
        break;
    case GPS_MODE_NO_FIX:
        puts("No fix");
        break;
    case GPS_MODE_2D_FIX:
        puts("2D");
        break;
    case GPS_MODE_3D_FIX:
        puts("3D");
        break;
    default:
        break;
    }

    printf("\n");

    return EXIT_SUCCESS;
}
