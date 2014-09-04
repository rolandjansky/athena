# jobOptions file for LAr Cell Reconstruction from LArRawChannel 
# with full corrections. 
# Properties of LArCellRec:
theApp.Dlls += [ "LArCellRec" ]

# For Nova database (needed by CaloNoiseTool)
#include( "LArCondCnv/LArCondCnv_G3_jobOptions.py" )

theApp.Dlls += [ "LArTools","LArCalibUtils" ]

#add cell maker tools to top cell maker alg (defined elsewhere)
#one tool for all LAr
#
# split the cells
#
# CaloCellMaker.CaloCellMakerToolNames += [
#                      "LArCellBuilderFromLArRawChannelTool/cellbuilderLAr" ]
# # CellBuilder properties
# ToolSvc.cellbuilderLAr.LArRegion   = ""
# ToolSvc.cellbuilderLAr.EThreshold  = -100000. * MeV
# ToolSvc.cellbuilderLAr.OutputLevel =  ERROR

# it is also possible to specify one tool per sub calo:
CaloCellMaker.CaloCellMakerToolNames += [
    "LArCellBuilderFromLArRawChannelTool/cellbuilderEM",
    "LArCellBuilderFromLArRawChannelTool/cellbuilderHEC",
    "LArCellBuilderFromLArRawChannelTool/cellbuilderFCal"]
# CellBuilder properties
ToolSvc.cellbuilderEM.LArRegion      = "LAr_EM"
ToolSvc.cellbuilderEM.EThreshold     =-100000. * MeV
ToolSvc.cellbuilderEM.OutputLevel    =  WARNING
ToolSvc.cellbuilderHEC.LArRegion     = "LAr_HEC"
ToolSvc.cellbuilderHEC.EThreshold    =-100000.  * MeV  
ToolSvc.cellbuilderHEC.OutputLevel   =  WARNING
ToolSvc.cellbuilderFCal.LArRegion    = "LAr_FCal"
ToolSvc.cellbuilderFCal.EThreshold   =-100000.  * MeV
ToolSvc.cellbuilderFCal.OutputLevel  =  WARNING
