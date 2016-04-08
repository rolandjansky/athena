#!/bin/sh
#
# NICOS - NIghtly COntrol System
# Author Alex Undrus <undrus@bnl.gov>
#
# ----------------------------------------------------------
# run_nicos_for_stable_release.sh : name says it all
# ----------------------------------------------------------
#
function show_help() {
    echo "Usage: run_nicos_for_stable_release.sh \\"
    echo "-r <relname> <--opt|--dbg>"
    echo "This script performs the standard task of testing and creation"
    echo "of documentation for the stable release located in"
    echo "/afs/cern.ch/atlas/software/dist area (release must be"
    echo "already built). NICOS web pages are generated in "
    echo "/afs/cern.ch/atlas/software/dist/www area." 
    echo "Mandatory options:"
    echo " -r | --relnum   name of a stable release, e.g. 10.4.0."
    echo "                 must exist in /afs/cern.ch/atlas/software/dist"
    echo "Optional:" 
    echo " <--opt|--dbg>   compiler option (dbg or opt)"
    echo "if omitted both opt and dbg documentation will be produced"
}
 
export NICOS_HOME="/afs/cern.ch/atlas/software/dist/nightlies/nicos_1_0"
release=""
option="all"

comname=`basename $0`
if [ "$comname" = "run_nicos_for_stable_release.sh" ]; then
exitcomjob="exit"
else
exitcomjob="return"
fi

while [ $# -ne 0 ]; do
    case $1 in
         -r | --relnum)    release=$2; shift;;
              --opt)       option="opt";;
              --dbg)       option="dbg";;      
         -* | --*)         show_help; eval ${exitcomjob} 1;;
    esac
    shift
done
 
eval $NICOS_HOME/atlasjobs/test_relname.sh $release; stat=$? 
if [ "$stat" != "0" ]; then
echo "run_nicos_for_stable_release.sh:ERROR: invalid name of stable release"
show_help; eval ${exitcomjob} 1
fi

if [ "$option" = "opt" -o "$option" = "all" ]; then 
eval $NICOS_HOME/atlasjobs/test_and_docs_for_stable_release -r $release --opt -d /afs/cern.ch/atlas/software/dist/${release}/AtlasRelease/*/cmt 2>&1 & 
fi

if [ "$option" = "all" ]; then sleep 1200; fi

if [ "$option" = "dbg" -o "$option" = "all" ]; then
eval $NICOS_HOME/atlasjobs/test_and_docs_for_stable_release -r $release --dbg -d /afs/cern.ch/atlas/software/dist/${release}/AtlasRelease/*/cmt 2>&1 &
fi

