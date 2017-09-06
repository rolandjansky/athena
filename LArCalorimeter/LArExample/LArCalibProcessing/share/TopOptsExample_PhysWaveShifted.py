
# this will triogger IOV to retriev CaliWave
RunNumber = 29142 
IOVBegin = 29142
Partition = "EB-EMBA"
SubDet = 'Barrel'

#########################
# to read official DB
#########################
# by default, 
# nothing to do

#######################################
# to read COOL in a local SQlite file 
#######################################

# local SQlite file
#InputPhysWaveSQLiteFile = "myDB200.db"

# or a extern SQlite file with the location 
#InputPhysWaveSQLiteFile = "myDB200_29142_29143_29146.db"
# if necessary give the folder tag
#PhysWaveLArCalibFolderTag = "-comm-05"
# you need to have your POOL files inserter in the  PoolFileCatalog

#########################
# to read a POOL file
#########################
#ReadPhysWaveFromCOOL = False

# or a extern POOL file with the location
#InputPhysWavePoolDir = "/afs/cern.ch/user/t/tarrade/scratch0/CommissioningMonitoring/AtlasOfflineRelease/Calibration/13.0.30/run"
#InputPhysWavePoolFileName = "FCalWave.pool.root"

#InputPhysWavePoolDir = "/tmp/tarrade"
#InputPhysWavePoolFileName = "LArPhysWave_RTM_29142_EB-EMBA.pool.root"

# you need to have your POOL files inserter in the  PoolFileCatalog
#######################################

include("LArCalibProcessing/LArCalib_PhysWaveShifter_jobOptions.py")

LArPhysWaveShifter.OutputLevel  = DEBUG


########################
#        options
#########################
#------------
# Message
#------------
#svcMgr.MessageSvc.OutputLevel  = WARNING
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
