#=============================================================
# JopOptions file to test TileMuId2DBAlg
# File: TileCalibAlgs/jobOptions_TileMuId2DBTest.py
#=============================================================

import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import ToolSvc

#=============================================================
# Global Flags
#=============================================================
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_bytestream()    

#=============================================================
# Geometry setup
#=============================================================
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.ID_setOff()
DetFlags.detdescr.Muon_setOff()
DetFlags.detdescr.LAr_setOn()
DetFlags.detdescr.Tile_setOn()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-Comm-00-00-00"

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon.AlgSequence import AlgSequence

topSequence = AlgSequence()

#=============================================================
# set global tag
#=============================================================
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('COMCOND-ES1C-000-00')

#=============================================================
# Add includes
#=============================================================
include( "CaloDetMgrDetDescrCnv/CaloDetMgrDetDescrCnv_joboptions.py")
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )
include( "TileIdCnv/TileIdCnv_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "TileConditions/TileConditions_jobOptions.py" )
include( "LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

#=============================================================
# Add CaloNoise
#=============================================================
from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg ('totalNoise')

#============================================================
# Add TileMuId2DBAlg
#============================================================
from TileCalibAlgs.TileCalibAlgsConf import TileMuId2DBAlg
theTileMuId2DBAlg = TileMuId2DBAlg("TileMuId2DBAlg")
topSequence += theTileMuId2DBAlg

#============================================================
# Add to the job
#============================================================
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

#============================================================
# MessageSvc setup
#============================================================
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = INFO

#============================================================
# Dummy event loop setup
#============================================================
svcMgr.EventSelector.RunNumber         = 99999
svcMgr.EventSelector.EventsPerRun      = 5
svcMgr.EventSelector.FirstEvent        = 1
svcMgr.EventSelector.EventsPerLB       = 5
svcMgr.EventSelector.FirstLB           = 1
svcMgr.EventSelector.InitialTimeStamp  = 0
svcMgr.EventSelector.TimeStampInterval = 5
theApp.EvtMax                          = 1

#============================================================
# Print out job summary
#============================================================
print svcMgr
print job
