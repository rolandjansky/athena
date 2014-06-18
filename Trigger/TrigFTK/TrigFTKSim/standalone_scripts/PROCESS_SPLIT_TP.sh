#!/bin/bash

# Sample script used for large-scale single-muon grid processing using TP split banks

if [ "$#" -lt "3" ]; then
    echo "USAGE: $0 SITE inpDS tag [ftkDS]";
    echo " (supply ftkDS for merging jobs) ";
    return -1 &> /dev/null;
    exit -1;
fi;
source grid.sh

site=${1}
_inp=${2}
MY_INPUT="--inDS ${_inp}"
_tag=${3}

echo "site = ${site}"
echo "inp = ${_inp}"
echo "tag = ${_tag}"
if [ "$#" -ge "4" ]; then
    _ftkds=${4}
    echo "ftkds = ${_ftkds}"
fi;

global_extra="${global_extra} -o skip_tarball,COPY_WRAPS_FROM_DCACHE,COPY_BANKS_FROM_DCACHE"
INPUT="-n 150000 --nFilesPerJob 7 ${MY_INPUT}"  # good for single muons 20k/file
INPUT="-n 10000 --nFilesPerJob 10 ${MY_INPUT}"  # good for 3E34 events 10/file

############################### SPLIT run (per-region) ######################################
#### split arch run
unset extra loops regions subregions inputs
# specify 8L part:
m=16
runstring=raw_8L_50x16x144_${_tag}_16M      # notice that first tag has to give nsubs, e.g: _16M
baseDS8=${baseDS8}
bankDS8=${bankDS8_50x16x144}
MAXPATTERNS8=4000000                       # per subregion!
# specify 4L part:
runstring4=raw_4L_20x156I225x25_${_tag}_1M  # notice that first tag has to give nsubs, e.g: _16M
baseDS4=${baseDS4}
bankDS4=${bankDS4_20x156I225x25}
MAXPATTERNS4=25000000                      # per subregion!
extra=""

if [ -n "${_ftkds}" ]; then
#### Merging subregions for 11L(aka 4L) tracks in the split arch run
    ftkDS=${_ftkds}
    runstring=raw_11L_20x156I225x25_${_tag}_1M
    MAXPATTERNS=${MAXPATTERNS4}
    extra=""
    mergeSPLIT_reg
else
    global_extra="${global_extra} -o skip_ana"
    runSPLIT_reg
fi;
