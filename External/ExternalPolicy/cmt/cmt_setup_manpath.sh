#!/bin/sh
#+
#	Script to setup MANPATH to eliminate bad entries and to move all the default
# 	entries (that begin with /usr) to the end
#
#	Author:-
#	David Quarrie
#-
[ -n "${ATLAS_NO_SETUP_MANPATH:-}" ] && return

s_out=
export MANPATH=`manpath`
if [ ! -z "${MANPATH}" ]; then
    s_dirs=`\echo $MANPATH | \sed 's/:/ /g'`
    s_usr=
    for s_dir in $s_dirs; do
	    s_try=`echo $s_dir | grep "^/usr"`
	    if [ -z $s_try ]; then
            s_out=${s_out}:$s_dir 
        else
	        s_usr=${s_usr}:$s_dir 
	    fi
    done
    s_out=${s_out}:${s_usr}
    s_out=`\echo $s_out | \sed 's/^://'`
    s_out=`\echo $s_out | \sed 's/::/:/'`
	s_dirs=
	s_dir=
	s_usr=
	s_try=
fi

export MANPATH=${s_out}
s_out=
