
## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py) 
useEmon           = True
keyname           = 'efd'
keycount          = 255
buffersize        = 10
updateperiod      = 200
timeout           = 240000
streamLogic       = 'Or'
#streamName        = 'bulk'
streamName = 'L1Calo:MinBias:L1MinBias:Egamma:JetTauEtmiss:Muons:CosmicCalo:CosmicMuons'
useAtlantisEmon   = False
evtMax            = -1

## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)                    
import os
partitionName = os.environ.get("TDAQ_PARTITION", "ATLAS")

publishName     = 'PixelPT'
#isserverName    = 'Histogramming-Pixel'
if (partitionName=='TestDataProvider_TCT'):
    isserverName    = 'Histogramming'
else:
    isserverName    = 'Histogramming-Pixel'

## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'
fileName          = './0.data'

#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
ConditionsTag     = 'COMCOND-HLTP-004-01'
DetDescrVersion   = 'ATLAS-GEO-20-00-01'

doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

abortonuncheckedstatuscode = False

## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = False
doInDet     = True
doMuon      = False
doLArg      = False
doTile      = False
doTrigger   = False
doHist      = False
doJiveXML   = False
doEgammaTau = False

#set to True in the JO
#doCommissioning = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = False
doPixelOnlyMon = True
doCaloMon = False
doPhysMon = False
doTrigMon = False
doIDMon   = False
doTRTMon  = False
doSCTMon  = False
doMuonMon = False
doLucidMon = False
## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

