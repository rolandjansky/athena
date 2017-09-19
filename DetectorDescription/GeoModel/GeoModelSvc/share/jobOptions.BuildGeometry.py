# job options to build geometry of the ATLAS detector

# Create an event selector:
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.GlobalFlags import globalflags
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.DataSource.set_Value_and_Lock('geant4')

# Detector flags
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOn()
DetFlags.Tile_setOn()
DetFlags.Muon_setOn()
DetFlags.Lucid_setOn()
DetFlags.Print()

# GeoModel
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# Generate one dummy event
from AthenaCommon.AppMgr import AppMgr as appMgr
appMgr.EvtMax = 1

# Set some conditions tag to avoud crashes
from IOVDbSvc.CondDB import conddb
conddb.setGlobalTag('OFLCOND-CSC-00-00-00')

PoolSvc = Service("PoolSvc")
PoolSvc.SortReplicas = False

