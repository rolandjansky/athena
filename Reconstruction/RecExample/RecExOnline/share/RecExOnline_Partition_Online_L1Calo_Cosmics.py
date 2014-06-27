## Job options for Running the L1Calo Athena Online Monitoring
## A few notes:
## At the moment one needs to edit the RecExOnline_monitoring.py 
##    and define the doLVL1CaloMon variable and replace doTrigMon
##    in the statement:
##       DQMonFlags.useTrigger(doTrigMon)
##       DQMonFlags.doLVL1CaloMon(doTrigMon)
##    with doLVL1CaloMon. Then add doLVL1CaloMon to
##    the 'orMon' list.
## I think that is all you need. Cheers, Taylor


# used by RecExOnline to setup Emon Service
useEmon           = True


##################
## -- flags set in: RecExOnline_emonsvc.py (from RecExOnline_jobOptions.py)

import os

# set partition name (default is ATLAS)
partitionName = os.getenv("TDAQ_PARTITION","ATLAS")

# set name of this publisher as it will appear in IS
publishName       = "l1calo-athenaPT-cosmics"

# name of the stream type (physics,express, etc.)
streamType = os.getenv("L1CALO_PTIO_STREAM_TYPE","physics")

# name of the stream (Egamma,JetTauEtmiss,MinBias,Standby, etc.)
# this can be a colon(:) separated list of streams that use
# the 'streamLogic' to combine
streamName = os.getenv("L1CALO_PTIO_STREAM_NAME","CosmicCalo")

# logic used to combine multiple streams
# can be set to 'Ignore' which means the stream selection isn't used
streamLogic = os.getenv("L1CALO_PTIO_STREAM_LOGIC","Or")
if(partitionName == "L1CaloStandalone"):
   streamLogic = "Ignore"

# name of L1 items to select
# can be colon(:) separated list of L1 items
lvl1Name          = ''

# logic used to combined multiple L1 items
# can be set to 'Ignore' which means the L1 selection isn't used
lvl1Logic         = 'Ignore'

# set the Sampler Key Type name (default is SFI)
if ( not os.environ.get("L1CALO_PTIO_KEY") ):
  if( partitionName == "L1CaloStandalone" ):
    keyname = 'REB'
  else:
    keyname = 'SFI'
else:
  keyname = os.environ.get("L1CALO_PTIO_KEY")

# set the Sampler count (default is 25)
keycount = int(os.environ.get("L1CALO_PTIO_KEY_COUNT","25"))

# event buffer size for each sampler
buffersize        = 10

# time in seconds between updating plots
updateperiod      = 30

# timeout (not sure what this does)
timeout           = 240000

# IS server onwhich to create this provider
isserverName    = 'Histogramming'

# this is not the global monitoring
isGlobalMonitoring = False





############
## -- flags set in: RecExOnline_globalconfig.py  (from RecExOnline_jobOptions.py)

#read the pickle file if you want to use the AMI tag info
#stored in ami_recotrf.pickle (produced by 'tct_getAmiTag.py f140 ami_recotrf.cmdargs ami_recotrf.pickle') 
#usePickleConfig   = False
#pickleconfigfile  = './ami_recotrf.pickle'
#DataSource        = 'data'
#InputFormat       = 'bytestream'
#fileName         = './0.data'
#doESD             = True
#writeESD          = False
#doAOD             = False
#writeAOD          = False
isOnlineStateless = True
beamType = 'collisions' # default: 'cosmics'
#is_T0_project_tag = 'cos010'

#COND tag and GEO are needed for running over a test partition or against ATLAS cosmics, calib
#ConditionsTag     = 'COMCOND-HLTP-004-01'
#DetDescrVersion   = 'ATLAS-GEO-20-00-01'

#doPixelOnlyMon   = False

# unlimit max number of events
evtMax            = -1

###########
## -- flags set in: RecExOnline_recoflags.py (from RecExOnline_jobOptions.py)
doAllReco   = False
# following set to default values from JOs
#doInDet     = doAllReco
#doMuon      = doAllReco
#doLArg      = doAllReco
#doTile      = doAllReco
#doLucid     = False
#doHist      = True
#doJiveXML   = False
#doEgammaTau = doAllReco

#doCommissioning = False
#abortonuncheckedstatuscode = False

# Found that this must be true for REB runs but not for SFI
if (partitionName == "L1CaloStandalone"):
   doTrigger = True #Default: False
else:
   doTrigger = False


################
## -- flags set in: RecExOnline_monitoring.py (from RecExOnline_jobOptions.py)

