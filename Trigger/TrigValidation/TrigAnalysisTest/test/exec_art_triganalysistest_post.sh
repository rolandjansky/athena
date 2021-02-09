#!/bin/bash

echo $(date "+%FT%H:%M %Z")"     Execute TrigAnalysisTest post processing for test ${NAME}"

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
echo $(date "+%FT%H:%M %Z")"     Test AthenaTrigAOD_TrigDecTool"
export TDT_LOG=${JOB_LOG%%.*}.TrigDecTool.${JOB_LOG#*.}
athena.py -c 'fileList=["AOD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigAOD_TrigDecTool.py 2>&1 | tee ${TDT_LOG}
echo "art-result: ${PIPESTATUS[0]} ${TDT_LOG%.*}"
echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${TDT_LOG} 2>&1 | tee checklog.TrigDecTool.log
echo "art-result: ${PIPESTATUS[0]} CheckLog.TrigDecTool"

echo "##############################"
echo $(date "+%FT%H:%M %Z")"     Test AthenaTrigAOD_TrigEDMCheck"
export EDM_LOG=${JOB_LOG%%.*}.TrigEDMCheck.${JOB_LOG#*.}
athena.py -c 'fileList=["AOD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigAOD_TrigEDMCheck.py 2>&1 | tee ${EDM_LOG}
echo "art-result: ${PIPESTATUS[0]} ${EDM_LOG%.*}"
echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${EDM_LOG} 2>&1 | tee checklog.TrigEDMCheck.log
echo "art-result: ${PIPESTATUS[0]} CheckLog.TrigEDMCheck"

echo "##############################"
echo $(date "+%FT%H:%M %Z")"     Test AthenaTrigAOD_TrigEDMAuxCheck"
export EDMAUX_LOG=${JOB_LOG%%.*}.TrigEDMAuxCheck.${JOB_LOG#*.}
athena.py -c 'fileList=["AOD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigAOD_TrigEDMAuxCheck.py 2>&1 | tee ${EDMAUX_LOG}
echo "art-result: ${PIPESTATUS[0]} ${EDMAUX_LOG%.*}"
echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${EDMAUX_LOG} 2>&1 | tee checklog.TrigEDMAuxCheck.log
echo "art-result: ${PIPESTATUS[0]} CheckLog.TrigEDMAuxCheck"

echo "################################"
echo $(date "+%FT%H:%M %Z")"     Test AthenaTrigAOD_TrigHLTMon"
export TRIGHLTMON_LOG=${JOB_LOG%%.*}.TrigHLTMon.${JOB_LOG#*.}
TrigHLTMon_tf.py --inputAODFile  AOD.pool.root --outputHISTFile myHIST.root 2>&1 | tee ${TRIGHLTMON_LOG}
echo "art-result: ${PIPESTATUS[0]} ${TRIGHLTMON_LOG%.*}"
echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${TRIGHLTMON_LOG} 2>&1 | tee checklog.TrigHLTMon.log
echo "art-result: ${PIPESTATUS[0]} CheckLog.TrigHLTMon"

echo "################################"
echo $(date "+%FT%H:%M %Z")"     Test AthenaTrigESD_HLTMonitoring"
export HLTMON_LOG=${JOB_LOG%%.*}.HLTMonitoring.${JOB_LOG#*.}
athena.py -c 'fileList=["ESD.pool.root"]' -b TrigAnalysisTest/testAthenaTrigESD_HLTMonitoring.py 2>&1 | tee ${HLTMON_LOG}
echo "art-result: ${PIPESTATUS[0]} ${HLTMON_LOG%.*}"
echo $(date "+%FT%H:%M %Z")"     Running checklog"
timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${HLTMON_LOG} 2>&1 | tee checklog.HLTMonitoring.log
echo "art-result: ${PIPESTATUS[0]} CheckLog.HLTMonitoring"
mv histo.root expert-monitoring.root 
if [ -f ${REF_FOLDER}/expert-monitoring.root ]; then
  echo $(date "+%FT%H:%M %Z")"     Running rootcomp"
  timeout 10m rootcomp.py ${REF_FOLDER}/expert-monitoring.root 2>&1 | tee rootcompout.HLTMonitoring.log
  echo "art-result: ${PIPESTATUS[0]} RootComp.HLTMonitoring"
else
  echo $(date "+%FT%H:%M %Z")"     No reference expert-monitoring.root found in ${REF_FOLDER}"
fi

echo "#######################################"
echo $(date "+%FT%H:%M %Z")"     Test AthenaTrigAODtoAOD_TrigNavSlimming"
echo "AtlasProject" $AtlasProject
if [ $AtlasProject = "AthenaP1" ]; then
   echo "This is AthenaP1 so not running TrigNavSlimming test"   
else
   export SLIM_LOG=${JOB_LOG%%.*}.TrigNavSlimming.${JOB_LOG#*.}
   athena.py -c 'TestType="RSegamma";useCONDBR2=False;' -b TrigAnalysisTest/testAthenaTrigAODtoAOD_TrigNavSlimming.py 2>&1 | tee ${SLIM_LOG}
   echo "art-result: ${PIPESTATUS[0]} ${SLIM_LOG%.*}"
   echo $(date "+%FT%H:%M %Z")"     Running checklog"
   timeout 5m check_log.py --errors --config checklogTriggerTest.conf --showexcludestats ${SLIM_LOG} 2>&1 | tee checklog.TrigNavSlimming.log
   echo "art-result: ${PIPESTATUS[0]} CheckLog.TrigNavSlimming"
fi

if [ -f AOD.pool.root ]; then 
  echo $(date "+%FT%H:%M %Z")"     Running CheckFile on AOD"
  timeout 10m checkFile.py AOD.pool.root 2>&1 | tee AOD.pool.root.checkFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileAOD.TrigNavSlimming"
  echo $(date "+%FT%H:%M %Z")"     Running CheckxAOD AOD"
  timeout 10m checkxAOD.py AOD.pool.root 2>&1 | tee AOD.pool.root.checkxAODFile
  echo "art-result: ${PIPESTATUS[0]} CheckFileXAOD.TrigNavSlimming"
else 
  echo $(date "+%FT%H:%M %Z")"     No AOD.pool.root to check"
fi

if [ -f AOD.pool.root ]; then
  echo "#######################################"
  echo $(date "+%FT%H:%M %Z")"     Test checkFileTrigSize_RTT"
  export TRIGSIZE_LOG=${JOB_LOG%%.*}.TrigEDMSize.${JOB_LOG#*.}
  timeout 10m checkFileTrigSize_RTT.py 2>&1 | tee ${TRIGSIZE_LOG}
  echo "art-result: ${PIPESTATUS[0]} TrigEDMSize"
else
  echo $(date "+%FT%H:%M %Z")"     No AOD.pool.root to check with checkFileTrigSize_RTT"
fi

echo  $(date "+%FT%H:%M %Z")"     Files in directory:"
ls -lh

echo $(date "+%FT%H:%M %Z")"     Done executing TrigAnalysisTest post processing for test ${NAME}"
