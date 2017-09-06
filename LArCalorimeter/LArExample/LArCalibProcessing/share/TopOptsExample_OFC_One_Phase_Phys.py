# this will triogger IOV to retriev AutoCorr and CaliWave
RunNumber = 69129
Partition = "EB-EMBA"
SubDet = 'Barrel'
GainList = [ "HIGH" ]

## where you want to put the outputfile
#OutputOFCRootFileDir= ""
#OutputOFCPoolFileDir= ""

#########################
# to read official DB
#########################
# by default, 
# nothing to do it will pickup the GlobalTag


#######################################
# to read COOL in a local SQlite file 
#######################################

## OFC
#######################
## local SQlite file
#InputOFCSQLiteFile = "myDB200.db"
#InputOFCSQLiteFile = "/afs/cern.ch/user/l/larcalib/w0/data/WorkingDirectory/69125_69129_69135_Barrel_24/poolFiles/myDB200_69125_69129_69135_EB-EMBA_OFCPhys.db"
# the folder tag
#OFCLArCalibFolderTag = "-comm-11"
# >>>>>>>>>> you NEED to have your POOL files inserter in the  PoolFileCatalog

#########################
# to read a POOL file
#########################

## OFC
########################
#ReadAutoCorrFromCOOL = False
# extern POOL file with the location
#InputAutoCorrPoolDir = ""
#InputAutoCorrPoolFileName = ""

#######################################

include ( "LArCalibProcessing/LArCalib_PickOFCOnePhase.py" )

#######################################


LArOFPhasePicker.OutputLevel = DEBUG
LArOFC2Ntuple.OutputLevel = DEBUG

#########################
#        options
#########################

#------------
# Message
#------------
#svcMgr.MessageSvc = Service( "MessageSvc" )
#svcMgr.MessageSvc.defaultLimit = 9999999  # all messages
#svcMgr.MessageSvc.OutputLevel = VERBOSE

#----------------
# Dump StoreGate
#----------------
#svcMgr.StoreGateSvc = Service("StoreGateSvc" )
#svcMgr.StoreGateSvc.Dump = True
#svcMgr.StoreGateSvc.OutputLevel = DEBUG

#--------------------
# Dump DetectorStore
#--------------------
#svcMgr.DetectorStore = Service("DetectorStore" )
#svcMgr.DetectorStore.Dump = True
#svcMgr.DetectorStore.OutputLevel = DEBUG

#----------------------
# Dump ConditionStore
#----------------------
#svcMgr.ConditionStore  = Service("ConditionStore" )
#svcMgr.ConditionStore.Dump = True
#svcMgr.ConditionStore.OutputLevel = DEBUG
