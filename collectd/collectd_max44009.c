// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// MAX44009
// This code is designed to work with the MAX44009_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <math.h>

float get_lux(void){
    // Create I2C bus
    int file;
    char *bus = "/dev/i2c-0";
    if ((file = open(bus, O_RDWR)) < 0){
        printf("Failed to open the bus. \n");
        exit(1);
    }
    // Get I2C device, MAX44009 I2C address is 0x4A(74)
    ioctl(file, I2C_SLAVE, 0x4A);

    // Select configuration register(0x02)
    // Continuous mode, Integration time = 800 ms(0x40)
    char config[2] = {0};
    config[0] = 0x02;
    config[1] = 0x40;
    write(file, config, 2);
    //sleep(1);

    // Read 2 bytes of data from register(0x03)
    // luminance MSB, luminance LSB
    char reg[1] = {0x03} ;
    write(file, reg, 1);
    char data[2] = {0};
    float luminance;

    if(read(file, data, 2) != 2){
        printf("Erorr : Input/output Erorr \n");
        luminance = 0;
    } else {
        // Convert the data to lux
        int exponent = (data[0] & 0xF0) >> 4;
        int mantissa = ((data[0] & 0x0F) << 4) | (data[1] & 0x0F);
        luminance = pow(2, exponent) * mantissa * 0.045;
    }
    return luminance;
}

int main(void){
    char *hostname = getenv("COLLECTD_HOSTNAME");
    int interval = atoi(getenv("COLLECTD_INTERVAL"));

    while (1){
        float lux = get_lux();
        printf("PUTVAL \"%s/exec-max44009/gauge-max44009_lux\" interval=%d N:%.2f\n", hostname, interval, lux);
        sleep(interval);
    }
    return 0;
}
