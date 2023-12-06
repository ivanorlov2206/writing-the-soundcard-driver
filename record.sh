#!/bin/bash

arecord -D hw:CARD=$4,DEV=0 -c $3 -i -f S16_LE -r 48000 --duration=$2 $1
