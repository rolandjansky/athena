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

from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs
rec.doWriteESD=False
rec.doWriteTAG=False
rec.doWriteAOD=False

rec.doMuon=False
rec.doMuonCombined=False

recAlgs.doTrackRecordFilter=False
rec.doTrigger=False
#obsolete doAODLVL1=False
rec.doAOD=False
#doTruth=False

recAlgs.doMuTag=False

from JetRec.JetRecFlags import jetFlags
jetFlags.Enabled=False

rec.CBNTAthenaAware=True

# If tracking is off, this also needs to be off.
rec.doEgamma = False


#PoolRDOInput=[ "/afs/cern.ch/atlas/offline/data/testfile/q02initialprod.0001.H_2e2mu.q02dig_1004.etacut.0001_extract.pool.root" ]


jp.AthenaCommonFlags.EvtMax=5


include ("RecExCond/RecExCommon_flags.py")

rec.AutoConfiguration = ['everything']
import RecExConfig.AutoConfiguration as auto
auto.ConfigureFromListOfKeys(rec.AutoConfiguration())

# Flags that are defined in python are best set here
# switch off ID and muons
DetFlags.ID_setOff()
DetFlags.Muon_setOff()

rec.doNameAuditor=True

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# user property modifiers should be at the end to overridde all defaults
# TrackParticleTruthMaker.Enable=False 

# very very temporary
# InDetPrepRawDataTruthMaker=Algorithm("InDetPrepRawDataTruthMaker")
# InDetPrepRawDataTruthMaker.Enable=False

#print memory usage for all events (in case of memory leak crash)
#CBNT_Audit.nEventPrintMem=999999

MessageSvc.Format = "% F%40W%S%7W%R%T %0W%M"

print "RTTINFO = 1002_H_2e2mu_q02dig_1004"


#from __main__ import AtlasTrackingGeometrySvc
#AtlasTrackingGeometrySvc.AssignMaterialFromCOOL = False

