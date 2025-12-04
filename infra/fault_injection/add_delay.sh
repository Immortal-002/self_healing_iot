#!/bin/bash
set -e
# Add or replace a netem qdisc with 150ms delay on loopback.
# Use replace if qdisc exists; otherwise add.

DEV=lo
DELAY=150ms

# Try add; if it fails, try replace
if tc qdisc show dev $DEV | grep -q "netem"; then
    sudo tc qdisc replace dev $DEV root netem delay $DELAY
else
    sudo tc qdisc add dev $DEV root netem delay $DELAY
fi
echo "delay added"
