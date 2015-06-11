##############################################################
# Job options read a LArRampComplete object from a POOL file #
# and dump it to an ROOT ntuple                              #
##############################################################

from time import strptime,time
from calendar import timegm


if "date" not in dir():
    date="2015-02-06:09:45:00"

if "TimeStamp" not in dir():
   try:
      ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
      TimeStamp=int(timegm(ts))*1000000000L
   except ValueError:
      print "ERROR in time specification, use e.g. 2007-05-25:14:01:00"
      

from LArCalibProcessing.TimeStampToRunLumi import TimeStampToRunLumi

rlb=TimeStampToRunLumi(TimeStamp)
if rlb is None:
   print "WARNING: Failed to convert time",TimeStamp,"into a run/lumi number"
   myRunNumber=999999
   myLumiBlock=0 
else:
   myRunNumber=rlb[0]
   myLumiBlock=rlb[1]

if "RunNumber" in dir():
   myRunNumber=RunNumber
   myLumiBlock=0

print "Working on run",myRunNumber,"LB",myLumiBlock,"Timestamp:",TimeStamp

if "GlobalTag" not in dir():
    GlobalTag     = 'CONDBR2-BLKPA-2015-05'

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.em_setOn()

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

# the Tile, LAr and Calo detector description package
DetDescrVersion = "ATLAS-R2-2015-03-01-00"

online = True
include ("LArConditionsCommon/LArMinimalSetup.py")

svcMgr.EventSelector.RunNumber=myRunNumber
svcMgr.EventSelector.EventsPerRun=1
svcMgr.EventSelector.FirstEvent=1

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

#from LArConditionsCommon.LArCondFlags import larCondFlags
#larCondFlags.SingleVersion = False
include ("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from LArCalibTools.LArCalibToolsConf import LArHVScaleCorr2Ntuple
theLArHVScaleCorr2Ntuple = LArHVScaleCorr2Ntuple("LArHVScaleCorr2Ntuple")
theLArHVScaleCorr2Ntuple.AddFEBTempInfo = False
topSequence += theLArHVScaleCorr2Ntuple

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import NTupleSvc
svcMgr += NTupleSvc()
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='hvcorr_read.root' OPT='NEW'" ]

theApp.EvtMax=1
svcMgr.MessageSvc.OutputLevel=INFO

#DetStore=Service("DetectorStore");
#svcMgr.DetStore.Dump=TRUE
