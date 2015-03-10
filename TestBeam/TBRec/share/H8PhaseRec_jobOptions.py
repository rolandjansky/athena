# assume only one TDC for H8
TBPhaseRec.TDCNames           = ["frag0x12chan0"]     # the TDC(s) with phase information

# Time-dependent (=run-dependent) TDC calibration
TBPhaseRec.CalibFileName      = "H8PhaseCalib.txt"

# These TDCToTime and the TDCwac values are derived from run 1000944
#TBPhaseRec.TDCToTime          = [-0.036*ns]           # per TDC
#TBPhaseRec.TDCwac             = [565]                 # wrap around constants
#TBPhaseRec.TDCMin             = [0.]                  # used only for choosing the best TDC.  Not relevant for H8

TBPhaseRec.TTCClockPeriod     = 25.*ns
TBPhaseRec.TimeBins           = 25                    # 0 <= phase index <= (TimeBins - 1)
TBPhaseRec.TBPhaseKey         = "TBPhase"             # key of the TBPhase object that TBPhaseRec tries to create
TBPhaseRec.GuardCutValue      = 10.0                  # WAC guard region cut
TBPhaseRec.NeverReturnFailure = TRUE                  # controls when StatusCode::FAILURE can be issued in execute()

TBPhaseRec.OutputLevel        = INFO

# WAC guard region (now integrated in TBPhaseRec)
#theApp.TopAlg += ["TBEventStreamer/PhaseStreamer"]
#PhaseStreamer = Algorithm( "PhaseStreamer" )
#PhaseStreamer.ToolNames += [ "TBPhaseStreamerTool/GuardCut" ]
#PhaseStreamer.GuardCut.TBPhaseKey = "TBPhase"
#PhaseStreamer.GuardCut.Guard = 10.0 
