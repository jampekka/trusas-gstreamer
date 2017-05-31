#!/bin/sh
tail -c 0 -f $1 | gst-launch-1.0 filesrc location=/dev/stdin ! tsdemux ! decodebin ! autovideosink
