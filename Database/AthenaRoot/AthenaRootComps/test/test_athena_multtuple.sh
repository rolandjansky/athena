#!/bin/bash

set -e

testname=test_athena_multtuple
rm -rf $testname
mkdir $testname
cd $testname

ATLAS_REFERENCE_TAG=AthenaRootComps/AthenaRootCompsReference-01-00-01
refbase=ref.d3pd_der.ascii
chkfile=d3pd.ascii

reffile=$refbase
get_files -data -symlink $reffile > /dev/null
if [ ! -r $reffile ]; then
  # Look for the file in DATAPATH.
  # We have to look for the directory, not the file itself,
  # since get_files is hardcoded not to look more than two
  # levels down.
  get_files -data -symlink $ATLAS_REFERENCE_TAG > /dev/null
  reffile=`basename $ATLAS_REFERENCE_TAG`/$refbase
  if [ ! -r $reffile ]; then
      testdata=$ATLAS_REFERENCE_DATA
      if [ "$testdata" = "" ]; then
          testdata=/afs/cern.ch/atlas/maxidisk/d33/referencefiles
      fi
      reffile=$testdata/$ATLAS_REFERENCE_TAG/$refbase
  fi
fi

testdir=$ATLAS_REFERENCE_DATA
if [ "$testdir" = "" ]; then
    testdir="root://eosatlas//eos/atlas/user/b/binet/utests/utests/filter-d3pd"
fi

arc_test_make_slim.py $testdir/ntuple.0.root ntuple_slim.0.root 100
arc_test_make_slim.py $testdir/ntuple.1.root ntuple_slim.1.root 100

echo "::::::::::::::::::::::::::::::::::::::::::::::"
echo "::: run athena-ntuple-dumper... (w/ multiple tuples)"
time_cmd="/usr/bin/time -a -o d3pd.rw.timing.log"
/bin/rm -rf d3pd.rw.timing.log >& /dev/null
($time_cmd athena.py -c'FNAMES=["ntuple_slim.0.root", "ntuple_slim.1.root"]; TUPLENAME="egamma;egamma_der"' -lERROR AthenaRootComps/test_athena_ntuple_dumper.py >& d3pd.000.0.log.txt) || exit 1
grep "user" d3pd.rw.timing.log
echo "::: comparing ascii dumps..."
/bin/rm -f d3pd.ascii.diff
diff -urN $reffile $chkfile >& d3pd.ascii.diff
sc=$?
echo "::: comparing ascii dumps...[$sc]"
if [ $sc -ne 0 ]; then
    exit $sc
fi
