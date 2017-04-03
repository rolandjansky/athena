#!/usr/bin/env bash

pwd

echo "args: $*"

BASEDIR=/afs/cern.ch/user/s/sutt/public
 
RUNSCRIPT=$BASEDIR/TIDArdict.sh

cp $RUNSCRIPT .

./TIDArdict.sh $*

