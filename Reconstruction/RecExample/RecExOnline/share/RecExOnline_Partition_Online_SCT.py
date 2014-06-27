
## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py)
useEmon           = True
keycount          = 10
buffersize        = 10
updateperiod      = 50
timeout           = 240000
streamName        = 'L1Calo'
streamLogic       = 'Ignore'

useAtlantisEmon   = False
evtMax            = -1

## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)
## SCT Options
import os
partitionName = os.environ.get("TDAQ_PARTITION", "TestDataProvider_TCT")
isserverName = 'Histogramming-SCT-iss'

publishNumber = os.environ.get("JOB_NUMBER", "1")
streamName = os.environ.get("STREAM_NAME")

if ( publishNumber == 'WarmStart' ):
  publishName = "SCT-"+publishNumber
  publishTimeout = 3600000
else:
  publishName = "SCT-PT-"+publishNumber
#  streamName = 'MinBias'
  streamName = 'CosmicCalo:CosmicCaloEM:L1Calo:L1CaloEM:MinBias:MuonswBeam:RNDM'
#  streamName = 'MinBias'

print publishName
print 'Hi. the streamName = '
print streamName

## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'
#fileName         = './0.data'

#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
ConditionsTag     = 'COMCOND-HLTP-004-01'
DetDescrVersion   = 'ATLAS-GEO-20-00-01'
beamType = 'collisions'

doESD             = True
writeESD          = False
doAOD             = False
writeAOD          = False
IOVDbSvcMessage   = False

abortonuncheckedstatuscode = False

## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = False
doInDet     = True
doMuon      = doAllReco
doLArg      = doAllReco
doTile      = doAllReco
doTrigger   = False
doHist      = doAllReco
doJiveXML   = False
doEgammaTau = doAllReco

#set to True in the JO
#doCommissioning = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = False
doCaloMon = doAllMon
doPhysMon = doAllMon
doTrigMon = False
doIDMon   = False
doSCTMon  = True
doTRTMon  = doAllMon
doMuonMon = doAllMon

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

