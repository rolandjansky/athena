###############################################################
#
# Set Flags for Running the Trigger in readBS Mode
# includes Trigger_topOptions.py to do the real work.
# Reads from file RawEvent.re (file must already exist;
# run athena with TrigWriteBS_Flags.py to produce it)
# 
# use:
# athena.py -bs testAllReadBSMemAudit.py
#==============================================================
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

rec.doDetailedAuditor = True
# no. of times to loop over same file
repeatFile = 20
acf.EvtMax = 10000
#

#include( "TriggerRelease/TrigReadBS_Flags.py" )
include( "TriggerTest/testIDreadBS_Flags.py" )
#include( "TriggerTest/testMuonReadBS_Flags.py" )
#include( "TriggerTest/testCaloReadBS_Flags.py" )
theApp.EvtMax = 10000
MessageSvc.defaultLimit = (repeatFile+1)*theApp.EvtMax
ChronoStatSvc.NumberOfSkippedEventsForMemStat = 50
#
#if doDetailedAuditor:
#    theApp.AuditTools=True # FIXME crash on finalisation
#
# change the input file
ByteStreamEventStorageInputSvc.OutputLevel = DEBUG
#BSRDOInput=["rfio:/eos/atlas/user/d/demelian/bs/rome/lumi01/004022/rome.004022.lumi01.ele_25_00001_bs.data"]
# 50 events per file
acf.BSRDOInput=["/eos/atlas/user/d/demelian/bs/rome/dijets17/004814/rome.004814.JF17_pythia_jet_filter_00001_bs.data"]
BSRDOInput+=["/eos/atlas/user/d/demelian/bs/rome/dijets17/004814/rome.004814.JF17_pythia_jet_filter_00002_bs.data"]
BSRDOInput+=["/eos/atlas/user/d/demelian/bs/rome/dijets17/004814/rome.004814.JF17_pythia_jet_filter_00003_bs.data"]
BSRDOInput+=["/eos/atlas/user/d/demelian/bs/rome/dijets17/004814/rome.004814.JF17_pythia_jet_filter_00004_bs.data"]
BSRDOInput+=["/eos/atlas/user/d/demelian/bs/rome/dijets17/004814/rome.004814.JF17_pythia_jet_filter_00005_bs.data"]
#BSRDOInput=["/afs/cern.ch/atlas/offline/data/testfile/ele25_digi_Rome_10ev_test_EventStorage.data"]
while repeatFile > 0:
	BSRDOInput += [BSRDOInput[0]]
	BSRDOInput += [BSRDOInput[1]]
	BSRDOInput += [BSRDOInput[2]]
	BSRDOInput += [BSRDOInput[3]]
	BSRDOInput += [BSRDOInput[4]]
	repeatFile -= 1
if 'theRTTisRunningMe' in dir() and theRTTisRunningMe:
	print "input data to be set up by RTT"
	del ByteStreamEventStorageInputSvc
else:
	ByteStreamEventStorageInputSvc.FullFileName = BSRDOInput
	print ByteStreamEventStorageInputSvc.FullFileName

# algorithm to measure vmem size
theApp.DLLs += [ "TrigTestTools" ]
theApp.TopAlg += [ "MemAuditAlg" ]

#print EvtMax 
#print theApp.EvtMax
