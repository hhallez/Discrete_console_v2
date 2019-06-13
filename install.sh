#!/bin/sh
SERVICE=Discrete;

if ps ax | grep -v grep | grep $SERVICE > /dev/null
then
    echo "$SERVICE service running, stopping service"
    sudo systemctl stop $SERVICE.service
    sudo systemctl disable $SERVICE.service
    echo "$SERVICE service stopped, updating..."
    sudo cp $SERVICE.service /etc/systemd/system/$SERVICE.service
    sudo systemctl start Discrete.service
    sudo systemctl enable Discrete.service
else
  echo "installing $SERVICE service..."
  sudo cp $SERVICE.service /etc/systemd/system/$SERVICE.service
  sudo systemctl start Discrete.service
  sudo systemctl enable Discrete.service
fi
echo "done"
