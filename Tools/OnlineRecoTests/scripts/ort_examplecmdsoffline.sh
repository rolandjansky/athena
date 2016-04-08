#!/bin/sh
## runcmds found in <../test/OnlineRecoTests_DefaultConfiguration.xml> : 

##
tct_getAmiTag.py f100 ami_recotrf.cmdargs ami_recotrf.pickle

##
export RUNNUMBER=96544; tct_recotrf.sh 0 IDCosmic 3 500 /castor/cern.ch/grid/atlas/DAQ/2008/00${RUNNUMBER} ami_recotrf.cmdargs False offlineReco0_isOnlineFalse.log

##
export RUNNUMBER=96544; tct_recotrf.sh 1 TRTCosmic 3 500 /castor/cern.ch/grid/atlas/DAQ/2008/00${RUNNUMBER} ami_recotrf.cmdargs False offlineReco1_isOnlineFalse.log

##
export RUNNUMBER=96544; tct_recotrf.sh 2 IDCosmic 1 100 /castor/cern.ch/grid/atlas/DAQ/2008/00${RUNNUMBER} ami_recotrf.cmdargs False offlineReco2_isOnlineTrue.log

##
export POSTINC=OnlineRecoTests/OnlineRecoTests_autoconfiguration.py; tct_recotrf.sh 1 L1Calo 1 100 /castor/cern.ch/grid/atlas/DAQ/2009 ami_recotrf.cmdargs True

##
athena RecExOnline/RecExOnline_Test_Offline.py | tee offlineReco3_isOnlineFalse.log

##
export PREEXEC="rec.doInDet.set_Value_and_Lock(True),,rec.doMuon.set_Value_and_Lock(False),,rec.doLArg.set_Value_and_Lock(False),,rec.doTile.set_Value_and_Lock(False),,DQMonFlags.doMonitoring.set_Value_and_Lock(False)"; export RUNNUMBER=91890; tct_recotrf.sh 0 IDCosmic 1 100 /castor/cern.ch/grid/atlas/DAQ/2008/${RUNNUMBER} ami_recotrf.cmdargs

##
export PREEXEC="rec.doInDet.set_Value_and_Lock(False),,rec.doMuon.set_Value_and_Lock(True),,rec.doLArg.set_Value_and_Lock(False),,rec.doTile.set_Value_and_Lock(False),,DQMonFlags.doMonitoring.set_Value_and_Lock(False)"; export RUNNUMBER=91890; tct_recotrf.sh 0 IDCosmic 1 100 /castor/cern.ch/grid/atlas/DAQ/2008/${RUNNUMBER} ami_recotrf.cmdargs

##
export PREEXEC="rec.doInDet.set_Value_and_Lock(False),,rec.doMuon.set_Value_and_Lock(False),,rec.doLArg.set_Value_and_Lock(True),,rec.doTile.set_Value_and_Lock(False),,DQMonFlags.doMonitoring.set_Value_and_Lock(False)"; export RUNNUMBER=91890; tct_recotrf.sh 0 L1CaloEM 1 100 /castor/cern.ch/grid/atlas/DAQ/2008/${RUNNUMBER} ami_recotrf.cmdargs

##
export PREEXEC="rec.doInDet.set_Value_and_Lock(False),,rec.doMuon.set_Value_and_Lock(False),,rec.doLArg.set_Value_and_Lock(False),,rec.doTile.set_Value_and_Lock(True),,DQMonFlags.doMonitoring.set_Value_and_Lock(False)"; export RUNNUMBER=91890; tct_recotrf.sh 0 L1CaloEM 1 100 /castor/cern.ch/grid/atlas/DAQ/2008/${RUNNUMBER} ami_recotrf.cmdargs

##
export PREEXEC="rec.doInDet.set_Value_and_Lock(False),,rec.doMuon.set_Value_and_Lock(False),,rec.doLArg.set_Value_and_Lock(True),,rec.doTile.set_Value_and_Lock(True),,DQMonFlags.doMonitoring.set_Value_and_Lock(False)"; export RUNNUMBER=91890; tct_recotrf.sh 0 L1CaloEM 1 100 /castor/cern.ch/grid/atlas/DAQ/2008/${RUNNUMBER} ami_recotrf.cmdargs

##
ort_checkLogfiles.py offlineReco0_isOnlineFalse.log,offlineReco1_isOnlineFalse.log,offlineReco2_isOnlineTrue.log,offlineReco3_isOnlineFalse.log latest_copied_release ami_recotrf.pickle,globalflags.pickle latest_data.argdict.pickle http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0-vol3/rtt/fallbackfiles/ort/latest_data.argdict.pickle,http://atlas-project-fullchaintest.web.cern.ch/atlas-project-FullChainTest/tier0-vol3/rtt/fallbackfiles/ort/latest.data True

##
echo "ErrorCode=0 (OK)" ; cp -f 0.data /afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/ort/latest.data; cp -f latest_data.argdict.pickle /afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/ort/ ; cp -f latest_copied_release /afs/cern.ch/atlas/offline/external/FullChainTest/tier0-vol3/rtt/fallbackfiles/ort/

##
## Now start the online jobs
##
#ort_examplecmdsonline.sh

