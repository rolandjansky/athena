#!/bin/bash

set -e

testname=test_athena_ntuple_dumper_multiple
rm -rf $testname
mkdir $testname
cd $testname

ATLAS_REFERENCE_TAG=AthenaRootComps/AthenaRootCompsReference-01-00-01
refbase=ref.d3pd.ascii
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

echo "::::::::::::::::::::::::::::::::::::::::::::::"
echo "::: run athena-ntuple-dumper... (w/ multiple output-streams)"
time_cmd="/usr/bin/time -a -o d3pd.rw.timing.log"
/bin/rm -rf d3pd.rw.timing.log >& /dev/null
($time_cmd athena.py -c'USEVARHANDLE=1; DOWRITE=2' -lERROR AthenaRootComps/test_athena_ntuple_dumper.py >& d3pd.000.0.log.txt) || exit 1
grep "user" d3pd.rw.timing.log
echo "::: comparing ascii dumps..."
/bin/rm -f d3pd.ascii.diff
diff -urN $reffile $chkfile >& d3pd.ascii.diff
sc=$?
echo "::: comparing ascii dumps...[$sc]"
if [ $sc -ne 0 ]; then
    exit $sc
fi
echo "::: comparing output n-tuples..."
acmd.py diff-root d3pd.root d3pd_2.root -t egamma 2> /dev/null
sc=$?
echo "::: comparing output n-tuples...[$sc]"
if [ $sc -ne 0 ]; then
    exit $sc
fi
