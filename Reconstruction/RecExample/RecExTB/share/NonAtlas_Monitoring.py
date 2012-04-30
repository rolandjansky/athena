theApp.Dlls += [ "AthenaMonitoring"]
theApp.TopAlg += [ "AthenaMon/athenamon" ]
theApp.Dlls += [ "TBMonitoring" ]
athenamon = Algorithm( "athenamon" )
#athenamon.AthenaMonTools = ["BeamDetectorMonTool/beamdetmon"]

tdcbinnum = 100
tdcmin = 0
tdcmax = 1023

# ------------- BPC Monitor -------
athenamon.AthenaMonTools += ["TBBPCMonTool/BPCmon"]
ToolSvc.BPCmon.MonitorBPC      = FALSE
ToolSvc.BPCmon.MonitorBPCRaw   = TRUE
ToolSvc.BPCmon.FakeDetectors   = FALSE
# H8 BPCs
#ToolSvc.beamdetmon.BPCNames = ["BPC-2","BPC-1","BPC0","BPC1","BPC2"]
ToolSvc.BPCmon.BPCNames = ["BPC-2","BPC-1","BPC0"]
ToolSvc.BPCmon.BPCMaxPos = 10
ToolSvc.BPCmon.BPCMinPos = -10
ToolSvc.BPCmon.BPCBinNum = 100

ToolSvc.BPCmon.TDCBinNum = tdcbinnum
ToolSvc.BPCmon.TDCMax = tdcmax
ToolSvc.BPCmon.TDCMin = tdcmin

# ------------- Scint Monitor -------
athenamon.AthenaMonTools += ["TBScintillatorMonTool/Scintmon"]
ToolSvc.Scintmon.FakeDetectors   = FALSE
# H8 scint names :
ToolSvc.Scintmon.ScintNames = ["S0","S1","S2-Up","S2-Down","S3-Left","S3-Right","C1","C2","muTag","muHalo","muVeto"]

ToolSvc.Scintmon.TDCBinNum = tdcbinnum
ToolSvc.Scintmon.TDCMax = tdcmax
ToolSvc.Scintmon.TDCMin = tdcmin

# ------------- ADC Monitor -------
athenamon.AthenaMonTools += ["TBADCRawMonTool/ADCmon"]
include( "TBCnv/H8ADCChannelNames.py" )
ToolSvc.ADCmon.FakeDetectors   = FALSE
ToolSvc.ADCmon.TDCBinNum = tdcbinnum
ToolSvc.ADCmon.TDCMax = tdcmax
ToolSvc.ADCmon.TDCMin = tdcmin

# ------------- TDC Monitor -------
athenamon.AthenaMonTools += ["TBTDCRawMonTool/TDCmon"]
include( "TBCnv/H8TDCChannelNames.py" )
ToolSvc.TDCmon.FakeDetectors   = TRUE
ToolSvc.TDCmon.TDCBinNum = tdcbinnum
ToolSvc.TDCmon.TDCMax = tdcmax
ToolSvc.TDCmon.TDCMin = tdcmin

#ToolSvc.beamdetmon.TriggFlags = {0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000011};
#ToolSvc.beamdetmon.TriggFlags = [1,2,4,8,16,32]


