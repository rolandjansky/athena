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

import os
os.system('rm ESD.pool.root AOD.pool.root')

rec.doWriteESD=True
rec.doWriteTAG=False
rec.doWriteAOD=False
rec.doAOD=False
#obsolete doAODLVL1=False
rec.doTrigger=False

from RecExConfig.RecAlgsFlags import recAlgs
recAlgs.doMissingET=False

# number of event to process
jp.AthenaCommonFlags.EvtMax=5


include ("RecExCond/RecExCommon_flags.py")

rec.doMuon=False
rec.doMuonCombined=False

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()



# include my own algorithm(s)
# include my own algorithm
# UserAlgs=[ "MyPackage/MyAlgorithm_jobOptions.py" ] 

# no jet
#doJetRec=False

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# user property modifiers should be at the end to overridde all defaults
# TrackParticleTruthMaker.Enable=False 

# very very temporary
# InDetPrepRawDataTruthMaker=Algorithm("InDetPrepRawDataTruthMaker")
# InDetPrepRawDataTruthMaker.Enable=False

#print memory usage for all events (in case of memory leak crash)
#PoolRDOInput="/castor/cern.ch/user/c/costanzo/validation/data/dc2val.e_e100_eta25.006005.g4dig/dc2val.e_e100_eta25.006005.g4dig.0019.root"
MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

