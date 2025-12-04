#!/bin/bash
set -e
# remove netem on loopback
sudo tc qdisc del dev lo root || true
echo "delay removed"
