#!/bin/sh
#+
#	Script to slim PATH of entries that doesn't exist, aren't
#	directories, or are empty directories.
#
#	Author:-
#	David Quarrie
#-


s_out=

if [ ! -z "${PATH}" ]; then
    s_dirs=`\echo $PATH | \sed 's/:/ /g'`
    for s_dir in $s_dirs; do
        if [ $s_dir != "." ]; then
            if [ -d $s_dir ]; then
                s_files=`\ls $s_dir | \wc -w`
                if [ ${s_files} -gt 0 ]; then
                    s_out=${s_out}:$s_dir 
                fi
            fi
        else
            s_out=${s_out}:$s_dir 
        fi
    done
    s_out=`\echo $s_out | \sed 's/^://'`
fi

export PATH=${s_out}
s_dir=
s_dirs=
s_files=
s_out=
