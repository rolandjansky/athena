#!/bin/bash

#echo disabled ; exit 1

mkdir -p /tmp/$USER/mplcfg
export MPLCONFIGDIR=/tmp/$USER/mplcfg

EC=0

tmpdir=/tmp/$USER/custom_archmon_$RANDOM$RANDOM && \
targetdir=/afs/cern.ch/atlas/project/pmb/archive/custom_webcache/pages_custom && \
targetdir_tmp=${targetdir}__tmp__$RANDOM$RANDOM
targetdir_tmp2=${targetdir_tmp}_2
mkdir $tmpdir && \
source /afs/cern.ch/atlas/project/pmb/rtt-mon/runtime/setup.sh && \
python /afs/cern.ch/atlas/project/pmb/rtt-mon/PmbUtils/cron_scripts/customjobs_analyse/custom_archmon.py $tmpdir $* && \
touch $targetdir_tmp && \
NKB=`du -ks $tmpdir|awk '{print $1}'` && \
TOTKB=`fs lq $targetdir_tmp|tail -1|awk '{print $2}'` && \
USEDKB=`fs lq $targetdir_tmp|tail -1|awk '{print $3}'` && \
if [ $((TOTKB-USEDKB-10000)) -lt $NKB ]; then
    echo "Not enough free space! Aborting!"
    rm -rf $tmpdir
    rm -rf $targetdir_tmp
    exit 1
fi && \
rm $targetdir_tmp && \
cp -r $tmpdir ${targetdir_tmp} && \
if [ -d ${targetdir} ]; then
    mv -f ${targetdir} ${targetdir_tmp2}
fi && \
mv ${targetdir_tmp} ${targetdir}
EC=$?
rm -rf ${targetdir_tmp}
rm -rf ${targetdir_tmp2}
rm -rf $tmpdir
if [ $EC != 0 ]; then
    echo "Problems encountered. Exiting with EC=$EC."
fi
exit $EC
