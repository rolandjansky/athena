#!/usr/bin/env bash
#
# Do REGTEST between several runs in one log file
#


if [ $# -ne 3 ]; then
   if [ $# -ne 1 ]; then
	   echo "Syntax: `basename $0` LOGFILE"
	   echo "Syntax: `basename $0` LOGFILE  RUN_REFERENCE RUN_TEST"
	   exit 1
   fi
fi


log=$1
runref="$2"
runtest="$3"

runlogs=`splitlog.py -b '.*Psc prepareForRun finished.*' -e '.*Psc stopRun finished.*' $log`
nruns=`echo $runlogs | wc -w`

echo "Doing REGTEST between several runs in one log file"
echo
echo "Log file: $log"
echo "Number of runs: $nruns"

status=0
first=""
logref=""
logtest=""
for f in $runlogs; do

    if [ -z $logref ]; then
       if [ -n $runref ]; then
	logref=`echo $f | grep log.$runref`
       fi
    fi
    if [ -z $logtest ]; then
	if [ -n "$runtest" ]; then
	logtest=`echo $f | grep log.$runtest`
	fi
    fi

    grep -a REGTEST $f > $f.regtest
    if [ -z $first ]; then
        first=$f
        continue
    fi
    if [ -z "$runref" ]; then
	echo "*****************************************************************************"
	echo " REGTEST of $first and $f"
	echo "*****************************************************************************"
	diff -b $first.regtest $f.regtest
	status=$(($status+$?))
    fi	
done

if [ -n "$runtest" ]; then
    echo "*****************************************************************************"
    echo " REGTEST of $logref and $logtest"
    echo "*****************************************************************************"
    diff -b $logref.regtest $logtest.regtest
    status=$(($status+$?))
fi

# Delete run-wise log files
rm -f $runlogs

echo
if [ $status -eq 0 ]; then
    echo "No differences found"
fi
echo "Final return code: $status"
exit $status
