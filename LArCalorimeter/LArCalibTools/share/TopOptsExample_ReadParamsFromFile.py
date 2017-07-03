DataStructure = "LArCaliPulseParamsVsCalib"
DetStoreKey   = "LArCaliPulseParams"
ChannelIdType = "OnlineCalibID"

AsciiFileName = "/afs/cern.ch/user/m/mdelmast/scratch0/Athena/p3c/params/AllBoards.dat"

PoolFileName  = "LArCaliPulseParamsVsCalib_AllBoards.pool.root"
RootFileName  = "LArCaliPulseParamsVsCalib_AllBoards.root"

IOVBegin  = 0

ParamsFolderName = "/LAR/ElecCalib/CaliPulseParams/Measured"
#ParamsFolderName = "/LAR/ElecCalib/CaliPulseParams/MeasuredCorrected"

OutputSQLiteFile = "LArCaliPulseParamsVsCalib_AllBoards.db"

include("LArCalibTools/LArReadParamsFromFile_jobOptions.py")

LArReadFile.OutputLevel   = VERBOSE

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
