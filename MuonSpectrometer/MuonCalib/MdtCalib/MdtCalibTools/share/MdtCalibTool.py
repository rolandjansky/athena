theApp.Dlls += ["MdtCalibTools"]

ToolSvc = Service( "ToolSvc" )
MdtCalibTool = Service( "ToolSvc.MuonCalib_MdtCalibTool" )

MdtCalibTool.MdtSegmentSelectorToolType = "MuonCalib::SimpleMdtSegmentSelectorTool"
MdtCalibTool.MdtSegmentSelectorToolName = "MdtCalib_SimpleMdtSegmentSelectorTool"
MdtCalib_SimpleMdtSegmentSelectorTool = Service("ToolSvc.MdtCalib_SimpleMdtSegmentSelectorTool" )
#modify here MdtCalib_SimpleMdtSegmentSelectorTool properties
#MdtCalib_SimpleMdtSegmentSelectorTool.MinHitsOntrack=3.
#MdtCalib_SimpleMdtSegmentSelectorTool.MaxChi2=10.

if doMuonCalibAtlas:
    include( "MuonCalibTools/AtlasRegionSelectorTool.py" )
    MdtCalibTool.RegionSelectorType = "MuonCalib::AtlasRegionSelectorTool"
    MdtCalibTool.RegionSelectorName = "MdtCalib_AtlasRegionSelectorTool"
    MdtCalib_AtlasRegionSelectorTool = Service("ToolSvc.MdtCalib_AtlasRegionSelectorTool" )
    #modify here MdtCalib_AtlasRegionSelectorTool properties
    #MdtCalib_AtlasRegionSelectorTool.Split = 1
else :
    include( "MuonCalibTools/TestbeamRegionSelectorTool.py" )
    MdtCalibTool.RegionSelectorType = "MuonCalib::TestbeamRegionSelectorTool"
    MdtCalibTool.RegionSelectorName = "MdtCalib_TestbeamRegionSelectorTool"
    MdtCalib_TestbeamRegionSelectorTool = Service("ToolSvc.MdtCalib_TestbeamRegionSelectorTool" )
