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

/* The following sites were used to help calculate values used in these tests:
 *   http://www.hhhh.org/wiml/proj/nmeaxor.html
 *   http://andrew.hedges.name/experiments/convert_lat_long/
 *   http://www.catb.org/gpsd/NMEA.htm
 */

#include "gps.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#define SIZEOF_STRING(s)    (sizeof(s) - 1)
#define NMEA_OVERHEAD_SIZE  SIZEOF_STRING("$*00\r\n")

static void test_init_tpv(void **state)
{
    (void)state;
    struct gps_tpv tpv;

    gps_init_tpv(&tpv);
    assert_true(GPS_MODE_UNKNOWN == tpv.mode);
    assert_int_equal(tpv.altitude, GPS_INVALID_VALUE);
    assert_int_equal(tpv.latitude, GPS_INVALID_VALUE);
    assert_int_equal(tpv.longitude, GPS_INVALID_VALUE);
    assert_int_equal(tpv.track, GPS_INVALID_VALUE);
    assert_int_equal(tpv.speed, GPS_INVALID_VALUE);
    assert_string_equal(tpv.time, "0000-00-00T00:00:00.000Z");
    assert_string_equal(tpv.talker_id, "\0");
}

static void test_encode_valid_message(void **state)
{
    (void)state;
    const char msg[] = "PMTK251,38400";
    char buf[32];
    char *end;

    end = gps_encode(buf, msg);
    assert_non_null(end);
    assert_ptr_equal(buf + SIZEOF_STRING(msg) + NMEA_OVERHEAD_SIZE, end);
    assert_string_equal(buf, "$PMTK251,38400*27\r\n");
}

static void test_encode_empty_message(void **state)
{
    (void)state;
    const char msg[] = "";
    char buf[32];
    char *end;

    end = gps_encode(buf, msg);
    assert_non_null(end);
    assert_ptr_equal(buf + NMEA_OVERHEAD_SIZE, end);
    assert_string_equal(buf, "$*00\r\n");
}

static void test_decode_valid_gga_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPGGA,172814.0,3723.46587704,N,12202.26957864,W,2,6,1.2,18.893,M,-25.669,M,2.0,0031*4F\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_OK);
    assert_string_equal(tpv.talker_id, "GP");
    assert_string_equal(tpv.time, "0000-00-00T17:28:14.000Z");
    assert_int_equal(tpv.latitude, 37391097);
    assert_int_equal(tpv.longitude, -122037826);
    assert_int_equal(tpv.altitude, 18893);
}

static void test_decode_valid_gll_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPGLL,3704.229,N,07647.090,W,153030.311,A*23\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_OK);
    assert_string_equal(tpv.talker_id, "GP");
    assert_string_equal(tpv.time, "0000-00-00T15:30:30.311Z");
    assert_int_equal(tpv.latitude, 37070483);
    assert_int_equal(tpv.longitude, -76784833);
}

static void test_decode_valid_gsa_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPGSA,A,3,01,04,07,16,20,,,,,,,,3.6,2.2,2.7*35\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_OK);
    assert_string_equal(tpv.talker_id, "GP");
    assert_true(GPS_MODE_3D_FIX == tpv.mode);
}

static void test_decode_valid_rmc_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPRMC,023044,A,3907.3840,N,12102.4692,W,0.0,156.1,131102,15.3,E,A*37\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_OK);
    assert_string_equal(tpv.talker_id, "GP");
    assert_string_equal(tpv.time, "2002-11-13T02:30:44.000Z");
    assert_int_equal(tpv.latitude, 39123066);
    assert_int_equal(tpv.longitude, -121041153);
    assert_int_equal(tpv.track, 156100);
    assert_int_equal(tpv.speed, 0);
}

static void test_decode_valid_vtg_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPVTG,176.90,T,,M,3.68,N,6.81,K,A*36\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_OK);
    assert_string_equal(tpv.talker_id, "GP");
    assert_int_equal(tpv.track, 176900);
    assert_int_equal(tpv.speed, 1891);
}

static void test_decode_valid_zda_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPZDA,050306,29,10,2003,,*43\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_OK);
    assert_string_equal(tpv.talker_id, "GP");
    assert_string_equal(tpv.time, "2003-10-29T05:03:06.000Z");
}

static void test_decode_empty_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_ERROR_HEAD);
}

static void test_decode_invalid_header(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "?GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_ERROR_HEAD);
}

static void test_decode_invalid_footer(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75??";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_ERROR_FOOT);
}

static void test_decode_invalid_checksum(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*??\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_ERROR_CHECKSUM);
}

static void test_decode_mismatch_checksum(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*FF\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_ERROR_CHECKSUM);
}

static void test_decode_truncated_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$GPGGA,092751.000,5321.6802,N,0063";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_ERROR_TRUNCATED);
}

static void test_decode_unsupported_message(void **state)
{
    (void)state;
    struct gps_tpv tpv;
    int result;
    char nmea[] = "$PGRME,15.0,M,22.5,M,15.0,M*1B\r\n";

    gps_init_tpv(&tpv);
    result = gps_decode(&tpv, nmea);
    assert_int_equal(result, GPS_ERROR_UNSUPPORTED);
}

static void test_error_string_ok(void **state)
{
    (void)state;
    const char *msg;

    msg = gps_error_string(GPS_OK);
    assert_string_equal(msg, "No error while parsing NMEA");
}

static void test_error_string_out_of_range(void **state)
{
    (void)state;
    const char *msg;

    msg = gps_error_string(9999);
    assert_string_equal(msg, "Unknown error");
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_init_tpv),
        cmocka_unit_test(test_encode_valid_message),
        cmocka_unit_test(test_encode_empty_message),
        cmocka_unit_test(test_decode_valid_gga_message),
        cmocka_unit_test(test_decode_valid_gll_message),
        cmocka_unit_test(test_decode_valid_gsa_message),
        cmocka_unit_test(test_decode_valid_rmc_message),
        cmocka_unit_test(test_decode_valid_vtg_message),
        cmocka_unit_test(test_decode_valid_zda_message),
        cmocka_unit_test(test_decode_empty_message),
        cmocka_unit_test(test_decode_invalid_header),
        cmocka_unit_test(test_decode_invalid_footer),
        cmocka_unit_test(test_decode_invalid_checksum),
        cmocka_unit_test(test_decode_mismatch_checksum),
        cmocka_unit_test(test_decode_truncated_message),
        cmocka_unit_test(test_decode_unsupported_message),
        cmocka_unit_test(test_error_string_ok),
        cmocka_unit_test(test_error_string_out_of_range)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
