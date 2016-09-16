#!/bin/bash
# author Andre Anjos <andre.dos.anjos@cern.ch>
# author Ricardo Abreu <ricardo.abreu@cern.ch>

thisdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd -P )"
testdir="$thisdir/../HLTTestApps"
echo $testdir

function runtest()
{
  echo
  echo "==============================================================="
  echo "Running "$@" tests"
  echo "==============================================================="
  echo 
  python "$testdir/$@"
  ret=$? 
  echo
  echo "============================================"
  echo "The status output of this set of tests is $ret"
  echo "============================================"
  echo
  if [ $ret -ne 0 ]; then
    exit $ret
  fi
}

targets=( "option.py" "pausable_istream.py" "configuration.py" "infrastructure.py" "online_infrastructure.py" "processor.py")
                     
for tgt in "${targets[@]}" 
do
  runtest $tgt
done

