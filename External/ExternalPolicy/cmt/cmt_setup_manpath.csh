#!/bin/csh
#+
#	Script to setup MANPATH to eliminate bad entries and to move all the default
# 	entries (that begin with /usr) to the end
#
#	Author:-
#	David Quarrie
#-
if ( ${?ATLAS_NO_SETUP_MANPATH} ) then
    if ( "${ATLAS_NO_SETUP_MANPATH}" != "" ) then
	exit
    endif
endif

set s_out=
setenv MANPATH `manpath`
if ( ! -z "${MANPATH}" ) then
    set s_dirs=`\echo $MANPATH | \sed 's/:/ /g'`
	set s_usr=
    foreach s_dir ( $s_dirs )
    	set s_try=`\echo $s_dir | \grep "^/usr"`
    	if ( "$s_try" == "" ) then
        	set s_out=${s_out}:$s_dir 
    	else
        	set s_usr=${s_usr}:$s_dir 
    	endif
    end
    set s_out=${s_out}:${s_usr}
    set s_out=`\echo $s_out | \sed 's/^://'`
    set s_out=`\echo $s_out | \sed 's/::/:/'`
	unset s_dirs
	unset s_dir
	unset s_usr
	unset s_try
endif

setenv MANPATH ${s_out}
unset s_out
