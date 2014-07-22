###############################################################
#
# Job options fragment for InDetRegionSelectorLUT
#
#==============================================================

include.block("MuonRegionSelector/MuonRegionSelectorTable_jobOptions.py")


# get DetFlags
from AthenaCommon.DetFlags import DetFlags
# get topSequence
topSequence = AlgSequence()
# get ToolSvc
ToolSvc = Service( "ToolSvc" )

#### RPC ####

if DetFlags.detdescr.RPC_on():
    from MuonRegionSelector.MuonRegionSelectorConf import RPC_RegionSelectorTable
    RPC_RegionSelectorTable = RPC_RegionSelectorTable(name = "RPC_RegionSelectorTable")

    ToolSvc += RPC_RegionSelectorTable
    print      RPC_RegionSelectorTable
else :
    RPC_RegionSelectorTable = None


#### MDT ####

if DetFlags.detdescr.MDT_on():
    from MuonRegionSelector.MuonRegionSelectorConf import MDT_RegionSelectorTable
    MDT_RegionSelectorTable = MDT_RegionSelectorTable(name = "MDT_RegionSelectorTable")

    ToolSvc += MDT_RegionSelectorTable
    print      MDT_RegionSelectorTable
else :
    MDT_RegionSelectorTable = None


#### CSC ####

if DetFlags.detdescr.CSC_on():
    from MuonRegionSelector.MuonRegionSelectorConf import CSC_RegionSelectorTable
    CSC_RegionSelectorTable = CSC_RegionSelectorTable(name = "CSC_RegionSelectorTable")

    ToolSvc += CSC_RegionSelectorTable
    print      CSC_RegionSelectorTable
else :
    CSC_RegionSelectorTable = None


#### TGC ####

if DetFlags.detdescr.TGC_on():
    from MuonRegionSelector.MuonRegionSelectorConf import TGC_RegionSelectorTable
    TGC_RegionSelectorTable = TGC_RegionSelectorTable(name = "TGC_RegionSelectorTable")

    ToolSvc += TGC_RegionSelectorTable
    print      TGC_RegionSelectorTable
else :
    TGC_RegionSelectorTable = None

