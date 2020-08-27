#------------------------------------------------------------
# jobOption Fragment for Beam Instrumentation Reconstruction 
# and Monitoring on the Event Filter
#------------------------------------------------------------


#---- Read Beam Instrumentation
include( "TBCnv/TBReadH8BS_jobOptions.py") 

#---- Reconstruct Beam Instrumentation ------------------------
theApp.Dlls+=["TBRec"]
#--- BPCs
theApp.TopAlg += ["TBBPCRec/TBBPCRec"]
TBBPCRec =  Algorithm("TBBPCRec")
TBBPCRec.SGkey="BPCRawCont"
include( "TBRec/H8BPCRec_jobOptions.py" )

#--- Scintillators
ps=picosecond
theApp.TopAlg += ["TBScintillatorRec"]
TBScintRec =  Algorithm("TBScintillatorRec")
include( "TBRec/H8ScintRec_jobOptions.py" )

# --- Monitor Beam Instruments
theApp.Dlls += ["TBMonitoring" ]
theApp.TopAlg += ["AthenaMon/BeamMon"]
BeamMon=Algorithm("BeamMon")

# ------------- Scint Monitor -------
BeamMon.AthenaMonTools += ["TBScintillatorMonTool/Scintmon"]
ToolSvc.Scintmon.FakeDetectors   = FALSE
# H8 scint names :
ToolSvc.Scintmon.ScintNames = ["S0","S1","S2-Up","S2-Down","S3-Left","S3-Right","C1","C2","muTag","muHalo","muVeto"]
ToolSvc.Scintmon.TDCBinNum = 4096
ToolSvc.Scintmon.TDCMax = 4096*35*ps
ToolSvc.Scintmon.TDCMin = 0
ToolSvc.Scintmon.ADCBinNum = 4096
ToolSvc.Scintmon.ADCMax = 4096
ToolSvc.Scintmon.ADCMin = 0
ToolSvc.Scintmon.BookAtFirstEvent=True;
#ToolSvc.Scintmon.OutputLevel=DEBUG
ToolSvc.Scintmon.histoPath="/Monitoring/Scintillators/"
# ------------- BPC Monitor -------
BeamMon.AthenaMonTools += ["TBBPCMonTool/BPCmon"]
ToolSvc.BPCmon.MonitorBPC      = TRUE
ToolSvc.BPCmon.MonitorBPCRaw   = FALSE
ToolSvc.BPCmon.FakeDetectors   = FALSE
# H8 BPCs
#ToolSvc.BPCmon.BPCNames = ["BPC-2","BPC-1","BPC0","BPC1","BPC2"]
#BPC-2 not used right now
ToolSvc.BPCmon.BPCNames = ["BPC-1","BPC0","BPC1","BPC2"] 
ToolSvc.BPCmon.BPCMaxPos = 50
ToolSvc.BPCmon.BPCMinPos = -50
ToolSvc.BPCmon.BPCBinNum = 100
ToolSvc.BPCmon.BookAtFirstEvent=True;
ToolSvc.BPCmon.histoPath="/Monitoring/BPC/"
#ToolSvc.BPCmon.OutputLevel=DEBUG
