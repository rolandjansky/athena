#!/bin/bash

set -e

testdir=`arc_linkinputs.sh`
arc_test_make_slim.py $testdir/ntuple.0.root ntuple_slim.0.root 100
arc_test_make_slim.py $testdir/ntuple.1.root ntuple_slim.1.root 100

echo "::::::::::::::::::::::::::::::::::::::::::::::"
echo "::: run athena-ntuple-dumper-seek... (w/ multiple tuples)"
time_cmd="/usr/bin/time -a -o d3pd.rw.timing.log"
/bin/rm -rf d3pd.rw.timing.log >& /dev/null
($time_cmd athena.py -i -c'FNAMES=["ntuple_slim.0.root", "ntuple_slim.1.root"]; TUPLENAME="egamma;egamma_der"' -lERROR AthenaRootComps/test_athena_ntuple_dumper_seek.py >& d3pd.000.0.log.txt) || exit 1
grep "user" d3pd.rw.timing.log
