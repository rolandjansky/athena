#
# Job Options for reading LAr HV Pathologies from COOL using local SQLite database
#

if "GloablTag" not in dir():
    GlobalTag = 'COMCOND-BLKPA-006-01'



## basic job configuration
import AthenaCommon.AtlasUnixGeneratorJob

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

## get a handle to the default top-level algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## get a handle to the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

## get a handle to the ApplicationManager
from AthenaCommon.AppMgr import theApp

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo = 'atlas'
globalflags.DataSource = 'data'
globalflags.InputFormat = 'pool'

from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.DetDescrVersion='ATLAS-GEO-20-00-00'

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
from AtlasGeoModel import SetupRecoGeometry

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

TileUseDCS=False
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")


#--------------------------------------------------------------
# Access to IOVSvc, IOVDbSvc and CondDBMySQLCnvSvc
#--------------------------------------------------------------
import IOVDbSvc.IOVDb

from LArCondUtils.LArCondUtilsConf import LArHVPathologyDbAlg
LArHVPathologyDbAlg = LArHVPathologyDbAlg()
LArHVPathologyDbAlg.Folder = "/LAR/HVPathologiesOfl/Pathologies"
topSequence += LArHVPathologyDbAlg

# Here mytest.db is the name of SQLite file created by this job
svcMgr.IOVDbSvc.dbConnection  = "sqlite://;schema=larhvpathology.db;dbname=COMP200"
svcMgr.IOVDbSvc.Folders += [ "/LAR/HVPathologiesOfl/Pathologies<tag>LARHVPathologiesOflPathologies-UPD1-00</tag>" ]
    
svcMgr.MessageSvc.OutputLevel = 4
svcMgr.MessageSvc.debugLimit  = 100000
svcMgr.MessageSvc.infoLimit   = 100000

LArHVPathologyDbAlg.OutputLevel     = 3
svcMgr.IOVDbSvc.OutputLevel         = 3

#--------------------------------------------------------------
# The following turns off the ability to set EventSelector parameters
# below
#ApplicationMgr.EvtSel = "NONE"
#--------------------------------------------------------------
# Set the following when reading back to adjust the run/event numbers
svcMgr.EventSelector.RunNumber         = 9999999
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                   = 1

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

# Check the dictionary in memory for completeness
svcMgr.AthenaSealSvc.CheckDictionary = TRUE
