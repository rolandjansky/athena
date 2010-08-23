######################################################################################
#
# This file is intended for running the MET slice in the offline software.
# By default a ttbar RDO file is used.
#
# HOWTO run:
# 1. create a run directory          : cd $TestArea; mkdir run; cd run
# 2. source & get some trigger stuff : source source TriggerRelease_links.sh
# 3. copy our test jobOption file    : cp ../Trigger/TrigHypothesis/TrigMissingETHypo/share/testMETSlice_jobOptions.py .
# 4. run the job                     : athena testMETSlice_jobOptions.py &>! log &
#
######################################################################################


### usually ATN tests runs with following RDO input:
#PoolRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/calib1_csc11.005200.T1_McAtNlo_Jimmy.digit.RDO.v12000301_tid003138._00016_extract_10evt.pool.root"]


if not ('EvtMax' in dir()):
    EvtMax=10
if not ('OutputLevel' in dir()):
    OutputLevel=INFO

### need to check if default is ok
doCBNT=False



doTrigger=True
doESD=False

doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False
doESD=False
doTAG=False

doTruth=True
#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------

TriggerFlags.readHLTconfigFromXML=False
TriggerFlags.readLVL1configFromXML=False

TriggerFlags.enableMonitoring = [ 'Validation', 'Time', 'Log' ]

#------------ This is a temporary fix ---------------
TriggerFlags.abortOnConfigurationError=True
#-------------end of temporary fix-------------------

####  First switch all slices OFF
TriggerFlags.Slices_all_setOff()


# add muons for MET ... !
if readMuonHits:
    DetFlags.digitize.Muon_setOn()
    DetFlags.readRIOBS.Muon_setOff()
    DetFlags.readRIOPool.LVL1_setOff()
    TriggerFlags.doLVL1=True
    ###### This is a temporary fix ################
    DetFlags.simulateLVL1.Muon_setOn()
else:
    DetFlags.digitize.Muon_setOff()

###### This is a temporary fix ################
DetFlags.simulateLVL1.Calo_setOn()

# Enable MET slice
TriggerFlags.METSlice.unsetAll()
TriggerFlags.METSlice.setL2Calo()
TriggerFlags.METSlice.setEFCalo()
#TriggerFlags.doEF=False

TriggerFlags.MuonSlice.setL2Muon()

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence()
print ServiceMgr


AlgSequence().TrigSteer_L2.OutputLevel=VERBOSE
AlgSequence().TrigSteer_L2.T2MissingET.OutputLevel=VERBOSE
