include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

# include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H6_joboptions.py" )
#
#---- ByteStream-Files input --------------------
#

ToolSvc = Service( "ToolSvc" )

include( "TBCnv/TBReadH6BS_jobOptions.py" )
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
# With H6 - See also TBReadH6BS_jobOptions.py !!
#ByteStreamInputSvc.inputFiles    = ["bytestream.out"]
#ByteStreamInputSvc.inputFiles    = ["/scratchdisk/rmcphers/data/run240.dat"]
#ByteStreamInputSvc.inputFiles    = ["/scratchdisk/delsart/data/run462.bstr"]
ByteStreamInputSvc.inputFiles    = ["/afs/cern.ch/atlas/offline/data/testfile/run431.bstr"]
#ByteStreamInputSvc.inputFiles    = ["/scratchdisk1/h6data/run857.bstr"]



ToolSvc.TBByteStreamCnvTool.Dump = TRUE
ToolSvc.TBByteStreamCnvTool.SummaryPath = "lastrun.txt"

# --- TBRec --------------
theApp.Dlls += [ "TBRec" ]

theApp.TopAlg += ["TBBPCRec/TBBPCRec"]
TBBPCRec =  Algorithm("TBBPCRec")
include( "TBRec/H6BPCRec_jobOptions.py" )

theApp.TopAlg += ["TBMWPCRec/TBMWPCRec"]
TBMWPCRec =  Algorithm("TBMWPCRec")
include( "TBRec/H6MWPCRec_jobOptions.py" )



# theApp.TopAlg += ["TBScintillatorRec"]
# TBScintRec =  Algorithm("TBScintillatorRec")
# include( "TBRec/H6ScintRec_jobOptions.py" )

# -- Dump TBLArDigits

# theApp.TopAlg += ["ReadTBLArDigits"]
# ReadTBLArDigits = Algorithm( "ReadTBLArDigits" )
# ReadTBLArDigits.ContainerKey="MEDIUM"
# ReadTBLArDigits.DumpFile="TBLArDigits.txt"
# ReadTBLArDigits.PrintCellLocation=TRUE
# ReadTBLArDigits.OutputLevel=3
# NtupleSvc = Service( "NtupleSvc" )
# NtupleSvc.Output=["FILE1 DATAFILE='TBLArDigits.root' TYP='ROOT' OPT='NEW'"]

# #--- Monitoring ---------------------------
theApp.TopAlg += [ "TBAlgoSequencer/Seq1" ]
theApp.Dlls += [ "GaudiAlg"]

theApp.Dlls += [ "AthenaMonitoring"]
Seq1 = Algorithm( "Seq1" )

Seq1.SubAlgorithms = [
#    "LArDigitProcessor<TBLArDigitContainer>/TBLArDigProcessor"
    "AthenaMon/TBMon1"
    ,"AthenaMon/LArMon1"
]
# Seq1.SubAlgorithms = ["TBBPCRec/TBBPCRec",
# #                       "ReadLArDigits",
#                       "AthenaMon/TBMon1",
#                       "AthenaMon/LArMon1"]

theApp.Dlls += [ "TBMonitoring" ]
#theApp.Dlls += [ "LArMonitoring" ]

TBMon1  = Algorithm( "TBMon1" )
#LArMon1 = Algorithm( "LArMon1" )

tdcbinnum = 100
tdcmin = 0
tdcmax = 1023

# ------------- BPC Monitor -------
TBMon1.AthenaMonTools += ["TBBPCMonTool/BPCmon"]
ToolSvc.BPCmon.MonitorBPC      = FALSE
ToolSvc.BPCmon.MonitorBPCRaw   = TRUE
ToolSvc.BPCmon.FakeDetectors   = FALSE
# H6 BPCs
ToolSvc.BPCmon.BPCNames = ["BPC0","BPC1","BPC2","BPC3","BPC4","BPCAAA5"]
ToolSvc.BPCmon.BPCMaxPos = 10
ToolSvc.BPCmon.BPCMinPos = -10
ToolSvc.BPCmon.BPCBinNum = 100

