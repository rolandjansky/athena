include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

# include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )
# include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H6_joboptions.py" )
#

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

#---- ByteStream-Files input --------------------
#

# include( "TBCnv/TBReadH6BS_jobOptions.py" )
include( "TBCnv/TBReadH6BS_EventStorage_jobOptions.py" )
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

# ByteStreamInputSvc.InputDirectory += ["/scratchdisk1/h6data"]
ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/cbt-ec2/phys"]
ByteStreamInputSvc.FilePrefix     += ["daq_beamDAQ_H6"]

ToolSvc.TBByteStreamCnvTool.Dump        = FALSE
ToolSvc.TBByteStreamCnvTool.OutputLevel = WARNING

#--- Monitoring ---------------------------

theApp.TopAlg += [ "TBAlgoSequencer/Seq1" ]

theApp.Dlls += [ "AthenaMonitoring"]

Seq1 = Algorithm( "Seq1" )

Seq1.SubAlgorithms = [
    "TBEventStreamer/EventInfoStreamer",
    "AthenaMon/TBMon0",
    "TBEventStreamer/BeamStreamer",
    "TBBPCRec/TBBPCRec",
    "TBPhaseRec/TBPhaseRec",
    "TBScintillatorRec/TBScintRec",
    "TBTailCatcherRec/TBTailCatcherRec",
    "TBMWPCRec/TBMWPCRec",
    "AthenaMon/TBMon1",
    "AthenaMon/LArMon1",
    "LArRawChannelSimpleBuilder/TBLArRawChannelBuilder",
    "CaloCellMaker/CaloCellMaker",
    "AthenaMon/CaloCellMon",
    "AthenaMon/CaloMonitor",
    "AthenaMon/PhaseMon"
    ]

EventInfoStreamer = Algorithm( "EventInfoStreamer" )
EventInfoStreamer.ToolNames = [ "TBEventInfoStreamerTool/EventTypeSelector" ]
# possible selection: "Physics", "Special", "f/e_Calibration",
# "Random", "BPC_Calibration", default: "Physics"
EventInfoStreamer.EventTypeSelector.AcceptedTypes=["Physics"]

#
BeamStreamer = Algorithm( "BeamStreamer" )
#
BeamStreamer.ToolNames = [ "TBPatternUnitStreamerTool/BitSelector" ]
#
BeamStreamer.BitSelector.AcceptPattern = [ "S1", "S2", "S3" ]
BeamStreamer.BitSelector.RejectPattern = [
#                                           "VetoLeft",
#                                           "VetoRight",
#                                           "VetoUp",
#                                           "VetoDown",
#                                           "HoleVeto",
                                           "EarlyPU",
                                           "LatePU"
                                           ]

TBMon0 = Algorithm("TBMon0")
TBMon0.AthenaMonTools += ["TBTriggerPatternUnitMonTool/PatternUnitMon"]
ToolSvc.PatternUnitMon.FakeDetectors = FALSE
ToolSvc.PatternUnitMon.TriggFlags = [0x00000001,  # S1 accept
                                     0x00000002,  # S2 accept
                                     0x00000004,  # S3 accept
                                     0x00000008,  # B2 accept
                                     0x00000010,  # VetoLeft 
                                     0x00000020,  # VetoRight
                                     0x00000040,  # VetoDown
                                     0x00000080,  # VetoUp
                                     0x00000100,  # Mu 1 Left
                                     0x00000200,  # Mu 1 Right
                                     0x00000400,  # Mu 2 Left
                                     0x00000800,  # Early PileUp
                                     0x00001000,  # Late PileUp
                                     0x00002000,  # In Spill Tag
                                     0x00004000,  # Off Spill Tag
                                     0x00008000,  # Cedar 6 of 8 segments
                                     0x00010000,  # Mu 2 Right
                                     0x00020000,  # Hole Counter
                                     0x00040000,  # W1*W2 coincidence
                                     0x00080000,  # B1 accept
                                     0x00100000,  # Scaler Reset Tag
                                     0x08000000,  # Cedar 7 of 8 segments
                                     0x10000000,  # Cedar 8 of 8 segments
                                     ]

TBBPCRec =  Algorithm("TBBPCRec")
include( "TBRec/H6BPCRec_jobOptions.py" )
TBBPCRec.OutputLevel = INFO
TBBPCRec.CalibFileName = "H6BPCCalib.txt"

TBPhaseRec =  Algorithm("TBPhaseRec")
include( "TBRec/H6PhaseRec_jobOptions.py" )
TBPhaseRec.OutputLevel = INFO

TBScintRec =  Algorithm("TBScintRec")
include( "TBRec/H6ScintRec_jobOptions.py" )
TBScintRec.OutputLevel = INFO

