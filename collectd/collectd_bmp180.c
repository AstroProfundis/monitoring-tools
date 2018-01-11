/*
 * A simple collectd exec plugin that reads data from BMP180
 * and similiar sensors.
 *
 * To use it, first `cd` into 'bmp180/src' and `make`, then compile
 * this plugin with:
 *   gcc -Wall bmp180/src/bmp180.o collectd_bmp180.c -o collectd_bmp180 -lm
 *
 * Remember to add following configs to your types.db:
 * bmp_temp                value:GAUGE:U:U
 * bmp_pres                value:GAUGE:0:U
 * bmp_alti                value:GAUGE:U:U
 * */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "bmp180/src/bmp180.h"

#define BMP_I2C_DEV "/dev/i2c-1"
#define BMP_I2C_PIN 0x77

void read_bmp_data(char *hostname, int interval){
    char *i2c_device = BMP_I2C_DEV;
    int address = BMP_I2C_PIN;

    void *bmp = bmp180_init(address, i2c_device);

    bmp180_eprom_t eprom;
    bmp180_dump_eprom(bmp, &eprom);
    bmp180_set_oss(bmp, 1);

    if(bmp != NULL){
        float t = bmp180_temperature(bmp);
        long p = bmp180_pressure(bmp);
        float alt = bmp180_altitude(bmp);

        printf("PUTVAL \"%s/exec-bmp/gauge-bmp_temp\" interval=%d N:%.4f\n", hostname, interval, t);
        printf("PUTVAL \"%s/exec-bmp/gauge-bmp_pres\" interval=%d N:%lu\n", hostname, interval, p);
        printf("PUTVAL \"%s/exec-bmp/gauge-bmp_alti\" interval=%d N:%.4f\n", hostname, interval, alt);

        bmp180_close(bmp);
    }
}

int main(int argc, char *argv[]){
    char *hostname = getenv("COLLECTD_HOSTNAME");
    int interval = atoi(getenv("COLLECTD_INTERVAL"));

    while (1){
        read_bmp_data(hostname, interval);
        sleep(interval); /* interval is in seconds, wait before next read */
    }
    return 0;
}
