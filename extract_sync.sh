#!/bin/sh

gst-launch-1.0 -q filesrc location=/dev/stdin ! tsdemux ! meta/x-klv ! filesink location=/dev/stdout

