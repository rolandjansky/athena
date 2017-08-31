#!/bin/bash

echo  "Execute TriggerTest post processing"

timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${JOB_LOG}

# this is RTT and will need some moving
#timeout 1m PerfMonRunner.py --fileName=ntuple.pmon.gz --options="-f 0.90"

timeout 1m chainDump.py -S --rootFile=expert-monitoring.root

export REF_FOLDER="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TriggerTest/ref/${AtlasBuildBranch}/${NAME}"

if [ -f ${REF_FOLDER}/athena.regtest ]; then
  echo "Running regtest"
  grep REGTEST athena.log > athena.regtest
  timeout 1m regtest.pl --inputfile athena.regtest --reffile ${REF_FOLDER}/athena.regtest
else
  echo "No reference athena.regtest found in ${REF_FOLDER}"
fi

if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
  echo "Running rootcomp"
  timeout 10m rootcomp.py ${REF_FOLDER}/expert-monitoring.root
  echo "Running checkcounts"
  timeout 10m trigtest_checkcounts.sh 0 expert-monitoring.root ${REF_FOLDER}/expert-monitoring.root HLT
else
  echo "No reference expert-monitoring.root found in ${REF_FOLDER}"
fi

if [ -f trig_cost.root ]; then 
  echo "Running CostMon"
  timeout 2h RunTrigCostD3PD -f trig_cost.root --outputTagFromAthena --costMode --linkOutputDir > costMon.log 2>&1 
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
