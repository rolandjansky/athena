## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True

#Crashes online if you do not set this....
isOfflineTest     = False

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py) 
useEmon           = True
#The number of machines we run with helpfully labelled "keycount"
keycount          = 10 #10 #1000
buffersize        = 10
updateperiod      = 200
timeout           = 600000

keyname           = 'dcm'

#Blank to read all
streamName          = ''

#Read Physics
streamType        = 'physics'          #Progonal Does not specify  these


streamLogic       = 'Or'#HAS TO BE OR AT ALL TIMES WHEN SPECIFYING A CERTAIN STREAM

useAtlantisEmon   = False


## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)                    
partitionName   = 'ATLAS'
#Current test partition looping through 2015 data if you want to test when no run is ongoing.
#partitionName   = 'GMTestPartition_lshi_tdaq6'
publishName     = 'EventDisplays'

if (partitionName == 'ATLAS'):
    evtMax            = -1

#Don't flood if you are running on a test loop
if (partitionName != 'ATLAS'):
    evtMax            = 200

## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'
fileName          = './0.data'
#beamType          = 'cosmics'
#beamType          = 'collisions'

#COND tag and GEO are needed for running over a test partition online
#Previous COND tag
ConditionsTag     = 'CONDBR2-HLTP-2017-03' #Removed 07/04
#Swapped to this following AMI tag for current reco. Swap back if not in release
#ConditionsTag     = 'CONDBR2-ES1PA-2016-01' #Different
#Current DetDesc
DetDescrVersion   = 'ATLAS-R2-2016-01-00-01'

doESD             = True
writeESD          = True # False - Jiggins_12Feb_v2 working version switch 
doAOD             = False #True  # False - Jiggins_12Feb_v2 wokring version switch
writeAOD          = False # True  # False - Jiggins_12Feb_v2 working version switch
IOVDbSvcMessage   = False

## ------------------------------------------ flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = True
doInDet     = doAllReco
doMuon      = True
doLArg      = doAllReco
doTile      = doAllReco
doTrigger   = doAllReco 
doHist      = False
doJiveXML   = False
doEgammaTau = False

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = False
doCaloMon = doAllMon
doPhysMon = doAllMon
doTrigMon = False
doIDMon   = doAllMon
doTRTMon  = doAllMon
doMuonMon = False

doIDMon   = doAllMon
doTRTMon  = doAllMon
doMuonMon = False

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

## Define the general output directory for VP1 and Atlantis
if not 'OutputDirectory' in dir():
  OutputDirectory="/atlas/EventDisplayEvents"

## Pause this thread until the ATLAS partition is up
include ("EventDisplaysOnline/WaitForAtlas_jobOptions.py")

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)

## Setup unique output files (so that multiple Athenas on the same machine don't interfere)
jobId = os.environ.get('TDAQ_APPLICATION_NAME', '').split(':')
if not len(jobId) == 5:
    from random import randint
    jobId = ['Athena-EventProcessor', 'Athena-EventDisplays-Segment', 'EventDisplays-Rack', 'tmp', '%d' % randint(0, 999)]

IPC_timeout = int(os.environ['TDAQ_IPC_TIMEOUT'])
print " IPC_timeout Envrionment Variable = %d" %IPC_timeout
#################################################################################

#from random import randrange
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

# #################### From JiveXML server python script ####################
athenaCommonFlags.PoolESDOutput = "ESD-%s-%s.pool.root" % (jobId[3], jobId[4])                               
## Additional flags from: GetTfCommand.py --AMI=x392
from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doLArHVCorr=False
jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr.set_Value_and_Lock(False)
jobproperties.CaloCellFlags.doLArCreateMissingCells=False

#Work around to stop crash in pixel cluster splitting 
from InDetRecExample.InDetJobProperties import InDetFlags#All OK
InDetFlags.doInnerDetectorCommissioning.set_Value_and_Lock(True)

from JetRec.JetRecFlags import jetFlags
jetFlags.useTracks.set_Value_and_Lock(False)

#from MuonRecExample.MuonRecFlags import muonRecFlags; 
#Crashes claiming does not exist
#muonRecFlags.writeRDO.set_Value_and_Lock(True);

from RecExConfig.RecFlags import rec
#rec.projectName.set_Value_and_Lock('data16_comm') # CHECK THIS NAME WITH RUN CONTROL

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doEFlow.set_Value_and_Lock(False)
recAlgs.doMissingET.set_Value_and_Lock(False)

## Main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

ToolSvc.InDetPixelRodDecoder.OutputLevel = ERROR

rec.abortOnUncheckedStatusCode = False
rec.abortOnErrorMessage=False

include ("EventDisplaysOnline/JiveXMLServer_jobOptions.py")
include ("EventDisplaysOnline/Atlantis_jobOptions.py")
#include ("EventDisplaysOnline/VP1_jobOptions.py")

## Disable histogramming
svcMgr.ByteStreamInputSvc.ISServer=''

################### Added by sjiggins 10/03/15 as given by Peter Van Gemmeren for name PoolFileatalogs
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog_%s_%s.xml" % (jobId[3], jobId[4])
####################################################################################################  

svcMgr.MessageSvc.OutputLevel = WARNING #INFO
svcMgr.MessageSvc.Format = "% F%t %18W%S%7W%R%T %0W%M"
