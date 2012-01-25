theApp.Dlls += ["MuonCalibTools"]

MooMakeRoads.WriteCalibPatterns = 2
MooMakeTracks.WriteCalibPatterns = 2

ToolSvc = Service( "ToolSvc" )
MooPatternForCalibrationTool = Service( "ToolSvc.MuonCalib::MooPatternForCalibrationTool" )
MooPatternForCalibrationTool.MooreCalibPatternsLocation = "Moore_patternsForCalibration"
