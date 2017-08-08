#!/bin/bash

# Execute grid post processing

# Test #1
timeout 10m check_log.pl --config checklogTriggerTest.conf ${ATHENA_LOG}

# Test #2
timeout 10m PerfMonRunner.py --fileName=ntuple.pmon.gz --options="-f 0.90"

# Test #3
timeout 10m chainDump.py -S --rootFile=expert-monitoring.root

# Test #4
if [ -f trig_cost.root ]; then 
  timeout 2h RunTrigCostD3PD -f trig_cost.root --outputTagFromAthena --costMode --linkOutputDir
else 
  echo "file trig_cost.root does not exist thus RunTrigCostD3PD will not be run" >> warnings_postprocessing.txt;
fi

# Test #5
if [[ `sed 's|.*\(.* \)|\1|' L1AV.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
  echo "L1 counts   ERROR  : all entires are ZERO please consult L1AV.txt" >> errors_postprocessing.txt
fi

# Test #6
if [[ `sed 's|.*\(.* \)|\1|' HLTChain.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
  echo "HLTChain counts   ERROR  : all entires are ZERO please consult HLTChain.txt" >> errors_postprocessing.txt
fi

# Test #7
if [[ `sed 's|.*\(.* \)|\1|' HLTTE.txt | sed 's/^[ \t]*//' |  sed '/^0/'d | wc -l` == 0 ]]; then 
  echo "HLTTE counts   ERROR  : all entires are ZERO please consult HLTTE.txt" >> errors_postprocessing.txt
fi
