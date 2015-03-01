#!/usr/bin/env bash
## Run rootcomp.py for partition test
## Run this script in the run directory of the partition test

## Prepend script name to all our printouts
shopt -s expand_aliases
alias log='echo [`basename $0`] '

if [ $# -ne 2 ]; then
   log "Syntax: `basename $0` L2-reference EF-reference"
   exit 1
fi

## Replace /latest/ with /NIGHTLY/latest
## This reproduces the same (confusing) behavior as in trigtest.pl
# this is already done in trigtest.pl
#l2ref=`echo $1 | sed "s#/latest/#/${NICOS_NIGHTLY_NAME}/latest/#"`
#efref=`echo $2 | sed "s#/latest/#/${NICOS_NIGHTLY_NAME}/latest/#"`
l2ref=$1
efref=$2

echo
log "*************************************************************************"
log "*                         Comparing L2 histograms                       *"
log "*************************************************************************"
rootcomp.py $l2ref 'Histogramming-L2-Segment-1-1-iss.root' --fileBaseDir='Histogramming-L2-Segment-1-1-iss/L2PU-32767/EXPERT' --noRoot --zip --ignoreMissingRef --output rootcomp_l2
l2status=$? 

echo
log "*************************************************************************"
log "*                        Comparing EF histograms                        *"
log "*************************************************************************"
node=`hostname -s`
rootcomp.py $efref 'Histogramming-EF-Segment-1-1-iss.root' --fileBaseDir="Histogramming-EF-Segment-1-1-iss/PT-1_EF-Segment-1-1_${node}_1/EXPERT" --noRoot --zip --ignoreMissingRef --output rootcomp_ef
efstatus=$?

log "-------------------------------------------------------------------------"
log "L2 rootcomp.py exit code: ${l2status}"
log "EF rootcomp.py exit code: ${efstatus}"
log "-------------------------------------------------------------------------"

if [ $l2status -eq 0 -a $efstatus -eq 0 ]; then
    exit 0
else
    exit 1
fi
