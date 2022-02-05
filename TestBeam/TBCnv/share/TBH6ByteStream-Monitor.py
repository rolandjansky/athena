include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

# include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
# include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H6_joboptions.py" )
#
#---- ByteStream-Files input --------------------
#

include( "TBCnv/TBReadH6BS_jobOptions.py" )
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )
# ByteStreamInputSvc.inputFiles    = ["MyRawEvent.re"]
# With H6 - See also TBReadH6BS_jobOptions.py !!
# ByteStreamInputSvc.inputFiles    = ["bytestream.out"]
# ByteStreamInputSvc.inputFiles    = ["/scratchdisk/rmcphers/data/run240.dat"]
#ByteStreamInputSvc.inputFiles    = ["/scratchdisk/rmcphers/data/run370.dat"]
ByteStreamInputSvc.inputFiles  = [ "/raid/bytestream/phys/run590.bstr" ]
                                 

ToolSvc = Service( "ToolSvc" )

ToolSvc.TBByteStreamCnvTool.Dump        = FALSE
ToolSvc.TBByteStreamCnvTool.OutputLevel = WARNING

#--- Monitoring ---------------------------

theApp.TopAlg += [ "TBAlgoSequencer/Seq1" ]

theApp.Dlls += [ "AthenaMonitoring"]

Seq1 = Algorithm( "Seq1" )

Seq1.SubAlgorithms = [
    "TBEventStreamer/BeamStreamer",
    "TBBPCRec/TBBPCRec",
#                       "ReadTBLArDigits",
    "AthenaMon/TBMon1",
    "AthenaMon/LArMon1",
    "TBLArRawChannelBuilder",
    "CaloCellMaker/CaloCellMaker", 
    "AthenaMon/CaloMonitor" ]

#
# Event selection (streaming)
#
# add some documentation
#
BeamStreamer = Algorithm( "BeamStreamer" )
#
BeamStreamer.ToolNames = [ "TBPatternUnitStreamerTool/BitSelector" ]
#
BeamStreamer.BitSelector.AcceptPattern = [ "S1", "S2", "S3" ]
BeamStreamer.BitSelector.RejectPattern = [ "VetoLeft",
                                           "VetoRight",
                                           "VetoUp",
                                           "VetoDown",
                                           "HoleVeto",
                                           "EarlyPU",
                                           "LatePU" ]

TBBPCRec =  Algorithm("TBBPCRec")
include( "TBRec/H6BPCRec_jobOptions.py" )
TBBPCRec.OutputLevel = WARNING

#ReadTBLArDigits = Algorithm( "ReadTBLArDigits" )
#ReadTBLArDigits.ContainerKey="FREE"
#ReadTBLArDigits.DumpFile="TBLArDigits.txt"
#ReadTBLArDigits.PrintCellLocation=TRUE
#ReadTBLArDigits.OutputLevel=3
#NtupleSvc = Service( "NtupleSvc" )
#NtupleSvc.Output=["FILE1 DATAFILE='TBLArDigits.root' TYP='ROOT' OPT='NEW'"]

#ReadLArDigits = Algorithm( "ReadLArDigits" )
#ReadLArDigits.ContainerKey="FREE"
#ReadLArDigits.DumpFile="LArDigits.txt"
#ReadLArDigits.PrintCellLocation=TRUE
#ReadLArDigits.OutputLevel=3

TBLArRawChannelBuilder = Algorithm("TBLArRawChannelBuilder");
TBLArRawChannelBuilder.OutputLevel = INFO
TBLArRawChannelBuilder.maxSamp     = 8
TBLArRawChannelBuilder.RecoMode    = "CUBIC"
TBLArRawChannelBuilder.CubicAdcCut = 10.

include ("CaloRec/CaloCellMakerFromRaw_H6_jobOptions.py" )

theApp.Dlls += [ "TBMonitoring" ]
theApp.Dlls += [ "LArMonitoring" ]

TBMon1  = Algorithm( "TBMon1" )
LArMon1 = Algorithm( "LArMon1" )

tdcbinnum = 100
tdcmin = 0
tdcmax = 1023

bpcmin    = -70*mm
bpcmax    =  70*mm
bpcbinnum =  70 

# ------------- BPC Monitor -------
TBMon1.AthenaMonTools += ["TBBPCMonTool/BPCmon"]
ToolSvc.BPCmon.MonitorBPC      = TRUE
ToolSvc.BPCmon.MonitorBPCRaw   = TRUE
ToolSvc.BPCmon.FakeDetectors   = FALSE
# H6 BPCs
ToolSvc.BPCmon.BPCNames = ["BPC0","BPC1","BPC2","BPC3","BPC4","BPC5"]
ToolSvc.BPCmon.BPCMaxPos = bpcmax
ToolSvc.BPCmon.BPCMinPos = bpcmin
ToolSvc.BPCmon.BPCBinNum = bpcbinnum
ToolSvc.BPCmon.TDCBinNum = tdcbinnum
ToolSvc.BPCmon.TDCMax    = tdcmax
ToolSvc.BPCmon.TDCMin    = tdcmin
#ToolSvc.BPCmon.OutputLevel =3

# # ------------- Scint Monitor -------
#TBMon1.AthenaMonTools += ["TBScintillatorMonTool/Scintmon"]
#ToolSvc.Scintmon.FakeDetectors   = FALSE
# # H6 scint names :
#ToolSvc.Scintmon.ScintNames = ["S1","S2","S3", "B",
#                               "Veto", 
#                               "muon1","muon2","muon3","muon4",
#                               "muon5","muon6","muon7","muon8",]

