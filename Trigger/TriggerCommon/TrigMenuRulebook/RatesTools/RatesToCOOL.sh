#! /bin/bash

PYTHONPATH="$PWD:$PYTHONPATH"

cmd="python control/RatesToCOOL.py"


instances=`ps ax | grep "$cmd" | grep -v "grep" | wc -l`


if [ "$instances" == "0" ]; then
	$cmd &> /tmp/$USER/RatesToCOOL.log &
else
	echo "Daemon already running. $instances instances."
fi

