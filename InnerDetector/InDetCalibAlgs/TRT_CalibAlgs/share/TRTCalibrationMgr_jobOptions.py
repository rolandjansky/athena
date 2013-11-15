# Supplying the necessary joboptions for running TRTAlignmentMgr
#include( "TrkTrackSummaryTool/ConfiguredTrkTrackSummaryTool.py")
#TrackSummaryTool = ConfiguredTrkTrackSummaryTool()

theApp.Dlls += [ "TRT_CalibAlgs" ]
theApp.TopAlg += ["TRTCalibrationMgr"]
# Sepcify input and [some optional] tools for the Manager
TRTCalibrationMgr = Algorithm( "TRTCalibrationMgr" )
TRTCalibrationMgr.TrkCollections = [ "Tracks" ]
TRTCalibrationMgr.TrackSelectionTool = "TRTCalibTrackSelectionTool/TRTTrackSelector"
TRTCalibrationMgr.AlignTrkTools = [ "FillAlignTrkInfo/TRTTrkInfo", "FillAlignTRTHits/TRTHits" ]
TRTCalibrationMgr.AccumulatorTools = [ "Accumulator/TRTAccumulator" ]
TRTCalibrationMgr.FitTools = [ "" ]

# Load all the tools
theApp.Dlls += [ "TRT_CalibTools" ]


