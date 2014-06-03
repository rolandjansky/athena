###############################################################
#
# Job options fragment for LArRegionSelectorCheckOL (standalone)
#
#==============================================================

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.DataSource.set_geant4()

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOff()
DetFlags.Calo_setOn()
DetFlags.Muon_setOff()

#CTBforTTCellMap = True

# setup GeoModel
# following line is only for backward compatibility
# should be cleaned away once migration to configurable is achieved
#DetDescrVersion="ATLAS-CTB-01"
DetDescrVersion="ATLAS-CSC-02-00-00"


from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-CSC-02-00-00"
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

include( "CaloConditions/CaloConditions_jobOptions.py" )
# following line is only for backward compatibility
# should be cleaned away once migration to configurable is achieved
from AthenaCommon.ConfigurableDb import getConfigurable
include( "LArDetDescr/LArDetDescr_joboptions.py" )
include( "CaloDetMgrDetDescrCnv/CaloTTDetMgrDetDescrCnv_joboptions.py" )

# Number of events to be processed (default is 10)
theApp.EvtMax = 1

svcMgr.MessageSvc.OutputLevel      = INFO
svcMgr.MessageSvc.debugLimit       = 100000

from AthenaCommon.AppMgr import ToolSvc
ToolSvc.LArRoI_Map.Print=FALSE
ToolSvc.LArRegionSelectorTable.PrintTable=FALSE
ToolSvc.LArRegionSelectorTable.OutputLevel=2


# RegionSelectorTable 
theApp.TopAlg += [ "LArRegionSelectorCheckOL" ]
#
# LArRegionSelectorCheckOL properties
#
LArRegionSelectorCheckOL = Algorithm( "LArRegionSelectorCheckOL" )
LArRegionSelectorCheckOL.TestTable=TRUE
LArRegionSelectorCheckOL.CheckOverlaps=FALSE
LArRegionSelectorCheckOL.OutputLevel=3
#==============================================================
#
# End of job options file
#
###############################################################
