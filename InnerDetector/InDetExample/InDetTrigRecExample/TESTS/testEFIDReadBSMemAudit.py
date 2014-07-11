# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
doDetailedAuditor = True
# no. of times to loop over same file
repeatFile = 20
#
include( "testEFIDreadBS_Flags.py" )
#EventSelector.SkipEvents = 99
MessageSvc.OutputLevel = INFO

theApp.EvtMax = 10000
MessageSvc.defaultLimit = 100000000  #(repeatFile+1)*theApp.EvtMax
ChronoStatSvc.NumberOfSkippedEventsForMemStat = 50
#
#if doDetailedAuditor:
#    theApp.AuditTools=True # FIXME crash on finalisation
#
# change the input file
#ByteStreamEventStorageInputSvc.OutputLevel = ERROR
ByteStreamInputSvc.OutputLevel = ERROR

DetDescrVersion = "ATLAS-DC3-02"
# 100 events per file
BSRDOInput=["rfio:/castor/cern.ch/user/d/demelian/bs/12.0.3/004100/mc11.004100.T1_McAtNLO_top.v11000401_120003._00001.bs.data"]
#BSRDOInput+=["rfio:/castor/cern.ch/user/d/demelian/bs/12.0.3/004100/mc11.004100.T1_McAtNLO_top.v11000401_120003._00002.bs.data"]
#BSRDOInput+=["rfio:/castor/cern.ch/user/d/demelian/bs/12.0.3/004100/mc11.004100.T1_McAtNLO_top.v11000401_120003._00003.bs.data"]
#BSRDOInput+=["rfio:/castor/cern.ch/user/d/demelian/bs/12.0.3/004100/mc11.004100.T1_McAtNLO_top.v11000401_120003._00004.bs.data"]
#BSRDOInput+=["rfio:/castor/cern.ch/user/d/demelian/bs/12.0.3/004100/mc11.004100.T1_McAtNLO_top.v11000401_120003._00005.bs.data"]
#BSRDOInput+=["rfio:/castor/cern.ch/user/d/demelian/bs/12.0.3/004100/mc11.004100.T1_McAtNLO_top.v11000401_120003._00006.bs.data"]
#to run on pcatlas00X uncomment it:
#BSRDOInput=["/space1/igrabows/data/top-bs/mc11.004100.T1_McAtNLO_top.v11000401_120003._00001.bs.data"]
#BSRDOInput+=["/space1/igrabows/data/top-bs/mc11.004100.T1_McAtNLO_top.v11000401_120003._00002.bs.data"]
#BSRDOInput+=["/space1/igrabows/data/top-bs/mc11.004100.T1_McAtNLO_top.v11000401_120003._00003.bs.data"]
#BSRDOInput+=["/space1/igrabows/data/top-bs/mc11.004100.T1_McAtNLO_top.v11000401_120003._00004.bs.data"]
#BSRDOInput+=["/space1/igrabows/data/top-bs/mc11.004100.T1_McAtNLO_top.v11000401_120005._00005.bs.data"]
#BSRDOInput+=["/space1/igrabows/data/top-bs/mc11.004100.T1_McAtNLO_top.v11000401_120003._00006.bs.data"]
#single RoI event
#BSRDOInput=["/space/igrabows/e25i-bs/mc11.004022.Electron_Pt_25.v11000401_120003._00001.bs.data"]
#BSRDOInput+=["/space/igrabows/e25i-bs/mc11.004022.Electron_Pt_25.v11000401_120003._00001.bs.data"]
#BSRDOInput+=["/space/igrabows/e25i-bs/mc11.004022.Electron_Pt_25.v11000401_120003._00001.bs.data"]
#BSRDOInput+=["/space/igrabows/e25i-bs/mc11.004022.Electron_Pt_25.v11000401_120003._00001.bs.data"]
while repeatFile > 0:
	BSRDOInput += [BSRDOInput[0]]
	BSRDOInput += [BSRDOInput[1]]
	BSRDOInput += [BSRDOInput[2]]
	BSRDOInput += [BSRDOInput[3]]
	BSRDOInput += [BSRDOInput[4]]
	BSRDOInput += [BSRDOInput[5]]
	repeatFile -= 1
if 'theRTTisRunningMe' in dir() and theRTTisRunningMe:
	print "input data to be set up by RTT"
	del ByteStreamInputSvc
else:
	ByteStreamInputSvc.FullFileName = BSRDOInput
	print ByteStreamInputSvc.FullFileName
# algorithm to measure vmem size
theApp.DLLs += [ "TrigTestTools" ]
theApp.TopAlg += [ "MemAuditAlg" ]

# Turn Off History Service for StoreGate
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.StoreGateSvc.ActivateHistory = False

HistorySvc = Service ( "HistorySvc" )
HistorySvc.Activate=False