TBTailCatcherRec =  Algorithm("TBTailCatcherRec")
include( "TBRec/H6TailCatcherRec_jobOptions.py" )

TBMWPCRec =  Algorithm("TBMWPCRec")
include( "TBRec/H6MWPCRec_jobOptions.py" )

TBLArRawChannelBuilder = Algorithm("TBLArRawChannelBuilder");
TBLArRawChannelBuilder.OutputLevel = INFO
TBLArRawChannelBuilder.maxSamp     = 3
TBLArRawChannelBuilder.RecoMode    = "CUBIC"
TBLArRawChannelBuilder.CubicAdcCut = 50.
TBLArRawChannelBuilder.OutputLevel = INFO

include ("CaloRec/CaloCellMakerFromRaw_H6_jobOptions.py" )

theApp.Dlls += [ "TBMonitoring" ]
theApp.Dlls += [ "LArMonitoring" ]
theApp.Dlls += [ "CaloMonitoring" ]

TBMon1  = Algorithm( "TBMon1" )
LArMon1 = Algorithm( "LArMon1" )

tdcbinnum = 100
tdcmin = 0
tdcmax = 2047

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
ToolSvc.BPCmon.BookAtFirstEvent = TRUE # If false, histos are booked before 1st event (no run number in histo title)

# ------------- Scint Monitor -------
TBMon1.AthenaMonTools += ["TBScintillatorMonTool/Scintmon"]
ToolSvc.Scintmon.FakeDetectors   = FALSE
# H6 scint names :
ToolSvc.Scintmon.ScintNames = ["S1","S2","S3", "B",
                               "Veto","Halo",
                               "muon1","muon2","muon3","muon4",
                               "muon5","muon6","muon7","muon8"]
ToolSvc.Scintmon.BookAtFirstEvent = TRUE # If false, histos are booked before 1st event (no run number in histo title)
ToolSvc.Scintmon.TDCBinNum = tdcbinnum
ToolSvc.Scintmon.TDCMax = tdcmax
ToolSvc.Scintmon.TDCMin = tdcmin
ToolSvc.Scintmon.OutputLevel = INFO

# ------------- MWPC Monitor -------
TBMon1.AthenaMonTools += ["TBMWPCMonTool/MWPCmon"]
ToolSvc.MWPCmon.MonitorMWPC      = TRUE
ToolSvc.MWPCmon.MonitorMWPCRaw   = TRUE
#ToolSvc.MWPCmon.FakeDetectors    = FALSE


# ------------- TailCatcher Monitor -------
TBMon1.AthenaMonTools += ["TBTailCatcherRawMonTool/TCmon"]
ToolSvc.TCmon.MonitorTailCatcherRaw = TRUE
ToolSvc.TCmon.MonitorTailCatcherRaw = TRUE
ToolSvc.TCmon.OutputLevel = INFO

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
ToolSvc.pedmon0.maxSamp       = 3
ToolSvc.pedmon0.pedMode       = "SAMPLE"
ToolSvc.pedmon0.pedSamp       = 0
ToolSvc.pedmon0.adcCut        = 50
ToolSvc.pedmon0.OutputLevel   = INFO

#------------- LArDigitTBPhaseMonitoring -------------
from LArCabling.LArCablingAccess import LArOnOffIdMapping
LArOnOffIdMapping()

LArMon1.AthenaMonTools += ["LArDigTBPhaseMonTool<LArDigitContainer>/LArDigitTBTDC0"]
ToolSvc.LArDigitTBTDC0.histoPathBase = "/stat/LArDigitTBTDC0"
ToolSvc.LArDigitTBTDC0.digitKey      = "FREE"
ToolSvc.LArDigitTBTDC0.TBPhaseName   = "TBPhase"
ToolSvc.LArDigitTBTDC0.pedMode       = "SAMPLE"
ToolSvc.LArDigitTBTDC0.pedSamp       = 0
#ToolSvc.LArDigitTBTDC0.OutputLevel   = INFO

#------------- CaloCell Monitoring ---------
CaloCellMon = Algorithm( "CaloCellMon")

CaloCellMon.AthenaMonTools = ["CaloCellMonitoringTool/calocelltool"]
ToolSvc.calocelltool.OutputLevel = INFO

