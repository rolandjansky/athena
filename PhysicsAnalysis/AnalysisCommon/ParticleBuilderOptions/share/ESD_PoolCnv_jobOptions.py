#---------------MC Truth
include( "PartPropSvc/PartPropSvc.py" )
include.block( "PartPropSvc/PartPropSvc.py" )
include( "G4SimAthenaPOOL/G4SimAthenaPOOL_joboptions.py" )
include( "GeneratorObjectsAthenaPool/GeneratorObjectsAthenaPool_joboptions.py" )

#---------------Inner Detector - Tracking 
#include( "InDetEventCnvTools/InDetEventCnvTools_jobOptions.py" )
include( "InDetEventAthenaPool/InDetEventAthenaPool_joboptions.py" )
#include( "TrkEventAthenaPool/TrkEventAthenaPool_joboptions.py" )

#---------------Calorimeter

from AthenaCommon.AppMgr import ToolSvc
include( "LArAthenaPool/LArAthenaPool_joboptions.py" )
#ToolSvc.LArTowerEMHEC.IncludedCalos = ["LAREM","LARHEC"]
from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
ToolSvc += LArTowerBuilderTool("LArTowerEMHEC",
                                IncludedCalos = ["LAREM","LARHEC"])

include( "TileEventAthenaPool/TileEventAthenaPool_joboptions.py" )

#---------------Muon
include( "MuonEventCnvTools/MuonEventCnvTools_jobOptions.py" )

#---------------Trigger
include( "TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py" )
include( "TrigEventAthenaPool/TrigEventAthenaPool_joboptions.py" )

