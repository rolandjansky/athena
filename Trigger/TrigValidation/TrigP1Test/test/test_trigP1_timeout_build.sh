#!/bin/bash
# art-description: athenaHLT test of timeout handling
# art-type: build
# art-include: master/Athena
# art-output: *.log
# art-output: *.out
# art-output: *.err
# art-output: *.data
# art-output: *.root

outputBaseName="output.test_trigP1_timeout.data"
nEvents=20

export NAME="timeout"
export ART_CMD="athenaHLT -n ${nEvents} --timeout 2000 --nprocs 2 --threads 2 --concurrent-events 2 -o ${outputBaseName} -f INPUT_FILE TrigP1Test/testHLT_timeout.py"
export ART_SKIP_CHECKLOG="true"
export ART_NO_COUNT="true"
exec_art_trigp1test.sh

outputFile=`ls ${outputBaseName}* | tail -n 1`

# Check if there is at least one event with the debug_HltTimeout stream tag
nDebugEvents=`trigbs_dumpHLTContentInBS.py --stag ${outputFile} | grep "\[('HltTimeout', 'debug')\]" | wc -l`
if [ $nDebugEvents -gt 0 ]; then 
  echo "Found ${nDebugEvents} events with stream tag debug_HltTimeout in the output file ${outputFile}"
  echo "art-result: 0 StreamTags"
else
  echo "ERROR no event with stream tag debug_HltTimeout was found in the output file ${outputFile}"
  echo "art-result: 1 StreamTags"
fi

# Check if the number of events in output is the same as number of processed events
nOutputEvents=`trigbs_dumpHLTContentInBS.py --stats ${outputFile} | grep Global_ID | wc -l`
if [ $nOutputEvents -eq $nEvents ]; then
  echo "All ${nEvents} processed events were found in the output file ${outputFile}"
  echo "art-result: 0 NOutputEvents"
else
  echo "Processed ${nEvents} events, but found ${nOutputEvents} in the output file ${outputFile}"
  echo "art-result: 1 NOutputEvents"
fi
