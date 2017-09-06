#######################################
# you can choose the number fo event
#theApp.EvtMax = 500
#######################################

InputDir = "/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2009/00110556"
RunNumberList = [ '00110556' ]
IOVBegin = 110556
Partition = "EB-EMBA"
SubDet = 'Barrel'
GainList = [ "MEDIUM" ]

#############################################################################################
## this cool channel selection is using to Read only BarrelPS Medium gain Delay from the DB
ChannelSelection='<channelSelection>385:416,1013</channelSelection>'
#############################################################################################

## where you want to put the outputfile
#OutputRampRootFileDir = ""
#OutputRampPoolFileDir = ""

isHEC = False
CorrectBadChannels   = True
ApplyAdHocCorrection = False
PeakOF = True
WriteNtuple = True

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
#################################################################################
# >>>>>>>>>> you NEED to have your POOL files inserter in the  PoolFileCatalog
#################################################################################

## OFC Cali
#######################
## local SQlite file
#InputOFCSQLiteFile = "myDB200.db"
# the folder tag
#OFCLArCalibFolderTag = "-UPD3-00"
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

include("LArCalibProcessing/LArCalib_Ramp_jobOptions.py")

#######################################

if ( CorrectBadChannels ) :
    theLArBadChannelTool.OutputLevel = INFO
    theLArRampPatcher.OutputLevel = INFO
    theLArRCBMasker.OutputLevel = DEBUG

## LArStripsCrossTalkCorrection
if (StripsXtalkCorr == True) :
    theLArStripsCrossTalkCorrector.OutputLevel = INFO

## LArBadChannel
if (CorrectBadChannels) :
    theLArRampPatcher.OutputLevel = INFO
    theLArRCBMasker.OutputLevel = DEBUG
    theLArBadChannelTool.OutputLevel =VERBOSE

## LArMonTool    
if ( doMonitoring ) :
    ToolSvc.LArFEBMon.OutputLevel = INFO

## LArRamp    
theLArOFPeakRecoTool.OutputLevel = VERBOSE
theLArRampBuilder.OutputLevel = INFO
LArRamps2Ntuple.OutputLevel = DEBUG

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
