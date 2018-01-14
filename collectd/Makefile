all:
	gcc -O2 -Wall collectd_dht.c -o collectd_dht -lwiringPi -lm
	gcc -O2 -Wall bmp180/src/bmp180.o collectd_bmp180.c -o collectd_bmp180 -lm
	gcc -O2 -Wall tsl2561/src/tsl2561.o collectd_tsl2561.c -o collectd_tsl2561

clean:
	rm collectd_dht collectd_bmp180 collectd_tsl2561 > /dev/null 2>&1 &
