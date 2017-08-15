#!/bin/bash

# Execute post processing

timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${JOB_LOG}

# this is RTT and will need some moving
#timeout 1m PerfMonRunner.py --fileName=ntuple.pmon.gz --options="-f 0.90"

timeout 1m chainDump.py -S --rootFile=expert-monitoring.root

grep REGTEST athena.log > athena.regtest
timeout 1m regtest.pl --inputfile athena.regtest --reffile athena.regtest
#TODO put in actual references

#timeout 1m rootcomp.py expert-monitoring.root
#TODO needs reference

timeout 1m trigtest_checkcounts.sh 0 expert-monitoring.root expert-monitoring.root HLT
#TODO put in actual references

if [ -f trig_cost.root ]; then 
  timeout 2h RunTrigCostD3PD -f trig_cost.root --outputTagFromAthena --costMode --linkOutputDir
else 
  echo "file trig_cost.root does not exist thus RunTrigCostD3PD will not be run"
fi

if [[ `sed 's|.*\(.* \)|\1|' L1AV.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
  echo "L1 counts   ERROR  : all entires are ZERO please consult L1AV.txt"
fi

if [[ `sed 's|.*\(.* \)|\1|' HLTChain.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
  echo "HLTChain counts   ERROR  : all entires are ZERO please consult HLTChain.txt"
fi

if [[ `sed 's|.*\(.* \)|\1|' HLTTE.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
  echo "HLTTE counts   ERROR  : all entires are ZERO please consult HLTTE.txt"
fi