ToolSvc.BPCmon.TDCBinNum = tdcbinnum
ToolSvc.BPCmon.TDCMax = tdcmax
ToolSvc.BPCmon.TDCMin = tdcmin

# ------------- MWPC Monitor -------
TBMon1.AthenaMonTools += ["TBMWPCMonTool/MWPCmon"]
ToolSvc.BPCmon.MonitorBPC      = FALSE
ToolSvc.BPCmon.MonitorBPCRaw   = TRUE
ToolSvc.BPCmon.FakeDetectors   = FALSE


# ------------- Scint Monitor -------
TBMon1.AthenaMonTools += ["TBScintillatorMonTool/Scintmon"]
ToolSvc.Scintmon.FakeDetectors   = FALSE
# H6 scint names :
ToolSvc.Scintmon.ScintNames = ["S1","S2","S3","Veto","Halo","B","muon1","muon2","muon3","muon4","muon5","muon6","muon7","muon8",]
ToolSvc.Scintmon.TDCBinNum = tdcbinnum
ToolSvc.Scintmon.TDCMax = tdcmax
ToolSvc.Scintmon.TDCMin = tdcmin

#------------- ADC Monitor -------
# TBMon1.AthenaMonTools += ["TBADCRawMonTool/ADCmon"]
# ToolSvc.ADCmon.TestADCMonTool   = FALSE
# include( "TBCnv/H6ADCChannelNames.py" ) # list of ADC
# ToolSvc.ADCmon.ADCBinNum = tdcbinnum
# ToolSvc.ADCmon.ADCMax = tdcmax
# ToolSvc.ADCmon.ADCMin = tdcmin

# # #------------- TDC Monitor -------
# TBMon1.AthenaMonTools += ["TBTDCRawMonTool/TDCmon"]
# ToolSvc.TDCmon.TestTDCMonTool   = FALSE
# include( "TBCnv/H6TDCChannelNames.py" ) # list of TDC
# ToolSvc.TDCmon.TDCBinNum = tdcbinnum
# ToolSvc.TDCmon.TDCMax = tdcmax
# ToolSvc.TDCmon.TDCMin = tdcmin

# #------------- TBLArDigitMonitoring -------------
# LArMon1.AthenaMonTools += ["LArDigMonTool<LArDigitContainer>/pedmon0"]
# ToolSvc.pedmon0.histoPathBase = "/stat/pedsamp0"
# ToolSvc.pedmon0.digitKey      = "MEDIUM"
#  #ToolSvc.pedmon0.maxSamp       = 3
# ToolSvc.pedmon0.pedMode       = "SAMPLE"
# ToolSvc.pedmon0.pedSamp       = 0
# ToolSvc.pedmon0.OutputLevel   = 3
# ToolSvc.pedmon0.adcCut        = 25
#ToolSvc.pedmon0.OutputLevel = 2


TBMon1.AthenaMonTools += ["TBTailCatcherRawMonTool/TCmon"]
ToolSvc.TCmon.TailCatcherBinNum = tdcbinnum
ToolSvc.TCmon.TailCatcherMax = tdcmax
ToolSvc.TCmon.TailCatcherMin = tdcmin

# -- use root histos --
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
#HistogramPersistencySvc.OutputFile  = "/scratchdisk/delsart/test.root"
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
theApp.EvtMax = 310

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =3
ToolSvc.TBByteStreamCnvTool.OutputLevel =2
ToolSvc.BPCmon.OutputLevel =3
ToolSvc.MWPCmon.OutputLevel =3
ToolSvc.Scintmon.OutputLevel =4
ToolSvc.TCmon.OutputLevel =4

TBBPCRec.OutputLevel =3

# Dump all objects available
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE
