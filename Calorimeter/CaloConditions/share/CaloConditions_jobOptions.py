include.block ( "CaloConditions/CaloConditions_jobOptions.py" )

############################################################################
#
#  Job options file for initializing CaloTriggerTowerService's
#  (from CaloTriggerTool package) maps:
#  TT-cell map, online-offline TT ids + some auxiliary maps  .
#
#  To be included in Comissioning and MD jobs; CTB jobs not supported in all versions.
#
#  Created 2007-APR-06 by F. Ledroit
#==========================================================================

from AthenaCommon.JobProperties import jobproperties
DGeo    =  jobproperties.Global.DetGeo()

# LArTTCellMap (maintained by F. Ledroit)

if (DGeo == "ctbh8"):
    print " No trigger mapping needed for CTB simulation"
else:
    include( "CaloConditions/LArTTCellMap_ATLAS_jobOptions.py")
    # TT online-offline & online-attributes maps (maintained by D. Prieur)
    include( "CaloConditions/CaloTTIdMap_ATLAS_jobOptions.py")
