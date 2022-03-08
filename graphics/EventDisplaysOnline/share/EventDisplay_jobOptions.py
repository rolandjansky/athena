## -- Overview of all default local settings that one can change 
## -- The default values are also shown.

## ------------------------------------------- name of the partition from which to read data and configuration parameters
partitionName   = 'ATLAS'
#partitionName   = 'GMTestPartition' #Test partition serving events from a raw data file if you want to test when no run is ongoing.

## ------------------------------------------- set both the old flags in RecExOnline and the new flags consistently
beamType          = 'collisions'
#beamType          = 'cosmics'

## Pause this thread until the ATLAS partition is up
include ("EventDisplaysOnline/WaitForAtlas_jobOptions.py")

## ------------------------------------------- set online defaults for AthenaConfiguration.AllConfigFlags
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.AutoConfigOnlineRecoFlags import autoConfigOnlineRecoFlags
from AthenaConfiguration.Enums import BeamType
ConfigFlags.Trigger.triggerConfig = 'DB' # temporary 02/2021
autoConfigOnlineRecoFlags(ConfigFlags, partitionName)

## ------------------------------------------- update selected ConfigFlags if needed
ConfigFlags.Beam.Type = BeamType(beamType)

## ------------------------------------------- flags set in: RecExOnline_jobOptions.py  
isOnline          = True
isOnlineStateless = True

#Crashes online if you do not set this....
isOfflineTest     = False

#for the time being, running over file is not possible ONLINE (please see RecExOnline_File_Offline.py) 
useEmon           = True
#The number of machines per single monitoring task we run with helpfully labelled "keycount"
keycount          = 30
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

# Set a unique group name for ED jobs so that: Events are distributed between ED jobs with the same groupName
# and shared between different (non ED) groups.
groupName         = "EventDisplaysOnline"


## ------------------------------------------- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)                    
publishName     = 'EventDisplays'

if (partitionName == 'ATLAS'):
    evtMax            = -1

#Don't flood if you are running on a test loop
if (partitionName != 'ATLAS'):
    evtMax            = 200
    keycount          = 1

## ------------------------------------------- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)
#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
usePickleConfig   = False
pickleconfigfile  = './ami_recotrf.pickle'
DataSource        = 'data'
InputFormat       = 'bytestream'
fileName          = './0.data'

#COND tag
ConditionsTag     = 'CONDBR2-HLTP-2018-01'
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
doMuon      = doAllReco
doLArg      = doAllReco
doTile      = doAllReco
doTrigger   = doAllReco 
doHist      = False
doJiveXML   = False
doEgammaTau = doAllReco # lshi Feb 18 2022 enable this to get rid of Electron error

## ------------------------------------------ flags set in : RecExOnline_monitoring.py (from from RecExOnline_jobOptions.py)
doAllMon  = False
doCaloMon = doAllMon
doPhysMon = doAllMon
doTrigMon = doAllMon
doIDMon   = doAllMon
doTRTMon  = doAllMon
doMuonMon = doAllMon

## ------------------------------------------ flags set in : RecExOnline_postconfig.py    (called from RecExOnline_jobOptions.py)

## Define the general output directory for VP1 and Atlantis
if not 'OutputDirectory' in dir():
  OutputDirectory="/atlas/EventDisplayEvents"

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock(ConditionsTag)

## Setup unique output files (so that multiple Athenas on the same machine don't interfere)
jobId = os.environ.get('TDAQ_APPLICATION_NAME', '').split(':')
if not len(jobId) == 5:
    from random import randint
    jobId = ['Athena-EventProcessor', 'Athena-EventDisplays-Segment', 'EventDisplays-Rack', 'tmp', '%d' % randint(0, 999)]

IPC_timeout = int(os.environ['TDAQ_IPC_TIMEOUT'])
printfunc (" IPC_timeout Envrionment Variable = %d" %IPC_timeout)
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
ConfigFlags.LAr.doHVCorr = False # ATLASRECTS-6823

#Work around to stop crash in pixel cluster splitting (Updated by lshi 23 July 2020, ATLASRECTS-5496)
from InDetRecExample.InDetJobProperties import InDetFlags#All OK
InDetFlags.doPixelClusterSplitting.set_Value_and_Lock(False)

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doMissingET.set_Value_and_Lock(False)

## from Global Monitoring 12 Oct 2021
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetGeo.set_Value_and_Lock('atlas')
jobproperties.Beam.bunchSpacing.set_Value_and_Lock(25) # Needed for collisions

if (partitionName != 'ATLAS'): # Can't get some information from the test partition
     ConfigFlags.Input.RunNumber = [412343]
     ConfigFlags.Input.ProjectName = 'data22_cos'
     ## ERROR Missing ROBFragment with ID 0x760001 requested ATR-24151 13 Oct 2021 lshi
     ConfigFlags.Trigger.L1.doMuon=False;
     ConfigFlags.Trigger.L1.doCalo=False;
     ConfigFlags.Trigger.L1.doTopo=False;
     ConfigFlags.Trigger.doNavigationSlimming = False # ATR-24551

## Main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")

ToolSvc.InDetPixelRodDecoder.OutputLevel = ERROR

include ("EventDisplaysOnline/JiveXMLServer_jobOptions.py")
include ("EventDisplaysOnline/Atlantis_jobOptions.py")
#include ("EventDisplaysOnline/VP1_jobOptions.py")

## Disable histogramming
svcMgr.ByteStreamInputSvc.ISServer=''

if (partitionName != 'ATLAS'):
     svcMgr.ByteStreamInputSvc.KeyValue = [ 'Test_emon_push' ]

################### Added by sjiggins 10/03/15 as given by Peter Van Gemmeren for name PoolFileatalogs
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:PoolFileCatalog_%s_%s.xml" % (jobId[3], jobId[4])
####################################################################################################  

svcMgr.MessageSvc.OutputLevel = WARNING #INFO
svcMgr.MessageSvc.Format = "% F%t %18W%S%7W%R%T %0W%M"
