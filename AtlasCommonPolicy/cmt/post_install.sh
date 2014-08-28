#!/bin/sh
#+
#	Script to execute post-installation commands
#
#	Author:-
#	David Quarrie
#-

if [ ! -z "$1" ]; then
	if [ -f $1 ]; then
		sh $1
	fi
fi
