/* Benchmark
 *
 * Test how long it takes to decode one of each type of sentence which the
 * GPS library supports. Note that if you debug this and notice that the year
 * is being set to 2094, don't worry. These test NMEA strings are from the
 * 1990s and the GPS library is designed to only support NMEA dates from Y2K
 * on.
 *
 * Test NMEA sentences taken from:
 * http://www.gpsinformation.org/dale/nmea.htm
 */

#include "gps.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_NMEA_STRINGS (6)
#define NMEA_STRING_SIZE (128)

int main(void)
{
    struct gps_tpv tpv;
    struct timespec start_ts, end_ts;
    char nmea[NUM_NMEA_STRINGS][NMEA_STRING_SIZE];
    unsigned int i;

    /* Setup */
    gps_init_tpv(&tpv);
    strncpy(nmea[0], "$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47\r\n", NMEA_STRING_SIZE);
    strncpy(nmea[1], "$GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39\r\n", NMEA_STRING_SIZE);
    strncpy(nmea[2], "$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A\r\n", NMEA_STRING_SIZE);
    strncpy(nmea[3], "$GPGLL,4916.45,N,12311.12,W,225444,A,*1D\r\n", NMEA_STRING_SIZE);
    strncpy(nmea[4], "$GPVTG,054.7,T,034.4,M,005.5,N,010.2,K*48\r\n", NMEA_STRING_SIZE);
    strncpy(nmea[5], "$GPZDA,201530.00,04,07,2002,00,00*60\r\n", NMEA_STRING_SIZE);

    /* Start timing and run */
    if (clock_gettime(CLOCK_MONOTONIC, &start_ts) < 0)
    {
        perror("clock_gettime start");
        return errno;
    }

    for (i = 0; i < NUM_NMEA_STRINGS; ++i)
    {
        gps_decode(&tpv, nmea[i]);
    }

    /* End timing and report */
    if (clock_gettime(CLOCK_MONOTONIC, &end_ts) < 0)
    {
        perror("clock_gettime end");
        return errno;
    }

    printf("Time taken to decode %d NMEA sentences:\n"
           "  %lds\n"
           "  %ldns\n",
           NUM_NMEA_STRINGS,
           (long)(end_ts.tv_sec - start_ts.tv_sec),
           end_ts.tv_nsec - start_ts.tv_nsec);

    return EXIT_SUCCESS;
}
