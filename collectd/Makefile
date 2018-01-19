_PREFIX=/usr/local/sbin

all:
	gcc -O2 -Wall collectd_dht.c -o collectd_dht.o -lwiringPi -lm
	gcc -O2 -Wall bmp180/src/bmp180.o collectd_bmp180.c -o collectd_bmp180.o -lm
	gcc -O2 -Wall tsl2561/src/tsl2561.o collectd_tsl2561.c -o collectd_tsl2561.o
	gcc -O2 -Wall collectd_max44009.c -o collectd_max44009.o -lm

install:
	install -Dsm4755 collectd_dht.o ${_PREFIX}/collectd_dht
	install -Dsm4755 collectd_bmp180.o ${_PREFIX}/collectd_bmp180
	install -Dsm4755 collectd_tsl2561.o ${_PREFIX}/collectd_tsl2561
	install -Dsm4755 collectd_max44009.o ${_PREFIX}/collectd_max44009

clean:
	rm *.o > /dev/null 2>&1 &
