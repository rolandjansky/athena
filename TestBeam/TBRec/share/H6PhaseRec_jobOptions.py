# assume two TDCs for H6
TBPhaseRec.TDCNames           = [                     # the TDC(s) with phase information
    "word1frag0x03chan0",
    "word2frag0x03chan0"
    ]
TBPhaseRec.TDCToTime          = [-0.05*ns, -0.05*ns]  # per TDC
TBPhaseRec.TDCwac             = [    658.,     390.]  # wrap around constants
TBPhaseRec.TDCMin             = [    320.,     320.]  # used only for choosing the best TDC if necessary
TBPhaseRec.TTCClockPeriod     = 25.*ns
TBPhaseRec.TimeBins           = 25                    # 0 <= phase index <= (TimeBins - 1)
TBPhaseRec.TBPhaseKey         = "TBPhase"             # key of the TBPhase object that TBPhaseRec tries to create


