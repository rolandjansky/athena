#!/bin/bash
# art-description: athenaHLT test of timeout handling
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.out
# art-output: *.err
# art-output: *.data
# art-output: *.root

outputBaseName="output.test_athenaHLT_timeout.data"
nEvents=20

(set -x; \
athenaHLT \
-n ${nEvents} \
--timeout 2000 \
--nprocs 2 \
--threads 2 \
--concurrent-events 2 \
-o ${outputBaseName} \
-f /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1 \
TrigP1Test/testHLT_timeout.py \
2>&1 > athena.log)

status=$?
echo "art-result: ${status} athenaHLT-mother"

outputFile=`ls ${outputBaseName}* | tail -n 1`

# Check if there is at least one event with the debug_HltTimeout stream tag
nDebugEvents=`trigbs_dumpHLTContentInBS.py --stag output.test_athenaHLT_timeout.data_Child-001._0001.data | grep "\[('HltTimeout', 'debug')\]" | wc -l`
if [ $nDebugEvents -gt 0 ]; then 
  echo "Found ${nDebugEvents} events with stream tag debug_HltTimeout in the output file ${outputFile}"
  echo "art-result: 0 StreamTags"
else
  echo "ERROR no event with stream tag debug_HltTimeout was found in the output file ${outputFile}"
  echo "art-result: 0 StreamTags"
fi

# Check if the number of events in output is the same as number of processed events
nOutputEvents=`trigbs_dumpHLTContentInBS.py --stats output.test_athenaHLT_timeout.data_Child-001._0001.data | grep Global_ID | wc -l`
if [ $nOutputEvents -eq $nEvents ]; then
  echo "All ${nEvents} processed events were found in the output file ${outputFile}"
  echo "art-result: 0 NOutputEvents"
else
  echo "Processed ${nEvents} events, but found ${nOutputEvents} in the output file ${outputFile}"
  echo "art-result: 1 NOutputEvents"
fi

