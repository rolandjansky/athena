#example of personal topOptions
#
# to use it  
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
# 
# see RecExCommon/RecExCommon_flags.py for more available flags
#

# readG3 and AllAlgs needs be set before the include, since several
# secondary flags are configured according to that one
#
# readG3=True # true if read g3 data
# AllAlgs = False # if false, all algorithms are switched off by defaults 
# doiPatRec = False

doWriteESD=False
doWriteTAG=False
doWriteAOD=False

doTrackRecordFilter=False
doTrigger=False
doAOD=False
#doTruth=False

include ("RecExCommon/RecExCommon_flags.py")

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

DetDescrVersion="ATLAS-DC3-02"


doJetRec=False
doMissingET=False

# number of event to process
EvtMax=-1
#EvtMax=2

#PoolRDOInput= ["/castor/cern.ch/atlas/project/dc2/preprod/g4dig805/dc2.002885.pyt_z_ee.g4dig805/data/dc2.002885.pyt_z_ee.g4dig805._0001.pool.root"]

# include my own algorithm(s)
# include my own algorithm
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# user property modifiers should be at the end to override all defaults
# TrackParticleTruthMaker.Enable=False 

# very very temporary
# InDetPrepRawDataTruthMaker=Algorithm("InDetPrepRawDataTruthMaker")
# InDetPrepRawDataTruthMaker.Enable=False

#print memory usage for all events (in case of memory leak crash)
#CBNT_Audit.nEventPrintMem=999999
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

printfunc ("BEGIN_RTTINFO = 1103_T1_McAtNLO_top")
