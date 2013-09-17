include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H6_joboptions.py" )

#
#---- ByteStream-Files input --------------------
#


include( "TBCnv/TBReadH6BS_jobOptions.py" )
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
#ByteStreamInputSvc.inputFiles    = ["MyRawEvent.re"]
# With H6 - See also TBReadH6BS_jobOptions.py !!
#ByteStreamInputSvc.inputFiles    = ["bytestream.out"]
ByteStreamInputSvc.inputFiles    = ["/scratchdisk/rmcphers/data/run370.dat"]

ToolSvc.TBByteStreamCnvTool.Dump = TRUE

# --- TBRec --------------
theApp.Dlls += [ "TBRec" ]

theApp.TopAlg += ["TBBPCRec/TBBPCRec"]
TBBPCRec =  Algorithm("TBBPCRec")
include( "TBRec/H6BPCRec_jobOptions.py" )

theApp.TopAlg += ["TBScintillatorRec"]
TBScintRec =  Algorithm("TBScintillatorRec")
include( "TBRec/H6ScintRec_jobOptions.py" )


#--- Monitoring ---------------------------
theApp.TopAlg += [ "TBAlgoSequencer/Seq1" ]
theApp.Dlls += [ "GaudiAlg"]

theApp.Dlls += [ "AthenaMonitoring"]
Seq1 = Algorithm( "Seq1" )

Seq1.SubAlgorithms = ["AthenaMon/athenamon"]

theApp.Dlls += [ "TBMonitoring" ]

athenamon = Algorithm( "athenamon" )

tdcbinnum = 100
tdcmin = 0
tdcmax = 1023

#------------- BPC Monitor -------
athenamon.AthenaMonTools += ["TBBPCMonTool/BPCmon"]
ToolSvc.BPCmon.MonitorBPC      = TRUE
ToolSvc.BPCmon.MonitorBPCRaw   = FALSE
ToolSvc.BPCmon.FakeDetectors   = FALSE
#H6 BPCs
ToolSvc.BPCmon.BPCNames = ["BPC0","BPC1","BPC2","BPC3","BPC4","BPC5"]
ToolSvc.BPCmon.BPCMaxPos = 10
ToolSvc.BPCmon.BPCMinPos = -10
ToolSvc.BPCmon.BPCBinNum = 100

ToolSvc.BPCmon.TDCBinNum = tdcbinnum
ToolSvc.BPCmon.TDCMax = tdcmax
ToolSvc.BPCmon.TDCMin = tdcmin

# # ------------- Scint Monitor -------
athenamon.AthenaMonTools += ["TBScintillatorMonTool/Scintmon"]
ToolSvc.Scintmon.FakeDetectors   = FALSE
#H6 scint names :
ToolSvc.Scintmon.ScintNames = ["S1","S2","S3","B","Veto","Halo","muon1","muon2","muon3","muon4","muon5","muon6","muon7","muon8",]

ToolSvc.Scintmon.TDCBinNum = tdcbinnum
ToolSvc.Scintmon.TDCMax = tdcmax
ToolSvc.Scintmon.TDCMin = tdcmin

# ------------- ADC Monitor -------
athenamon.AthenaMonTools += ["TBADCRawMonTool/ADCmon"]
ToolSvc.ADCmon.TestADCMonTool   = FALSE
include( "TBCnv/H6ADCChannelNames.py" ) # list of ADC
ToolSvc.ADCmon.ADCBinNum = tdcbinnum
ToolSvc.ADCmon.ADCMax = tdcmax
ToolSvc.ADCmon.ADCMin = tdcmin

# ------------- TDC Monitor -------
athenamon.AthenaMonTools += ["TBTDCRawMonTool/TDCmon"]
ToolSvc.TDCmon.TestTDCMonTool   = FALSE
include( "TBCnv/H6TDCChannelNames.py" ) # list of TDC
ToolSvc.TDCmon.TDCBinNum = tdcbinnum
ToolSvc.TDCmon.TDCMax = tdcmax
ToolSvc.TDCmon.TDCMin = tdcmin



#ToolSvc.beamdetmon.TriggFlags = {0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000011};
#ToolSvc.beamdetmon.TriggFlags = [1,2,4,8,16,32]


# -- use root histos --
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "test.root"




# -- Use auditors --
#theApp.Dlls += [ "GaudiAud" ]
# write out a summary of the time spent
#
#theAuditorSvc = AuditorSvc()
#theAuditorSvc.Auditors  += [ "ChronoAuditor"]
# write out a short message upon entering or leaving each algorithm
#
# AuditorSvc.Auditors  += { "NameAuditor" };
#-------------------------------------------------------------------------
# user driving parameters
#
# Number of events to be processed (default is 10)
theApp.EvtMax = 10

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =3
ToolSvc.TBByteStreamCnvTool.OutputLevel =3
ToolSvc.BPCmon.OutputLevel =3
ToolSvc.Scintmon.OutputLevel =3

# Dump all objects available
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE
