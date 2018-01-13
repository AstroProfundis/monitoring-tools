all:
	gcc -Wall collectd_dht.c -o collectd_dht -lwiringPi -lm
	gcc -Wall bmp180/src/bmp180.o collectd_bmp180.c -o collectd_bmp180 -lm

clean:
	rm collectd_dht collectd_bmp180 > /dev/null 2>&1 &
