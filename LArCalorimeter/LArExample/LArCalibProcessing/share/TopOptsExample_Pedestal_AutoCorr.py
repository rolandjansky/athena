#######################################
# you can choose the number of event
#theApp.EvtMax = 500

#######################################

InputDir="/castor/cern.ch/grid/atlas/DAQ/lar/ElecCalib/2009/00110554"
IOVBegin = 110554
RunNumberList = [ '00110554' ]
SubDet = "Barrel"
Partition = "EB-EMBA"
GainList = ["MEDIUM"]
runAccumulator = False

## where you want to put the outputfile
#OutputPedAutoCorrRootFileDir =""
#OutputPedPoolFileDir = ""
#OutputAutoCorrPoolFileDir= ""

WriteNtuple = True

#########################
# to read official DB
#########################
# by default, 
# nothing to do it will pickup the GlobalTag


#######################################
# to read COOL in a local SQlite file 
#######################################

## Bad Channel
#######################
#InputBadChannelSQLiteFile = "myDB200.db"
# the folder tag
#BadChannelsLArCalibFolderTag = "-UPD3-00"
#MissingFEBsLArCalibFolderTag = "-empty"


#######################################

include("LArCalibProcessing/LArCalib_PedestalAutoCorr_jobOptions.py")

#######################################

## LArPedestal
if ( Pedestal )  :
    if runAccumulator :
        LArPedestalMaker.OutputLevel = INFO
    else :
        LArPedACBuilder.OutputLevel = INFO
    LArPedestals2Ntuple.OutputLevel = INFO

## LArAutoCorr    
if ( AutoCorr ) :
    if runAccumulator :
        LArAutoCorrMaker.OutputLevel = INFO
    else :
        LArPedACBuilder.OutputLevel = INFO
    LArAutoCorr2Ntuple.OutputLevel = INFO

## LArMonTool
if ( doMonitoring ) :
    if (doLArFEBMon) :
        theLArFEBMon.OutputLevel = INFO
        
    if (doLArOddCellMonTool) :
        theLArOddCellsMonTool.OutputLevel = INFO
        theLArOddCellsMonTool.minimumRefNoise = [0.5,0.5]

    if (doLArScaNoiseTool) :
        theLArScaNoiseMonTool.OutputLevel = INFO
        
    if (doLArDigitNoiseMonTool) :
        theLArDigitNoiseMon.OutputLevel = INFO
    

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
