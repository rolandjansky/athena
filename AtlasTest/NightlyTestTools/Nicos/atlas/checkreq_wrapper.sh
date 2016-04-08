#!/bin/sh
command=$@
$@; stat=$?
exitvalue=0
echo "-------------------------------------------------------------------"
echo "checkreq exited with code $stat"
if [ "$stat" -eq 3 ]; then
exitvalue=$stat
echo "this triggers checkreq WARNING alarm"
fi
echo "-------------------------------------------------------------------"
exit $exitvalue
