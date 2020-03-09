## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True

## this is used for testing in lxplus
## isOfflineTest = True  (lxplus)
## isOfflineTest = False  (P1)
##
isOfflineTest     = False

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py) 

useEmon           = True
keycount          = 250
buffersize        = 10
updateperiod      = 200
timeout           = 600000
#keyname           = 'CompleteEvent'
keyname           = 'dcm'
# keyname           = 'SFI'
#streamName        ='CosmicCalo_physics'
streamName        ='express_express'

useAtlantisEmon   = False
evtMax            = 100

## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)
import os
partitionName   = os.environ.get("TDAQ_PARTITION", "TestDataProvider_TCT")
#"TestPartitionGlobalMonitoring" 

publishName     = 'SMW_GMtestM7_20_2_0_2_1_29may15'
isserverName    = 'Histogramming'

## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'
fileName          = './0.data'

#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
ConditionsTag     = 'CONDBR2-HLTP-2015-08'  ### tag created : 2015-11-04
### ConditionsTag     = 'CONDBR2-HLTP-2014-00'
###
### ConditionsTag     = 'CONDBR2-BLKPA-2014-03'
### ConditionsTag     = 'CONDBR2-ES1PA-2014-01'  ### new for offline (19.3.0.1)

### DetDescrVersion   = 'ATLAS-R2-2015-01-01-00'
### DetDescrVersion   = 'ATLAS-R2-2015-02-00-00'
DetDescrVersion   = 'ATLAS-R2-2015-03-01-00'

beamType = 'cosmics'

doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

abortonuncheckedstatuscode = False


## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = True

doInDet     = doAllReco
### doInDet     = False

doMuon      = doAllReco
### doMuon      = False

doLArg      = doAllReco
doTile      = doAllReco

doTrigger   = doAllReco
### doTrigger   = False

doHist      = doAllReco
doJiveXML   = False

doEgammaTau = doAllReco
### doEgammaTau = False

#set to True in the JO
#doCommissioning = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = True

doCaloMon = doAllMon

doTileMon = doAllMon #AK: new since 09 july 2014 (M4)
#doTileMon = False

doPhysMon = doAllMon

doTrigMon = doAllMon
### doTrigMon = False

doIDMon   = doAllMon
###doIDMon   = False

doTRTMon  = doAllMon
###doTRTMon  = False

doMuonMon = doAllMon
###doMuonMon = False

doCTPMon  = False #doAllMon #AK: 25-01-2014

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

from AthenaCommon.GlobalFlags import globalflags
globalflags.DatabaseInstance.set_Value_and_Lock("CONDBR2")

from AthenaCommon.BFieldFlags import jobproperties
jobproperties.BField.barrelToroidOn.set_Value_and_Lock(True)
jobproperties.BField.endcapToroidOn.set_Value_and_Lock(True)
jobproperties.BField.solenoidOn.set_Value_and_Lock(True)
###jobproperties.BField.solenoidOn.set_Value_and_Lock(False)


from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArHVCorr.set_Value_and_Lock(False)
jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False)
jobproperties.CaloCellFlags.doLArDeadOTXCorr.set_Value_and_Lock(False)  ####? for trigger, added 11th March, 2015, by CY


from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True)
InDetFlags.useBroadClusterErrors.set_Value_and_Lock(False)

from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doStreamAwareMon.set_Value_and_Lock(False)
DQMonFlags.enableLumiAccess.set_Value_and_Lock(False)
DQMonFlags.doCTPMon.set_Value_and_Lock(False)         
DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
### DQMonFlags.doMuonTrkPhysMon.set_Value_and_Lock(False) #xx
DQMonFlags.doMuonTrkPhysMon.set_Value_and_Lock(True) #xx
### DQMonFlags.doJetTagMon.set_Value_and_Lock(False)

from RecExConfig.RecFlags import rec
rec.doBTagging.set_Value_and_Lock(True)

from JetRec.JetRecFlags import jetFlags
jetFlags.useBTagging.set_Value_and_Lock(True)

### from JetRec.JetRecFlags import jetFlags
### jetFlags.useTracks.set_Value_and_Lock(False)

from MuonDQAMonFlags.MuonDQAProperFlags import MuonDQADetFlags
### MuonDQADetFlags.doMDTTGCL1Mon.set_Value_and_Lock(True)
MuonDQADetFlags.doTGCL1Mon.set_Value_and_Lock(True)
MuonDQADetFlags.doTGCMon.set_Value_and_Lock(True)


##---------------------------------------ID suggestion from Alex 18.Feb.2015  --------
jobproperties.InDetJobProperties.doTrackSegmentsSCT.set_Value_and_Lock(False)
jobproperties.InDetJobProperties.doTrackSegmentsPixel.set_Value_and_Lock(False)
jobproperties.InDetJobProperties.doTrackSegmentsTRT.set_Value_and_Lock(False)

##---------------------------------------Bunch Spacing 25ns  --------
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.InDet25nsec.set_Value_and_Lock(True)

jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25)

# --------------------------------------------- Monitoring
### replaced by MuonTrackMonitoring (SMW Jan 23 2016)
### from AthenaMonitoring.DQMonFlags import DQMonFlags
### DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(True)


### ------------------- inherited from 20.1.0.2 -----------
#from RecExConfig.RecFlags import rec
#rec.doJetMissingETTag.set_Value_and_Lock(False)


#from TrigHLTMonitoring.HLTMonFlags import HLTMonFlags
#HLTMonFlags.doBphys=False


#jobproperties.CaloCellFlags.doLArDeadOTXCorr.set_Value_and_Lock(False)

#from LArConditionsCommon.LArCondFlags import larCondFlags
#larCondFlags.OFCShapeFolder.set_Value_and_Lock("")

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")
