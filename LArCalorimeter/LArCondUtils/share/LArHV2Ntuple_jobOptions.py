##################################################################
# 
# example jobOptions to read HV from Cool/DCS in athena and fill simple ntuple
#
##################################################################

from time import strptime,time
from calendar import timegm

if "OutFile" not in dir():
   OutFile="dump_hv.root"

if "date" not in dir():
   date="2015-05-29:12:00:00"

if "TimeStamp" not in dir():
   try:
      ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
      TimeStamp=int(timegm(ts))*1000000000L
   except ValueError:
      print "ERROR in time specification, use e.g. 2007-05-25:14:01:00"

# run number only relevant for HV mapping information..   Use latest mapping here
if "RunNumber" not in dir():
   RunNumber = 999999

if "addCells" not in dir():
   addCells=False

if "GloablTag" not in dir():
   GlobalTag = 'CONDBR2-BLKPA-2016-14'


###################################################################
## basic job configuration
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Calo_setOn()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo='atlas'
globalflags.DataSource='data'
globalflags.InputFormat = 'bytestream'
globalflags.DatabaseInstance="CONDBR2"

# Get a handle to the default top-level algorithm sequence
#from AthenaCommon.AppMgr import ToolSvc


# Setup Db stuff
#import AthenaPoolCnvSvc.AthenaPool

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-20-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
#include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
#include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

#include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

from IOVDbSvc.CondDB import conddb 
conddb.addFolder("LAR_OFL","/LAR/IdentifierOfl/HVLineToElectrodeMap");
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREl/I16")
conddb.addFolder("DCS_OFL","/LAR/DCS/HV/BARREL/I8")

from LArCondUtils.LArCondUtilsConf import LArHV2Ntuple
theLArHV2Ntuple = LArHV2Ntuple("LArHV2Ntuple")
theLArHV2Ntuple.AddCellID=addCells
topSequence += theLArHV2Ntuple

#  ------------------------------------------------------------------
# ---  Ntuple 
# ------------------------------------------------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   svcMgr += THistSvc()

svcMgr.THistSvc.Output  = ["file1 DATAFILE='"+OutFile+"' OPT='RECREATE'"];



#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 100
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 100
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = int(TimeStamp/1e9)
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1

#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
svcMgr.IOVDbSvc.OutputLevel        = INFO
