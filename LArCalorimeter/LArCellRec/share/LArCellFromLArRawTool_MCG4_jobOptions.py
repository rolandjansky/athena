# jobOptions file for LAr Cell Reconstruction from LArRawChannel 
# with full corrections. 
# Properties of LArCellRec:
theApp.Dlls += [ "LArCellRec" ]

theApp.Dlls += [ "LArTools","LArCalibUtils" ]

#add cell maker tools to top cell maker alg (defined elsewhere)
# if all LAR required only one builder
if DetFlags.makeRIO.LAr_allOn():
    CaloCellMaker.CaloCellMakerToolNames += [
                      "LArCellBuilderFromLArRawChannelTool/cellbuilderLAr" ]
    # CellBuilder properties
    ToolSvc.cellbuilderLAr.LArRegion = ""
    ToolSvc.cellbuilderLAr.EThreshold =-100000. * MeV  
else:
    # one builder per lar calo
    if DetFlags.makeRIO.em_on():
        CaloCellMaker.CaloCellMakerToolNames += [
                      "LArCellBuilderFromLArRawChannelTool/cellbuilderem" ]
        # CellBuilder properties
        ToolSvc.cellbuilderLArem.LArRegion = "LAr_EM"
        ToolSvc.cellbuilderLArem.EThreshold =-100000. * MeV  
    if DetFlags.makeRIO.HEC_on():
        CaloCellMaker.CaloCellMakerToolNames += [
                      "LArCellBuilderFromLArRawChannelTool/cellbuilderem" ]
        # CellBuilder properties
        ToolSvc.cellbuilderLArHEC.LArRegion = "LAr_HEC"
        ToolSvc.cellbuilderLArHEC.EThreshold =-100000. * MeV  
    if DetFlags.makeRIO.FCal_on():
        CaloCellMaker.CaloCellMakerToolNames += [
                      "LArCellBuilderFromLArRawChannelTool/cellbuilderem" ]
        # CellBuilder properties
        ToolSvc.cellbuilderLArFCal.LArRegion = "LAr_FCal"
        ToolSvc.cellbuilderLArFCal.EThreshold =-100000. * MeV  
# it is also possible to specify one tool per sub calo: but this is less
# efficient 
#CaloCellMaker.CaloCellMakerToolNames += [
#                      "LArCellBuilderFromLArRawChannelTool/cellbuilderEM",
#	               "LArCellBuilderFromLArRawChannelTool/cellbuilderHEC",
#		       "LArCellBuilderFromLArRawChannelTool/cellbuilderFCal]
# CellBuilder properties
#ToolSvc.cellbuilderEM.LArRegion = "LAr_EM"
#ToolSvc.cellbuilderEM.EThreshold =-100000. * MeV  
#ToolSvc.cellbuilderHEC.LArRegion = "LAr_HEC"
#ToolSvc.cellbuilderHEC.EThreshold =-100000.  * MeV  
#ToolSvc.cellbuilderFCal.LArRegion = "LAr_FCal"
#ToolSvc.cellbuilderFCal.EThreshold =-100000.  * MeV

