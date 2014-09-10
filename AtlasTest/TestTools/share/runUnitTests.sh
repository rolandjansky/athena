#!/bin/sh
#/** @file runUnitTests.sh
# @brief sh script that cmt broadcasts gmake check and filters out its output
# @param opts options to be passed to cmt (eg -select=Store)
# @author Paolo Calafiura <pcalafiura@lbl.gov> - ATLAS Collaboration.
# $Id: runUnitTests.sh,v 1.1 2003-04-04 01:31:24 calaf Exp $
# **/
#if [ "$#" = 0 ]
#    then
#    opts=-
#else
    opts="$@"
#fi

joblog=gmakecheck.log
cmt bro "$opts" gmake check 2>&1 | tee $joblog | egrep "(post.sh>|Now trying)" 
tail -1 $joblog | grep -q "check ok"
rc=$?
if [ "$rc" = 0 ]
    then 
    echo "OK: gmake check exited normally. Output is in $joblog"
else
    tail $joblog
    echo  "ERROR: gmake check exited abnormally!"

    echo  " Please check ${PWD}/$joblog"
fi
