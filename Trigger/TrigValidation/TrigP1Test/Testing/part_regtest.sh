#!/usr/bin/env bash
## Do REGTEST for partition test
## Run this script in the run directory of the partition test

## Prepend script name to all our printouts
shopt -s expand_aliases
alias log='echo [`basename $0`] '

if [ $# -ne 2 ]; then
   log "Syntax: `basename $0` L2-reference EF-reference"
   exit 1
fi

## Get the partition name from the DB file name
part_name=`ls -1 *rel_nightly.data.xml`
part_name=`basename $part_name '.data.xml'`

## Replace /latest/ with /NIGHTLY/latest
## This reproduces the same (confusing) behavior as in trigtest.pl
# make the extraction of the nightly name  exactly  same as in trigtest.pl  
#l2ref=`echo $1 | sed "s#/latest/#/${NICOS_NIGHTLY_NAME}/latest/#"`
#efref=`echo $2 | sed "s#/latest/#/${NICOS_NIGHTLY_NAME}/latest/#"`

#printenv CMTPATH
#thisrel=`printenv CMTPATH | awk '{ print  substr($1, index($1,"builds/nightlies/")+17, 30 )  }' | awk '{print substr($1,1,index($1,"/Atlas")-1) }' ` 
#echo "looking for references in release: ${thisrel}"
#l2ref=`echo $1 | sed "s#/latest/#/${thisrel}/latest/#"`
#efref=`echo $2 | sed "s#/latest/#/${thisrel}/latest/#"`

l2ref=$1
efref=$2

regTest() {
    ref=$1
    file=$2
    ignore=$3
    grep -a REGTEST $file > $file.regtest
    echo "Reference: $ref"
    echo "File:      $file.regtest"
    if [ -z "$ignore" ]; then
        diff -b $ref $file.regtest
    else
        echo "Ignoring:  $ignore"
        cat ${ref} | egrep -v "${ignore}" > "ref.ignore"
        cat ${file}.regtest | egrep -v "${ignore}" > "file.ignore"
        diff -b "ref.ignore" "file.ignore"
    fi
    status=$?
    if [ $status -eq 0 ]; then
        echo "---> No differences found <---"
    fi
    
    return $status
}

## Diff L2PU log file
echo
log "*************************************************************************"
log "*                    REGTEST of L2PU vs. athenaMT                       *"
log "*************************************************************************"
l2log=`ls ${part_name}/L2PU*.out`
regTest $l2ref $l2log 'L2MissingET_Fex.*Lvl1Id|L2MissingET_Fex.*Run number|L2MissingET_Fex.*DetMask_1'
l2status=$?

## Diff PT log file
echo
log "*************************************************************************"
log "*                     REGTEST of PT vs. athenaPT                        *"
log "*************************************************************************"
eflog=`ls ${part_name}/PT*.out`
regTest $efref $eflog 'EFMissingET_Fex.*Lvl1Id|EFMissingET_Fex.*Run number|EFMissingET_Fex.*DetMask_1'
efstatus=$?

echo
log "-------------------------------------------------------------------------"
log "L2 regtest status: ${l2status}"
log "EF regtest status: ${efstatus}"
log "-------------------------------------------------------------------------"

if [ $l2status -eq 0 -a $efstatus -eq 0 ]; then
    exit 0
else
    exit 1
fi