#------------- CaloResponse Monitoring ---------
CaloMonitor = Algorithm( "CaloMonitor" )
CaloMonitor.AthenaMonTools += [ "TBCaloResponseMonTool/calMonitor" ]
ToolSvc.calMonitor.OutputLevel = INFO
# output
ToolSvc.calMonitor.histoPathBase     = "/stat/calomon"
# input
ToolSvc.calMonitor.CellContainerName = "AllCalo"   # CaloCellContainer
ToolSvc.calMonitor.BPCContainerName  = "BPCCont"   # TBBPCCont
#
#IncludedCalos: possible are LAREM, LARHEC, LARFCAL, TILE, or
#               H6CALO = LAREM, LARHEC, LARFCAL
#               H8CALO = LAREM, TILE
#
H8CALO                               = [ "LAREM", "TILE" ]
H6CALO                               = [ "LAREM", "LARHEC", "LARFCAL" ]
ToolSvc.calMonitor.IncludedCalos     = H6CALO

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
# calorimeter energy histogram definitions
ToolSvc.calMonitor.CaloEnergyBins    =  100
ToolSvc.calMonitor.CaloEnergyMin     =  -10.*GeV
ToolSvc.calMonitor.CaloEnergyMax     =  390.*GeV
# calorimeter eta range
ToolSvc.calMonitor.CaloEtaBins       = 35
ToolSvc.calMonitor.CaloEtaMin        = 1.5
ToolSvc.calMonitor.CaloEtaMax        = 5.0
# calorimeter phi range
ToolSvc.calMonitor.CaloPhiBins       = 64
ToolSvc.calMonitor.CaloPhiMin        =   90.*deg
ToolSvc.calMonitor.CaloPhiMax        =  180.*deg
# BPC x/y ranges
ToolSvc.calMonitor.BPCXBins          = bpcbinnum
ToolSvc.calMonitor.BPCXMin           = bpcmin
ToolSvc.calMonitor.BPCXMax           = bpcmax
ToolSvc.calMonitor.BPCYBins          = bpcbinnum
ToolSvc.calMonitor.BPCYMin           = bpcmin
ToolSvc.calMonitor.BPCYMax           = bpcmax
# average profile configurations
ToolSvc.calMonitor.MinEventsInBin    = 20
# display unit switches
ToolSvc.calMonitor.EnergyUnits       = GeV
ToolSvc.calMonitor.LengthUnits       = mm
ToolSvc.calMonitor.AngleUnits        = rad

#------------- Phase Monitor -------
PhaseMon = Algorithm( "PhaseMon" )
PhaseMon.AthenaMonTools = [ "TBPhaseMonTool/phaseMonitor" ]
ToolSvc.phaseMonitor.histoPathBase         = "/stat/phasemon"
ToolSvc.phaseMonitor.TBTDCRawContainerName = "TDCRawCont"  # TBTDCRawContainer
ToolSvc.phaseMonitor.CellContainerName     = "AllCalo"     # CaloCellContainer
ToolSvc.phaseMonitor.PhaseTDCNames         = ["word1frag0x03chan0","word2frag0x03chan0"]
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
# H6Samplings = [ "EME2", "EME3", "HEC0", "HEC1", "HEC2", "FCal1", "FCal2", "FCal3" ]
# H8Samplings = [ "PreSamplerB", "EMB1", "EMB2", "EMB3", "TileBar0", "TileBar1", "TileBar2" ]
ToolSvc.phaseMonitor.IncludedSamplings     = H6Samplings
ToolSvc.phaseMonitor.TBPhaseName           = "TBPhase"
ToolSvc.phaseMonitor.EnergyCut             = 5.*GeV
ToolSvc.phaseMonitor.TDCBins               = 80
ToolSvc.phaseMonitor.TDCMin                = 200
ToolSvc.phaseMonitor.TDCMax                = 1000
ToolSvc.phaseMonitor.TDCRightMin           = 200
ToolSvc.phaseMonitor.TDCRightMax           = 300
ToolSvc.phaseMonitor.TDCLeftMin            = -300
ToolSvc.phaseMonitor.TDCLeftMax            = -200
ToolSvc.phaseMonitor.TimeBins              = 100
# ToolSvc.phaseMonitor.TimeMin               = 180.*ns
# ToolSvc.phaseMonitor.TimeMax               = 220.*ns
ToolSvc.phaseMonitor.TimeMin               = 45.*ns
ToolSvc.phaseMonitor.TimeMax               = 95.*ns
ToolSvc.phaseMonitor.PhaseBins             = 70
ToolSvc.phaseMonitor.PhaseMin              = -5.*ns
ToolSvc.phaseMonitor.PhaseMax              = 30.*ns
ToolSvc.phaseMonitor.TimeUnits             = ns
ToolSvc.phaseMonitor.OutputLevel           = INFO

# -- use root histos --
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )

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
theApp.EvtMax = 1000

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel         = INFO

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel = WARNING

# Dump all objects available
# StoreGateSvc = Service( "StoreGateSvc" )
# StoreGateSvc.Dump = TRUE

ByteStreamInputSvc.RunNumber       = [1367]
HistogramPersistencySvc.OutputFile  = "run1367.root"
