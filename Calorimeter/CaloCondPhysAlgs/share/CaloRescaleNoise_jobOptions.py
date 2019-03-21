###############################################################
#
# Job options file for CaloRescaleNoise
#
#==============================================================

# configuration for data, read noise from database through CaloNoiseToolDB

GlobalTag =  'COMCOND-ES1C-000-00'
RunNumber = 154757
LumiBlock = 1
Geometry = 'ATLAS-GEO-01-00-00'

from PyCool import cool
from CoolConvUtilities.AtlCoolLib import indirectOpen

trigDB=indirectOpen('COOLONL_TRIGGER/COMP200',oracle=True)
trigfolder=trigDB.getFolder('/TRIGGER/LUMI/LBLB')
runiov=(RunNumber << 32)+ LumiBlock
obj=trigfolder.findObject(runiov,0)
payload=obj.payload()
TimeStamp=payload['StartTime']/1000000000L
trigDB.closeDatabase()


print " TimeStamp : ",TimeStamp

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

svcMgr.IOVDbSvc.GlobalTag = GlobalTag

from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
theCaloNoiseTool = CaloNoiseToolDefault()
ToolSvc += theCaloNoiseTool

from LArConditionsCommon import LArHVDB

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from CaloCondPhysAlgs.CaloCondPhysAlgsConf import CaloRescaleNoise
theCaloRescaleNoise = CaloRescaleNoise("CaloRescaleNoise")
theCaloRescaleNoise.noiseTool = theCaloNoiseTool

topSequence += theCaloRescaleNoise

#--------------------------------------------------------------
#--- Dummy event loop parameters
#--------------------------------------------------------------
svcMgr.EventSelector.RunNumber         = RunNumber
svcMgr.EventSelector.EventsPerRun      = 1
svcMgr.EventSelector.FirstEvent        = 0
svcMgr.EventSelector.EventsPerLB       = 1
svcMgr.EventSelector.FirstLB           = LumiBlock
svcMgr.EventSelector.InitialTimeStamp  = TimeStamp
svcMgr.EventSelector.TimeStampInterval = 5
svcMgr.EventSelector.OverrideRunNumber=True
theApp.EvtMax                          = 1


#  ------------------------------------------------------------------
# ---  Ntuple 
# ------------------------------------------------------------------
if not hasattr(ServiceMgr, 'THistSvc'):
   from GaudiSvc.GaudiSvcConf import THistSvc
   ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["file1 DATAFILE='cellnoise_data.root' OPT='RECREATE'"];


#--------------------------------------------------------------
# Set output level threshold (1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000
svcMgr.MessageSvc.infoLimit        = 100000
svcMgr.MessageSvc.Format           = "% F%30W%S%7W%R%T %0W%M"
svcMgr.IOVDbSvc.OutputLevel        = INFO
