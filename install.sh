#!/bin/sh
sudo cp Discrete.service /etc/systemd/system/Discrete.service
sudo systemctl start Discrete.service

sudo systemctl enable Discrete.service

