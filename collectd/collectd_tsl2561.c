/*
 * A simple collectd exec plugin that reads data from TSL2561
 * and similiar sensors.
 *
 * To use it, first `cd` into 'tsl2561/src' and `make`, then compile
 * this plugin with:
 *   gcc -Wall tsl2561/src/tsl2561.o collectd_tsl2561.c -o collectd_tsl2561
 *
 * Remember to add following configs to your types.db:
 * tsl_lux                value:GAUGE:0:U
 * */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "tsl2561/src/tsl2561.h"

#define TSL_I2C_DEV "/dev/i2c-0"
#define TSL_I2C_PIN 0x39

void read_tsl_data(char *hostname, int interval){
    char *i2c_device = TSL_I2C_DEV;
    int address = TSL_I2C_PIN;

    void *tsl = tsl2561_init(address, i2c_device);

    tsl2561_enable_autogain(tsl);
    tsl2561_set_integration_time(tsl, TSL2561_INTEGRATION_TIME_13MS);

    /* Flush stdout buffer:
     * This needs to be done before *anything* is written to STDOUT! */
    int status = setvbuf (stdout,
         /* buf  = */ NULL,
         /* mode = */ _IONBF, /* unbuffered */
         /* size = */ 0);
    if (status != 0){
        perror ("Error in setvbuf(): fail to flush stdout buffer.");
        exit (1);
    }

    if(tsl != NULL){
        long lux = tsl2561_lux(tsl);
        if (lux < 800){ // force hight gain in dark
            tsl2561_disable_autogain(tsl);
            tsl2561_set_timing(tsl, TSL2561_INTEGRATION_TIME_402MS, TSL2561_GAIN_16X);
            lux = tsl2561_lux(tsl);
        }

        printf("PUTVAL \"%s/exec-tsl/gauge-tsl_lux\" interval=%d N:%ld\n", hostname, interval, lux);

        tsl2561_close(tsl);
        i2c_device = NULL;
    }
}

int main(int argc, char *argv[]){
    char *hostname = getenv("COLLECTD_HOSTNAME");
    int interval = atoi(getenv("COLLECTD_INTERVAL"));

    while (1){
        read_tsl_data(hostname, interval);
        sleep(interval); /* interval is in seconds, wait before next read */
    }
    return 0;
}
