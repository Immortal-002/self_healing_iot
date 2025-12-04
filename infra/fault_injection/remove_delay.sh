#!/bin/bash
set -e
DEV=lo
# Remove netem qdisc if present
sudo tc qdisc del dev $DEV root 2>/dev/null || true
echo "delay removed"
