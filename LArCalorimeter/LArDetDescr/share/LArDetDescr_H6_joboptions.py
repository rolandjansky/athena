print "now initializing the LAr readout geometry : H6 flavour"
    
#include( "AtlasGeoModel/SetGeometryVersion.py" )
#include( "AtlasGeoModel/GeoModelInit.py" )

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

# as long as not done centrally by AtlasGeoModel ( CTB case )
#include( "AtlasGeoModel/LArGeoModel.py" )

GeoModelSvc = Service( "GeoModelSvc" )
GeoModelSvc.LAr_VersionOverride = "LAr-H6-00"

# ------------------------------------------------
#  FIXME : setup online identifier & mapping
#    ( should be done somewhere else )
# ------------------------------------------------
include( "CaloConditions/CaloConditions_jobOptions.py" ) 
include( "LArConditionsCommon/LArIdMap_H6_jobOptions.py")

from AthenaCommon.AppMgr import ToolSvc
from LArCabling.LArCablingConf import LArCablingService
LArCablingService = LArCablingService("LArCablingService")
LArCablingService.LArFebRodMapKey = "LArFebRodMapH6"
LArCablingService.LArOnOffIdMapKey = "LArOnOffIdMapH6"
ToolSvc += LArCablingService
# must be included once only :
include.block ("LArDetDescr/LArDetDescr_H6_joboptions.py")

