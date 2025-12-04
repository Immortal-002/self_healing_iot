#!/bin/bash
set -e
# add 150ms delay to loopback
sudo tc qdisc add dev lo root netem delay 150ms
echo "delay added"
