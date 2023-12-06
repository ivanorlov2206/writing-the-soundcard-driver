#!/bin/bash

aplay -D hw:CARD=$4,DEV=0 -c $2 -f S16_LE -r 48000 $1
