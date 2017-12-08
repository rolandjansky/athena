#!/bin/bash

echo "Execute TrigAnalysisTest post processing"

### DEFAULTS

if [ -z ${JOB_LOG} ]; then
  export JOB_LOG="athena.log"
fi

if [ -z ${TEST} ]; then
  export TEST="TrigAnalysisTest"
fi

if [ -z ${REF_FOLDER} ]; then
  export REF_FOLDER="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/${TEST}/ref/${AtlasBuildBranch}/${NAME}"
fi

###

echo "##############################"
echo "Test AthenaTrigAOD_TrigDecTool"
export TDT_LOG=${JOB_LOG%%.*}.TrigDecTool.${JOB_LOG#*.}
athena.py -c 'fileList=["AOD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigAOD_TrigDecTool.py | tee ${TDT_LOG}
echo "art-result: ${PIPESTATUS[0]}"
echo "Running checklog"
timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${TDT_LOG} | tee checklog.TrigDecTool.log
echo "art-result: ${PIPESTATUS[0]}"

echo "##############################"
echo "Test AthenaTrigAOD_TrigEDMCheck"
export EDM_LOG=${JOB_LOG%%.*}.TrigEDMCheck.${JOB_LOG#*.}
athena.py -c 'fileList=["AOD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigAOD_TrigEDMCheck.py | tee ${EDM_LOG}
echo "art-result: ${PIPESTATUS[0]}"
echo "Running checklog"
timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${EDM_LOG} | tee checklog.TrigEDMCheck.log
echo "art-result: ${PIPESTATUS[0]}"

echo "##############################"
echo "Test AthenaTrigAOD_TrigEDMAuxCheck"
export EDMAUX_LOG=${JOB_LOG%%.*}.TrigEDMAuxCheck.${JOB_LOG#*.}
athena.py -c 'fileList=["AOD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigAOD_TrigEDMAuxCheck.py | tee ${EDMAUX_LOG}
echo "art-result: ${PIPESTATUS[0]}"
echo "Running checklog"
timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${EDMAUX_LOG} | tee checklog.TrigEDMAuxCheck.log
echo "art-result: ${PIPESTATUS[0]}"

echo "################################"
echo "Test AthenaTrigAOD_TrigHLTMon"
export TRIGHLTMON_LOG=${JOB_LOG%%.*}.TrigHLTMon.${JOB_LOG#*.}
TrigHLTMon_tf.py --inputAODFile  AOD.pool.root --outputHISTFile myHIST.root | tee ${TRIGHLTMON_LOG}
echo "art-result: ${PIPESTATUS[0]}"
echo "Running checklog"
timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${TRIGHLTMON_LOG} | tee checklog.TrigHLTMon.log
echo "art-result: ${PIPESTATUS[0]}"

echo "################################"
echo "Test AthenaTrigESD_HLTMonitoring"
export HLTMON_LOG=${JOB_LOG%%.*}.HLTMonitoring.${JOB_LOG#*.}
athena.py -c 'fileList=["ESD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigESD_HLTMonitoring.py | tee ${HLTMON_LOG}
echo "art-result: ${PIPESTATUS[0]}"
echo "Running checklog"
timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${HLTMON_LOG} | tee checklog.HLTMonitoring.log
echo "art-result: ${PIPESTATUS[0]}"
mv histo.root expert-monitoring.root 
if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
  echo "Running rootcomp"
  timeout 10m rootcomp.py ${REF_FOLDER}/expert-monitoring.root | tee rootcompout.HLTMonitoring.log
  echo "art-result: ${PIPESTATUS[0]}"
else
  echo "No reference expert-monitoring.root found in ${REF_FOLDER}"
fi

echo "#######################################"
echo "Test AthenaTrigAODtoAOD_TrigNavSlimming"
export SLIM_LOG=${JOB_LOG%%.*}.TrigNavSlimming.${JOB_LOG#*.}
athena.py -c 'TestType="RSegamma";useCONDBR2=False;' -b TrigAnalysisTest/testAthenaTrigAODtoAOD_TrigNavSlimming.py | tee ${SLIM_LOG}
echo "art-result: ${PIPESTATUS[0]}"
echo "Running checklog"
timeout 1m check_log.pl --config checklogTriggerTest.conf --showexcludestats ${SLIM_LOG} | tee checklog.TrigNavSlimming.log
echo "art-result: ${PIPESTATUS[0]}"
if [ -f AOD.pool.root ]; then 
  echo "Running CheckFile on AOD"
  timeout 10m checkFile.py AOD.pool.root | tee .pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]}"
  echo "Running CheckxAOD AOD"
  timeout 10m checkxAOD.py AOD.pool.root | tee AOD.pool.root.checkxAODFile
  echo "art-result: ${PIPESTATUS[0]}"
else 
  echo "No AOD.pool.root to check"
fi
