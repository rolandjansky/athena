## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True

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
evtMax            = 500

## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)                    
import os
partitionName   = os.environ.get("TDAQ_PARTITION", "TestDataProvider_TCT")
#"TestPartitionGlobalMonitoring" 

publishName     = 'GM_test_19.1.0.1'
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
ConditionsTag     = 'COMCOND-HLTP-004-03'
DetDescrVersion   = 'ATLAS-GEO-20-00-01'
beamType = 'collisions'

doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

abortonuncheckedstatuscode = False


## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = True
doInDet     = doAllReco
doMuon      = doAllReco #False
doLArg      = doAllReco
doTile      = doAllReco
doTrigger   = False
doHist      = doAllReco
doJiveXML   = False
doEgammaTau = doAllReco

#set to True in the JO
#doCommissioning = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = True
doCaloMon = doAllMon
doTileMon = doAllMon #AK: new since 09 july 2014 (M4)
doPhysMon = doAllMon
doTrigMon = False
doIDMon   = doAllMon
doTRTMon  = doAllMon
doMuonMon = False #doAllMon

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

#### CHANGES TO GET 19.1.0.1 RECO WORKING (M4)
from AthenaMonitoring.DQMonFlags import DQMonFlags
DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(False)
DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)
#DQMonFlags.doJetTagMon.set_Value_and_Lock(False)
#DQMonFlags.doCaloMon.set_Value_and_Lock(False)
#DQMonFlags.doPixelMon.set_Value_and_Lock(False)


## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")
