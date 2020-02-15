#!/bin/bash
m=$1

echo "Sourcing setup scripts"
source /afs/cern.ch/work/j/jdickins/Pixel/LeakgeCurrent/code_Nick/setup_env.sh

python /afs/cern.ch/work/j/jdickins/Pixel/LeakgeCurrent/code_Nick/avgOverLB.py $m
