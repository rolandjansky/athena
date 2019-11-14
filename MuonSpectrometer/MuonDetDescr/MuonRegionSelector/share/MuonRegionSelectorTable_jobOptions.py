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

from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags

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

# could avoid first check in case DetFlags.detdescr.CSC_on() would take into account MuonGeometryFlags already
if MuonGeometryFlags.hasCSC() and DetFlags.detdescr.CSC_on():
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

#### New small wheel ####

# could avoid first check in case DetFlags.detdescr.Micromegas_on() would take into account MuonGeometryFlags already
if MuonGeometryFlags.hasMM() and DetFlags.detdescr.Micromegas_on():
    from MuonRegionSelector.MuonRegionSelectorConf import MM_RegionSelectorTable
    MM_RegionSelectorTable = MM_RegionSelectorTable(name = "MM_RegionSelectorTable")

    ToolSvc += MM_RegionSelectorTable
    print      MM_RegionSelectorTable
else :
    MM_RegionSelectorTable = None

# could avoid first check in case DetFlags.detdescr.sTGC_on() would take into account MuonGeometryFlags already
if MuonGeometryFlags.hasSTGC() and DetFlags.detdescr.sTGC_on():
    from MuonRegionSelector.MuonRegionSelectorConf import sTGC_RegionSelectorTable
    sTGC_RegionSelectorTable = sTGC_RegionSelectorTable(name = "sTGC_RegionSelectorTable")

    ToolSvc += sTGC_RegionSelectorTable
    print      sTGC_RegionSelectorTable
else :
    sTGC_RegionSelectorTable = None