#ToolSvc.Scintmon.TDCBinNum = tdcbinnum
#ToolSvc.Scintmon.TDCMax = tdcmax
#ToolSvc.Scintmon.TDCMin = tdcmin
#ToolSvc.Scintmon.OutputLevel =3

# ------------- Scint Monitor -------
# TBMon1.AthenaMonTools += ["TBTailCatcherRawMonTool/TCmon"]

#------------- ADC Monitor -------
TBMon1.AthenaMonTools += ["TBADCRawMonTool/ADCmon"]
ToolSvc.ADCmon.TestADCMonTool   = FALSE
include( "TBCnv/H6ADCChannelNames.py" ) # list of ADC
ToolSvc.ADCmon.ADCBinNum = tdcbinnum
ToolSvc.ADCmon.ADCMax = tdcmax
ToolSvc.ADCmon.ADCMin = tdcmin

#------------- TDC Monitor -------
TBMon1.AthenaMonTools += ["TBTDCRawMonTool/TDCmon"]
ToolSvc.TDCmon.TestTDCMonTool   = FALSE
include( "TBCnv/H6TDCChannelNames.py" ) # list of TDC
ToolSvc.TDCmon.TDCBinNum = tdcbinnum
ToolSvc.TDCmon.TDCMax = tdcmax
ToolSvc.TDCmon.TDCMin = tdcmin

#------------- LArDigitMonitoring -------------
LArMon1.AthenaMonTools += ["LArDigMonTool<LArDigitContainer>/pedmon0"]
ToolSvc.pedmon0.histoPathBase = "/stat/pedsamp0"
ToolSvc.pedmon0.digitKey      = "FREE"
# ToolSvc.pedmon0.maxSamp       = 3
ToolSvc.pedmon0.pedMode       = "SAMPLE"
ToolSvc.pedmon0.pedSamp       = 0
#ToolSvc.pedmon0.OutputLevel   = 3

#------------- CaloResponse Monitoring ---------
CaloMonitor = Algorithm( "CaloMonitor" )
CaloMonitor.AthenaMonTools += [ "TBCaloResponseMonTool/calMonitor" ]
ToolSvc.calMonitor.OutputLevel = INFO
#  output
ToolSvc.calMonitor.histoPathBase     = "/stat/calomon"
#  input
ToolSvc.calMonitor.CellContainerName = "AllCalo"   # CaloCellContainer
ToolSvc.calMonitor.BPCContainerName  = "BPCCont"   # TBBPCCont
#
# IncludedCalos: possible are LAREM, LARHEC, LARFCAL, TILE, or
#                H6CALO = LAREM, LARHEC, LARFCAL
#                H8CALO = LAREM, TILE
#
H8CALO                               = [ "LAREM", "TILE" ]
H6CALO                               = [ "LAREM", "LARHEC", "LARFCAL" ]
ToolSvc.calMonitor.IncludedCalos     = H6CALO
#
# IncludedSamplings: possible are PreSamplerB, EMB1, EMB2, EMB3,
#                                 PreSamplerE, EME1, EME2, EME3,
#                                 TileBar0, TileBar1, TileBar2,
#                                 TileGap1, TileGap2, TileGap3,
#                                 TileExt0, TileExt1, TileExt2,
#                                 FCal1, FCal2, FCal3
#                              or
#                                 H6Samplings = EME2, EME3,
#                                               HEC0, HEC1, HEC2
#                                               FCal1, FCal2, FCal3
#                                 H8Samplings = PreSamplerB,
#                                               EMB1, EMB2, EMB3,
#                                               TileBar0, TileBar1, TileBar2
#
H6Samplings = [ "EME2", "EME3", "HEC0", "HEC1", "HEC2",
                "FCal1", "FCal2", "FCal3" ]
H8Samplings = [ "PreSamplerB", "EMB1", "EMB2", "EMB3",
                "TileBar0", "TileBar1", "TileBar2" ]
ToolSvc.calMonitor.IncludedSamplings = H6Samplings
#  calorimeter energy histogram definitions
ToolSvc.calMonitor.CaloEnergyBins    =  100
ToolSvc.calMonitor.CaloEnergyMin     =  -10.*GeV
ToolSvc.calMonitor.CaloEnergyMax     =  390.*GeV
#  calorimeter eta range
ToolSvc.calMonitor.CaloEtaBins       = 35
ToolSvc.calMonitor.CaloEtaMin        = 1.5
ToolSvc.calMonitor.CaloEtaMax        = 5.0
#  calorimeter phi range
ToolSvc.calMonitor.CaloPhiBins       = 64
ToolSvc.calMonitor.CaloPhiMin        =   90.*deg
ToolSvc.calMonitor.CaloPhiMax        =  180.*deg
#  BPC x/y ranges
ToolSvc.calMonitor.BPCXBins          = bpcbinnum
ToolSvc.calMonitor.BPCXMin           = bpcmin
ToolSvc.calMonitor.BPCXMax           = bpcmax
ToolSvc.calMonitor.BPCYBins          = bpcbinnum
ToolSvc.calMonitor.BPCYMin           = bpcmin
ToolSvc.calMonitor.BPCYMax           = bpcmax
#  average profile configurations
ToolSvc.calMonitor.MinEventsInBin    = 20
#  display unit switches
ToolSvc.calMonitor.EnergyUnits       = GeV
ToolSvc.calMonitor.LengthUnits       = mm
ToolSvc.calMonitor.AngleUnits        = rad
# -- use root histos --
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "/home/lochp/scan-136.root"

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
theApp.EvtMax = 100000

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel         = INFO

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = WARNING

# Dump all objects available
#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = TRUE
