#
# jobOptions to read TRT conditions from a text file
# include it in a RecExample

# NOTE: In this example only calibration constants are given
#       bit it is now also possible to give a alignTextFile
#       -see the example in share. It is possible to give all
#        three Euler angles (remember that in this case the
#        angles change sign at negative z) instead of just one.


theApp.Dlls += [ "TRT_ConditionsAlgs" ]

ToolSvc = Service("ToolSvc")
ToolSvc.TRT_CondDbTool.readFromTextFiles = True
# This needs to be false for testbeam before 12.0.0
ToolSvc.TRT_CondDbTool.useCOOL = True
ToolSvc.TRT_CondDbTool.OutputLevel = 2
ToolSvc.TRT_DriftFunctionTool.OutputLevel = 1
ToolSvc.TRT_CondDbTool.calibTextFile = "CosmicCalibrationConstants.txt"




