#!/usr/bin/env bash
## Post-job analysis
## Run this script in the run directory of the partition test

## Prepend script name to all our printouts
shopt -s expand_aliases
alias echo='echo [`basename $0`] '

if [ $# -ne 1 ]; then
   echo "Syntax: `basename $0` <partition name>"
   exit 1
fi

part_name=$1

checkLog() {
    # Check log file $1 with alias $2
    echo "------------------------ Check $2 log file ------------------------"
    if [[ ! -e $1 ]]; then
        echo "Cannot find $2 log file"
        status=2
    else
        check_log.py --errors --config checklogTrigP1Test.conf $1
        status=$?
    fi
    return $status
}

## Extract log files
echo "Extracting log files..."
ls -1t *${part_name}*.tar.gz | head -1 | xargs -t tar xzf

get_files -symlink -data checklogTrigP1Test.conf

## Check MRS messages (ignore any meassages after EXIT transition)
sed '/Transition EXIT/,$d' output.mrs > output.preExit.mrs
checkLog "output.preExit.mrs" "MRS"
mrsstatus=$?

## Check L2PU log file
l2log=`ls -t ${part_name}/L2PU*.out | head -1`
checkLog $l2log "L2PU"
l2status=$?

## Check PT log file
eflog=`ls -t ${part_name}/PT*.out | head -1`
checkLog $eflog "PT"
efstatus=$?

if [ $l2status -eq 0 -a $efstatus -eq 0 ]; then
    exit 0
else
    exit 1
fi
