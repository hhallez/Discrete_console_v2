#include <QCoreApplication>
#include <QThread>
#include <QSettings>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <QtDebug>
#include <QThread>
#include <QCoreApplication>
#include "influxdb.h"
using namespace std;
#include <iostream>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "drivers/peripheral_controller.h"
#define DELAY_SECONDS 1

int delay = 1000;
QString server_address = "127.0.0.1";
int port = 8086;
QString database = "";
QString user = "";
QString password = "";
QString location = "";
QString bed = "";
QString sensor_setting = "";

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    printf("Reading settings from file: %s \r\n", QCoreApplication::applicationDirPath().toLocal8Bit().data());
    QSettings settings(QCoreApplication::applicationDirPath() + "/../settings.conf", QSettings::IniFormat);

    delay = settings.value("delay").toInt();
    server_address = settings.value("server_address").toString();
    port = settings.value("port").toInt();
    database = settings.value("database").toString();
    user = settings.value("username").toString();
    password = settings.value("password").toString();
    location = settings.value("location").toString();
    bed = settings.value("bed").toString();

    sensor_setting = settings.value("gas_sensor").toString();


    printf("Init drivers");
    PER_CTRLR_init_drivers();

    MIKROE_2467_data data1;
    MIKROE_3085_data data2;
    MIKROE_3056_data data3;
    MIKROE_3149_data data4;

    printf("Create influx instance \r\n");
    influxdb_cpp::server_info si(server_address.toStdString(), port, database.toStdString(), user.toStdString(), password.toStdString());
    string resp;
    while (1) {
        QThread::msleep(delay);
        int rslt1 = PCTRL_read_env(&data1);
        int rslt2 = PCTRL_read_env2(&data2);
        int rslt3 = PCTRL_read_air(&data3);
        int rslt4 = PCTRL_read_acc(&data4);
        int ret = influxdb_cpp::builder()
                      .meas("measure")
                      .tag("location", location.toStdString())
                      .tag("bed", bed.toStdString())
                      .field("Temperature", (double)data1.temperature / 100.0, 4)
                      .field("Humidity", (double)(data1.humidity / 1000.0), 5)
                      .field("Pressure", (double)(data1.pressure / 100.0), 6)
                      .field("Temperature2", (double)data2.t / 100.0, 4)
                      .field("Humidity2", (double)data2.rh / 100.0, 4)
                      .field("NH3", data3.NH3)
                      .field("NO2", data3.NO2)
                      .field("CO", data3.CO)
                      .field("AccX", data4.x)
                      .field("AccY", data4.y)
                      .field("AccZ", data4.z)

                      .post_http(si, &resp);
        //  .field("S_C2H5OH", (double)data.C2H5OH,6)
        //.field("S_C3H8", (double)data.C3H8,6)
        //  .field("S_C4H10", (double)data.C4H10,6)
        // .field("S_CH4", (double)data.CH4,6)
        // .field("S_CO", (double)data.CO,6)
        // .field("S_H2", (double)data.H2,6)
        //.field("S_NH3",(double) data.NH3,6)
        // .field("S_NO2", (double)data.NO2,6)
        //           .field("Motion", 0)
        //           .timestamp(now)
        if (ret == 0) {
            printf("Successfully sent data to %s \r\n", server_address.toStdString().c_str());
            printf("CO=%d NH3=%d NO2=%d \r\n", data3.CO, data3.NH3, data3.NO2);
            printf("T=%d H=%d P=%d \r\n", data1.temperature, data1.humidity, data1.pressure);
            printf("X=%d Y=%d Z=%d \r\n", data4.x, data4.y, data4.z);
            printf("T=%d H=%d \r\n", data2.t, data2.rh);
        }
        else {
            printf("Failed to send data!!! error: %d", ret);
        }
        fflush(stdout);
    }

    qDebug() << "Hello world2!";

    return a.exec();
}
