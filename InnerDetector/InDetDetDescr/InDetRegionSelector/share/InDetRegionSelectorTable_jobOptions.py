###############################################################
#
# Job options fragment for InDetRegionSelectorLUT
#
#==============================================================

include.block("InDetRegionSelector/InDetRegionSelectorTable_jobOptions.py")

# # For identfier to robid mappings
# if 'InDetCabling' not in theApp.Dlls:
#    theApp.Dlls += [ "InDetCabling" ]

# get DetFlags
from AthenaCommon.DetFlags import DetFlags
# get topSequence
topSequence = AlgSequence()
# get ToolSvc
ToolSvc = Service( "ToolSvc" )

#
# SiRegionSelectorTable and TRT_RegionSelectorTable properties
#
# ManagerName:      Manager name.
#
# For debugging the following is also available
# OuputFile:        File name of output file. 
# PrintHashId:	     true: print hash ids, false: print expanded id.
# PrintTable:       Output table to a text file.

if DetFlags.detdescr.pixel_on():
    from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
    PixelRegionSelectorTable = SiRegionSelectorTable(name        = "PixelRegionSelectorTable",
                                                     ManagerName = "Pixel",
                                                     OutputFile  = "RoITablePixel.txt",
                                                     PrintHashId = True,
                                                     PrintTable  = False)
    ToolSvc += PixelRegionSelectorTable
    print      PixelRegionSelectorTable
else :
    PixelRegionSelectorTable = None

if DetFlags.detdescr.SCT_on():
    from InDetRegionSelector.InDetRegionSelectorConf import SiRegionSelectorTable
    SCT_RegionSelectorTable = SiRegionSelectorTable(name        = "SCT_RegionSelectorTable",
                                                    ManagerName = "SCT",
                                                    OutputFile  = "RoITableSCT.txt",
                                                    PrintHashId = True,
                                                    PrintTable  = False)
    ToolSvc += SCT_RegionSelectorTable
    print      SCT_RegionSelectorTable
else :
    SCT_RegionSelectorTable = None

if DetFlags.detdescr.TRT_on():
    from InDetRegionSelector.InDetRegionSelectorConf import TRT_RegionSelectorTable
    TRT_RegionSelectorTable = TRT_RegionSelectorTable(name        = "TRT_RegionSelectorTable",
                                                      ManagerName = "TRT",
                                                      OutputFile  = "RoITableTRT.txt",
                                                      PrintHashId = True,
                                                      PrintTable  = False)
    ToolSvc += TRT_RegionSelectorTable
    print      TRT_RegionSelectorTable
else :
    TRT_RegionSelectorTable = None

if DetFlags.detdescr.FTK_on():
    from InDetRegionSelector.InDetRegionSelectorConf import FTK_RegionSelectorTable
    FTK_RegionSelectorTable = FTK_RegionSelectorTable(name        = "FTK_RegionSelectorTable",
                                                      ManagerName = "FTK",
                                                      OutputFile  = "RoITableFTK.txt",
                                                      PrintHashId = True,
                                                      PrintTable  = False)
    ToolSvc += FTK_RegionSelectorTable
    print      FTK_RegionSelectorTable
else :
    print "FTK_RegionSelectorTable not being set up"
    FTK_RegionSelectorTable = None


#
# --- now load algorithm
#
# from InDetRegionSelector.InDetRegionSelectorConf import InDetDD__InDetRegionSelectorLUT
# InDetRegionSelectorLUT = InDetDD__InDetRegionSelectorLUT(name                       = "InDetRegionSelectorLut",
#                                                          PixelRegionLUT_CreatorTool = PixelRegionSelectorTable,
#                                                          SCT_RegionLUT_CreatorTool  = SCT_RegionSelectorTable,
#                                                          TRT_RegionLUT_CreatorTool  = TRT_RegionSelectorTable,
#                                                          DoTests                    = False)
# topSequence += InDetRegionSelectorLUT
# print          InDetRegionSelectorLUT

###############################################################
