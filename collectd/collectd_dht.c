/*
 * A simple collectd exec plugin that reads data from DHT11/DHT22
 * and similiar sensors.
 * Modified base on example code from:
 *   https://www.piprojects.xyz/temperature-humidity-sensor-orange-pi/
 *
 * Remember to add following configs to your types.db:
 * dht_humi                value:GAUGE:0:U
 * dht_temp                value:GAUGE:U:U
 * dht_error               value:GAUGE:0:U
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>

#define MAX_TIMINGS 85
#define DHT_PIN 7

#define MAX_RETRY 5
#define RETRY_DELAY 200

int data[5] = { 0, 0, 0, 0, 0 };

void read_dht_data(char *hostname, int interval, double retries){
    uint8_t laststate    = HIGH;
    uint8_t counter      = 0;
    uint8_t j            = 0, i;

    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    /* pull pin down for 18 milliseconds */
    pinMode( DHT_PIN, OUTPUT );
    digitalWrite( DHT_PIN, LOW );
    delay( 18 );

    /* prepare to read the pin */
    pinMode( DHT_PIN, INPUT );

    /* detect change and read data */
    for ( i = 0; i < MAX_TIMINGS; i++ ){
        counter = 0;
        while ( digitalRead( DHT_PIN ) == laststate ){
            counter++;
            delayMicroseconds( 1 );
            if ( counter == 255 ){
                break;
            }
        }
        laststate = digitalRead( DHT_PIN );

        if ( counter == 255 )
            break;

        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) ){
            /* shove each bit into the storage bytes */
            data[j / 8] <<= 1;
            if ( counter > 16 )
                data[j / 8] |= 1;
            j++;
        }
    }

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

    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     * print it out if data is good
     */
    if ( (j >= 40) &&
         (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) ){
        float h = (float)((data[0] << 8) + data[1]) / 10;
        if ( h > 100 ){
            h = data[0];    // for DHT11
        }
        float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if ( c > 125 ){
            c = data[2];    // for DHT11
        }
        if ( data[2] & 0x80 ){
            c = -c;
        }
        printf("PUTVAL \"%s/exec-dht/gauge-dht_humi\" interval=%d N:%.2f\n", hostname, interval, h);
        printf("PUTVAL \"%s/exec-dht/gauge-dht_temp\" interval=%d N:%.2f\n", hostname, interval, c);
        printf("PUTVAL \"%s/exec-dht/gauge-dht_error\" interval=%d N:%.2f\n", hostname, interval, retries / MAX_RETRY);
    } else if (retries < MAX_RETRY) {
        /* On reading errors, wait for some time and try again */
        delay(RETRY_DELAY);
        read_dht_data(hostname, interval, ++retries);
    } else {
        printf("PUTVAL \"%s/exec-dht/gauge-dht_error\" interval=%d N:1.00\n", hostname, interval);
    }
}

int main( void ){
    char *hostname = getenv("COLLECTD_HOSTNAME");
    int interval = atoi(getenv("COLLECTD_INTERVAL"));

    if ( wiringPiSetup() == -1 )
        exit( 1 );

    while ( 1 ){
        read_dht_data(hostname, interval, 0.0);
        delay(interval * 1000); /* interval is in seconds, wait before next read */
    }
    return(0);
}
