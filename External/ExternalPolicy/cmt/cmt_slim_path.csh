#!/bin/csh
#+
#	Script to slim PATH of entries that doesn't exist, aren't
#	directories, or are empty directories.
#
#	Author:-
#	David Quarrie
#-


set s_out=

if ( ! -z "${PATH}" ) then
    set s_dirs=`\echo $PATH | \sed 's/:/ /g'`
    foreach s_dir ( $s_dirs )
        if ( $s_dir != "." ) then
            if ( -d $s_dir ) then
                set s_files=`ls $s_dir | \wc -w`
                if ( ${s_files} > 0 ) then
                    set s_out=${s_out}:$s_dir 
                endif
            endif
        else
            set s_out=${s_out}:$s_dir 
        endif
    end
    set s_out=`\echo $s_out | \sed 's/^://'`
endif

setenv PATH ${s_out}
unset s_dir
unset s_dirs
unset s_files
unset s_out
