print "now initializing the LAr readout geometry : H8 flavour"

# ------------------------------------------------
#   This is the real DetDescr part :
#   The identifier dictionary is choosen by using the
#   tag LAr-H8-00
# ----------------------------------------------

# Very very temporary hack : setup GeoModel for standalone clients !
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
# end of the hack ----

# as long as not done centrally by AtlasGeoModel ( CTB case )
include( "AtlasGeoModel/LArGeoModel.py" )

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.LAr_VersionOverride = "LAr-H8-00"
        
# ------------------------------------------------
#  FIXME : setup online identifier & mapping
#    ( should be done somewhere else )
# ------------------------------------------------
include( "CaloConditions/CaloConditions_jobOptions.py" ) 
include( "LArConditionsCommon/LArIdMap_H8_jobOptions.py")

from AthenaCommon.AppMgr import ToolSvc
from LArCabling.LArCablingConf import LArCablingService
LArCablingService = LArCablingService("LArCablingService")
LArCablingService.LArFebRodMapKey = "LArFebRodMapH8"
LArCablingService.LArOnOffIdMapKey = "LArOnOffIdMapH8"
ToolSvc += LArCablingService


# ------------------------------------------------
# FIXME : This has to be absorbed in alignment
# ------------------------------------------------
#    if GlobalFlags.DataSource.is_data():
#        ToolSvc.LArDetDescrTool.emb_x_shift= -10.
#    else:
#        ToolSvc.LArDetDescrTool.emb_x_shift= 0.

# must be included once only :
include.block ("LArDetDescr/LArDetDescr_H8_joboptions.py")

