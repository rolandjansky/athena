#######################################
# you can choose the number of event
#theApp.EvtMax = 500
#######################################

InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2009/00110555"
RunNumberList = [ '00110555' ]
IOVBegin =  110555
Partition = "EB-EMBA"
SubDet = 'Barrel'
GainList = [ "MEDIUM" ]

## where you want to put the outputfile
#OutputCaliWaveRootFileDir  =""
#OutputCaliWavePoolFileDir  =""

StripsXtalkCorr = True
WriteNtuple = True

###################################################################
# Check for empty (missing) phases
# If found in at least one channel, will abort run
# Don't enable if you;re running on a subset of events!
###################################################################
CheckEmptyPhases = False


#########################
# to read official DB
#########################
# by default, 
# nothing to do it will pickup the GlobalTag


#######################################
# to read COOL in a local SQlite file 
#######################################


## Pedestal
#######################
## local SQlite file
#InputPedSQLiteFile = "myDB200.db"
# the folder tag
#PedLArCalibFolderTag = "-UPD3-00"
################################################################################
# >>>>>>>>>> you NEED to have your POOL files inserter in the  PoolFileCatalog
################################################################################


## Bad Channel
#######################
#InputBadChannelSQLiteFile = "myDB200.db"
# the folder tag
#BadChannelsLArCalibFolderTag = "-UPD3-00"
#MissingFEBsLArCalibFolderTag = "-empty"

#######################################

include("LArCalibProcessing/LArCalib_Delay_jobOptions.py")

#######################################

if ( StripsXtalkCorr ) :
    theLArStripsCrossTalkCorrector.OutputLevel = INFO

LArCaliWaveBuilder.OutputLevel = INFO

if ( WriteNtuple ) :
    LArCaliWaves2Ntuple.OutputLevel = INFO

if (doCaliWaveSelector) :
    LArCaliWaveSelector.OutputLevel = INFO

## LArStripsCrossTalkCorrection 
if (StripsXtalkCorr == True) :
    theLArStripsCrossTalkCorrector.OutputLevel = INFO

## LArMonTool
if ( doMonitoring ) :
    ToolSvc.LArFEBMon.OutputLevel = INFO
    
## LArCaliWave
LArCaliWaveBuilder.OutputLevel = INFO
LArCaliWaves2Ntuple.OutputLevel = INFO

svcMgr.MessageSvc.OutputLevel = ERROR

svcMgr.IOVDbSvc.OutputLevel = INFO
    

########################
#        options
#########################
#------------
# Message
#------------
#svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
#svcMgr.MessageSvc.OutputLevel = VERBOSE

#from StoreGate.StoreGateConf import StoreGateSvc
#----------------
# Dump StoreGate
#----------------
#sgStore = StoreGateSvc("StoreGateSvc")
#sgStore.Dump = True
#sgStore.OutputLevel = DEBUG

#--------------------
# Dump DetectorStore
#--------------------
#dtStore = StoreGateSvc("DetectorStore")
#dtStore.Dump = True
#dtStore.OutputLevel = DEBUG

#----------------------
# Dump ConditionStore
#----------------------
#cdStore = StoreGateSvc("ConditionStore")
#cdStore.Dump = True
#cdStore.OutputLevel = DEBUG

