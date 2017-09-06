# this will triogger IOV to retriev CaliWave
RunNumber = '00110555'
IOVBegin = 110555
Partition = "EB-EMBA"
SubDet = 'Barrel'
GainList = [ "MEDIUM" ]

#############################################################################################
## this cool channel selection is using to Read only BarrelPS Medium gain Delay from the DB
ChannelSelection='<channelSelection>385:416,1013</channelSelection>'
#############################################################################################


## where you want to put the outputfile
#OutputOFCRootFileDir= ""
#OutputOFCPoolFileDir= ""
#OutputShapePoolFileDir= ""

WriteNtuple = True

#########################
# to read official DB
#########################
# by default, 
# nothing to do it will pickup the GlobalTag


#######################################
# to read COOL in a local SQlite file 
#######################################

## AutoCorr
#######################
## local SQlite file
#InputAutoCorrSQLiteFile = "myDB200.db"
# the folder tag
#AutoCorrLArCalibFolderTag = "-UPD3-00"
################################################################################
# >>>>>>>>>> you NEED to have your POOL files inserter in the  PoolFileCatalog
################################################################################

## PhysWave
#######################
## local SQlite file
#InputPhysWaveSQLiteFile = "myDB200.db"
# the folder tag
#PhysWaveLArCalibFolderTag = "-UPD3-00"
################################################################################
# >>>>>>>>>> you NEED to have your POOL files inserter in the  PoolFileCatalo
################################################################################

## Bad Channel
#######################
#InputBadChannelSQLiteFile = "myDB200.db"
# the folder tag
#BadChannelsLArCalibFolderTag = "-UPD3-00"
#MissingFEBsLArCalibFolderTag = "-empty"

######################################

include("LArCalibProcessing/LArCalib_OFC_Phys_jobOptions.py")

#######################################

LArPhysOFCAlg.OutputLevel = INFO
LArOFC2Ntuple.OutputLevel = DEBUG
LArPhysWaveShifter.OutputLevel  = INFO

svcMgr.IOVDbSvc.OutputLevel = DEBUG    


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

