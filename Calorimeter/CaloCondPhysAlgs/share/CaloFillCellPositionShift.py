###############################################################
#
# Job options file for CaloNoise2Ntuple
#
#==============================================================

# configuration for data, read noise from database through CaloNoiseToolDB

if 'GlobalTag' not in dir():
   GlobalTag =  'OFLCOND-RUN12-SDR-35'

if 'RunNumber' not in dir():
   RunNumber = 999999

if 'Geometry' not in dir():   
   Geometry = 'ATLAS-R2-2015-03-01-00'
 
if 'FolderTag' not in dir():
   FolderTag="LArCellPositionShift-ideal"
 
if 'OutputSQLiteFile' not in dir():   
   OutputSQLiteFile="LArCellPosition.db"

IOVRunNumberMin=0

from RecExConfig.RecFlags import rec
rec.RunNumber.set_Value_and_Lock(RunNumber)


from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
from AthenaCommon.Resilience import treatException,protectedInclude
protectedInclude( "PerfMonComps/PerfMonSvc_jobOptions.py" )

from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
DetFlags.LAr_setOn()
DetFlags.Tile_setOn()
DetFlags.digitize.all_setOff()

from AthenaCommon.GlobalFlags  import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('data')


import AthenaCommon.AtlasUnixGeneratorJob

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
jobproperties.Global.DetDescrVersion=Geometry

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include("TileConditions/TileConditions_jobOptions.py" )
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")
conddb.blockFolder("/LAR/LArCellPositionShift")

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloFillCellPositionShift
theFillCaloCellPositionShift = CaloFillCellPositionShift("CaloFillCellPositionShift")
topSequence += theFillCaloCellPositionShift

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1

from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
myOCA=OutputConditionsAlg("myOCA","LArCellPositionShift.pool.root")
myOCA.ObjectList = ["CaloRec::CaloCellPositionShift#LArCellPositionShift#/LAR/LArCellPositionShift"] 
myOCA.WriteIOV   = True
myOCA.OutputLevel= DEBUG
myOCA.IOVTagList = [ FolderTag ]
 
####to set the IOV to cover all run/event
include ("RegistrationServices/IOVRegistrationSvc_jobOptions.py")
###IOVRunNumberMin/Max etc defined in the above
myOCA.Run1=IOVRunNumberMin
#myOCA.Run2=IOVRunNumberMax
 
svcMgr.IOVDbSvc.dbConnection="sqlite://;schema="+OutputSQLiteFile+";dbname=OFLP200"


#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
svcMgr.IOVDbSvc.OutputLevel        = INFO
