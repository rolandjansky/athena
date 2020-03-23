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


# Pixel table

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


# SCT table

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


# TRT table 

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