# don't need all the monitoring enabled just for L1Calo
doAllMon  = False
#doCaloMon = doAllMon
#doPhysMon = doAllMon
#doIDMon   = doAllMon
#doPixelOnlyMon = False
#doSCTMon  = doAllMon
#doMuonMon = doAllMon
#doTRTMon  = doAllMon
#doTrigMon = doAllMon
doLVL1CaloMon = True
#doHLTMon  = doTrigMon
#doCTPMon  = doTrigMon
#doLucidMon= doAllMon
isOnline  = True





## main online reco scripts
include ("RecExOnline/RecExOnline_jobOptions.py")


print ' '
print '-------------------------------------------------------------'
print '|     RecExOnline_globalconfig settings'
print '| usePickleConfig    = ',usePickleConfig
print '| DataSource         = ',DataSource
print '| InputFormat        = ',InputFormat
print '| fileName           = ',fileName
#print '| doESD              = ',doESD
print '| writeESD           = ',writeESD
#print '| doAOD              = ',doAOD
print '| writeAOD           = ',writeAOD
print '| isOnlineStateless  = ',isOnlineStateless
print '| beamType           = ',beamType
print '| is_T0_project_tag  = ',is_T0_project_tag
print '| ConditionsTag      = ',ConditionsTag
#print '| DetDescrVersion    = ',DetDescrVersion
print '| doPixelOnlyMon     = ',doPixelOnlyMon
print '| evtMax             = ',evtMax
print '-------------------------------------------------------------'
print '|     RecExOnline_recoflags settings'
print '| doAllReco          = ',doAllReco
print '| doInDet            = ',doInDet
print '| doMuon             = ',doMuon
print '| doLArg             = ',doLArg
print '| doTile             = ',doTile
print '| doLucid            = ',doLucid
#print '| doTrigger          = ',doTrigger
#print '| doHist             = ',doHist
#print '| doJiveXML          = ',doJiveXML
print '| doEgammaTau        = ',doEgammaTau
print '| doCommissioning    = ',doCommissioning
print '| abortonuncheckedstatuscode = ',abortonuncheckedstatuscode
print '-------------------------------------------------------------'
print '|     RecExOnline_monitoring settings'
print '| doAllMon           = ',doAllMon
print '| doCaloMon          = ',doCaloMon
print '| doPhysMon          = ',doPhysMon
print '| doHLTMon           = ',doHLTMon
print '| doLVL1CaloMon      = ',doLVL1CaloMon
print '| doCTPMon           = ',doCTPMon
print '| doIDMon            = ',doIDMon
print '| doPixelOnlyMon     = ',doPixelOnlyMon
print '| doSCTMon           = ',doSCTMon
print '| doMuonMon          = ',doMuonMon
print '| doTRTMon           = ',doTRTMon
print '| doLucidMon         = ',doLucidMon
print '| isOnline           = ',isOnline
print '-------------------------------------------------------------'
print '|     RecExOnline: emonsvc settings'
print '| partitionName      = ',partitionName
print '| publishName        = ',publishName
print '| streamNames        = ',streamName
print '| streamType         = ',streamType
print '| streamLogic        = ',streamLogic
print '| lvl1Name           = ',lvl1Name
print '| lvl1Logic          = ',lvl1Logic
print '| keyname            = ',keyname
print '| keycount           = ',keycount
print '| buffersize         = ',buffersize
print '| updateperiod       = ',updateperiod
print '| timeout            = ',timeout
print '| isserverName       = ',isserverName
print '| isGlobalMonitoring = ',isGlobalMonitoring
print '--------------------------------------------------------------'





# added for testing purposes
# need to for the run number in the test data partitions
#if (partitionName.find("L1CaloStandalone") >= 0) or (partitionName.find("Test_dataProvider") >= 0) :
#   print "L1Calo Monitoring is overriding the run number and lumiblock number."
#   svcMgr.IOVDbSvc.forceRunNumber=182519
#   svcMgr.IOVDbSvc.forceLumiblockNumber=1
#   print "L1Calo Monitoring set run to ",svcMgr.IOVDbSvc.forceRunNumber,"and lumi block to",svcMgr.IOVDbSvc.forceLumiblockNumber


from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
tf.configForStartup = "HLTonline"
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg = TriggerConfigGetter()
if rec.doLArg and rec.doTile:
  from AthenaCommon.AlgSequence import AlgSequence
  TTjob = AlgSequence()
  TTjob.TriggerTowerMaker.LVL1ConfigSvc = "Trig::TrigConfigSvc/TrigConfigSvc"
   
