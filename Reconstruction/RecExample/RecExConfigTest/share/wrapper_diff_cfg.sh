#!/bin/sh

diff-jobo-cfg.py "$@" | tee long.log | sed '1,/^===/ d'
gzip long.log
