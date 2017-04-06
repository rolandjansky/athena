#!/bin/bash

if [ -f /etc/bashrc ]; then
        . /etc/bashrc
fi

source /afs/cern.ch/sw/lcg/external/gcc/4.8/x86_64-slc6/setup.sh
alias eos='/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select'

missing_list_file="/afs/cern.ch/user/a/atlidali/CalibrationLoop2016/IDAlignDBTools/log/missing_list.txt"
project=/eos/atlas/atlastier0/rucio/data16_13TeV

timestamp=`date +%s`

timestamp_begin=`echo ${timestamp} - 86400 \* 2 | bc`

date_tag=`date -d@${timestamp_begin} +"%b %d"`

runlist_ref=()
for run in `eos ls -l ${project}/express_express | grep -A100 "${date_tag}" | awk '{print $9;}'`
do
    runlist_ref+=(${run})
done

runlist=()
for run in `eos ls -l ${project}/calibration_IDTracks | grep -A100 "${date_tag}" | awk '{print $9;}'`
do
    runlist+=(${run})
done


missing_runlist=()
for run1 in ${runlist_ref[@]}; do
    flag=false
    for run2 in ${runlist[@]}; do
        if [ $run1 == $run2 ]; then
            flag=true
        fi
    done
    
    if [ $flag == false ]; then
	if [ `cat ${missing_list_file} | grep ${run1} | wc -l` == 0 ]; then
            echo $run1 >> ${missing_list_file}
	    ./send_missing_calibration_IDTracks_email.py ${run1}
	fi
    fi
done

