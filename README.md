# DISCRETE monitoring service

This firmware is to be executed on a Raspberry Pi with a MikroBUS hat attached.
The following click boards are to be clicked on:

* Wheather Click, based on BME280 (Bosch Sensortec)
* Tem&Hum2 Click, based on Si7034 (Silicon Labs)
* Accel5 Click, based on the BMA400
* AirQuality5 Click, based on the MICS6814 (SGX Sensortech) or the own made MICS6814 breakout boards


## Dependencies

* The QT core library needs to be included

### TODO steps to install QT Core Library

## Installation

* Clone the repo to a raspberry pi

```
   git clone https://github.com/hhallez/Discrete_console_v2.git
```

* Adapt the file `Distrete.service`

Alter the appropriate paths after `ExecStart` and `WorkingDirectory`

    * `ExecStart=<absolute path to Discrete firmware>/bin/Discrete_console`
    * `WorkingDirectory=<absolute path to Discrete firmware>/bin/``

* Run the install script as superuser

```
   sudo ./install.sh
```

The script should install the service, which will let the raspberry pi sample the sensors at startup.


## Configuration

The configuration is done at a configuration file `settings.conf`.

The following example settings sets the sampling period at 25 microseconds and sets the configuration for an influxDB type database.  It also assigns tags to the measurements, which then can be used to extract the date from the server.

```
;Delay in milliseconds (sampling delay)
delay=25

;Influx settings
server_address=IP-Address of server
port=8086
database=discrete_test
username=login
password=passwd


;Location settings
location=Biezenheem
bed=Bed 1

;Sensors settings
;version of gas sensor v1 = original mics, v2 is custom made mics
gas_sensor=v2
```
