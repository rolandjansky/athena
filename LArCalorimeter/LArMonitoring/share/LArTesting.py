RunNumber=300000
GlobalTag="CONDBR2-BLKPA-2015-05"
date="2015-10-18:02:00:00"
###################################################################
from time import strptime,time
from calendar import timegm

ts=strptime(date+'/UTC','%Y-%m-%d:%H:%M:%S/%Z')
TimeStamp=int(timegm(ts))*1000000000L

from RecExConfig.RecFlags import rec
rec.RunNumber.set_Value_and_Lock(RunNumber)

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
#DetFlags.Tile_setOn()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')
globalflags.DatabaseInstance="CONDBR2"

# Get a handle to the default top-level algorithm sequence
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

# Setup Db stuff
import AthenaPoolCnvSvc.AthenaPool

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-20-00-01'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

svcMgr.IOVDbSvc.GlobalTag = GlobalTag
try:
   svcMgr.IOVDbSvc.DBInstance=""
except: 
   pass

include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

include( "LArCondAthenaPool/LArCondAthenaPool_joboptions.py" )

from LArConditionsCommon import LArHVDB #Sets HV Calbling and DCS Database folders
#conddb.addOverride("/LAR/IdentifierOfl/HVLineToElectrodeMap","LARIdentifierOflHVLineToElectrodeMap-UPD3-00")
from LArCondUtils.LArCondUtilsConf import LArHVToolDB
theLArHVToolDB = LArHVToolDB("LArHVToolDB")
ToolSvc += theLArHVToolDB

from LArMonitoring.LArMonitoringConf import LArTesting
theLArAlg=LArTesting()

topSequence+=theLArAlg

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 0
#svcMgr.EventSelector.InitialTimeStamp  = int(TimeStamp/1e9)
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

