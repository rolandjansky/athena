include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]

#
#---- ByteStream-Files input --------------------
#


include( "TBCnv/TBReadH8BS_jobOptions.py" )
ByteStreamInputSvc = Service( "ByteStreamInputSvc" )

# With H8 : - See also TBReadH8BS_jobOptions.py !!
#ByteStreamInputSvc.InputDirectory += ["/afs/cern.ch/user/d/delsart/myAthena/Reconstruction/RecExample/RecExCommon/RecExCommon-00-01-26/run"]
#ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/tilecal/2004/daq/"]
ByteStreamInputSvc.InputDirectory += ["/castor/cern.ch/atlas/testbeam/combined/2004/"]
#ByteStreamInputSvc.InputDirectory += ["/afs/cern.ch/user/d/delsart/myAthena/Reconstruction/RecExample/RecExTB/RecExTB-00-00-27/run"]
# Prefix #
ByteStreamInputSvc.FilePrefix += ["daq_SFI-51_calo"]
# Run number 
#ByteStreamInputSvc.RunNumber = [1000853]
ByteStreamInputSvc.RunNumber = [1000944]
#ByteStreamInputSvc.RunNumber = [1000536]


ToolSvc.TBByteStreamCnvTool.Dump = False


#---- LAr BS Reading & Reconstruction
include("LArTBRec/LArTBRec_H8_Simple_jobOptions.py")
theApp.Dlls+=["LArTools"]
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )
LArRawChannelSimpleBuilder.RecoMode    = "CUBIC" 


# make CaloCellContainer
doTile=True
doSim=False
doLArHVCorrection=True
include ( "CaloTBRec/CaloTBRec_H8_Cell_jobOptions.py" )



# --- TBRec --------------
theApp.Dlls += [ "TBRec" ]

theApp.TopAlg += ["TBPhaseRec/TBPhaseRec"]
TBPhaseRec =  Algorithm("TBPhaseRec")
include("TBRec/H8PhaseRec_jobOptions.py")
##TBPhaseRec.PhaseTDCNames = ["frag0x12chan0","frag0x12chan1","asdasd"]
##TBPhaseRec.PhaseTDCToTime  = [-0.05*ns, -0.05*ns]  #  per TDC
##TBPhaseRec.PhaseTDCwac     = [ 660., 400.]  #  wrap around constants
##TBPhaseRec.PhaseTDCMin     = [    320.,     320.]  #  to help choosing the best TDC
##TBPhaseRec.TTCClockPeriod  = 25.*ns
##TBPhaseRec.TimeBins        = 25
##TBPhaseRec.TimeUnits       = ns
##include( "TBRec/H6PhaseRec_jobOptions.py" )


theApp.TopAlg += ["TBScintillatorRec"]
TBScintRec =  Algorithm("TBScintillatorRec")
TBScintRec.SGkey="ScintillatorRawCont"
include( "TBRec/H8ScintRec_jobOptions.py" ) 
theApp.TopAlg += ["TBBPCRec"]
TBBPCRec =  Algorithm("TBBPCRec")
TBBPCRec.SGkey="BPCRawCont"
include( "TBRec/H8BPCRec_jobOptions.py" )
#theApp.TopAlg += ["TBPhaseRec/TBPhaseRec"]
#TBPhaseRec =  Algorithm("TBPhaseRec")
#include( "TBRec/H8PhaseRec_jobOptions.py" )

theApp.Dlls += [ "CBNT_Utils" ]
theApp.Dlls += [ "CBNT_Athena" ]
theApp.TopAlg += [ "CBNT_Athena" ]

CBNT_Athena = Algorithm( "CBNT_Athena" )
CBNT_Athena.NtupleLocID="/FILE1/CBNT/t333"

CBNT_Athena.Members += [ "CBNT_BeamInstrumentation" ]
CBNT_BeamInstrumentation = Algorithm( "CBNT_BeamInstrumentation" )
CBNT_BeamInstrumentation.BPCnames=["BPC-2","BPC-1","BPC0","BPC1","BPC2"];
CBNT_BeamInstrumentation.Scintnames=["S0","S1","S2-Up","S2-Down","S3-Left","S3-Right","C1","C2","muTag","muHalo","muVeto"];

CBNT_Athena.Members += [ "CBNT_Timing" ]
CBNT_Timing = Algorithm( "CBNT_Timing" )
CBNT_Timing.EnergyCut = 2.*GeV
CBNT_Timing.IncludedSamplings += ["EMB2", "EMB1", "EMB0", "EMB3"]


#--- Histogram based monitoring ---------------------------
#theApp.Dlls += [ "AthenaMonitoring"]
#theApp.Dlls += ["CaloMonitoring"];
#theApp.Dlls += [ "TBMonitoring" ]

# We put the Monitoring in a sequencer to skip calibration events
#theApp.TopAlg += [ "TBAlgoSequencer/Seq" ]
#Seq = Algorithm( "Seq" )
#Seq.OutputLevel=INFO
#Seq.SubAlgorithms = ["TBEventStreamer/BeamStreamer",
#                     "AthenaMon/CaloMon"
#                     ]
                                                                                                              
# Steer Beam Streamer
#BeamStreamer = Algorithm( "BeamStreamer" )
#BeamStreamer.ToolNames = [ "TBPatternUnitStreamerToolH8/BitSelector" ]
#BeamStreamer.BitSelector.AcceptPattern = [ "Physics" ]
#BeamStreamer.OutputLevel=INFO

#CaloMon=Algorithm("CaloMon");
#CaloMon.AthenaMonTools += ["CaloCellMonitoringTool/CaloCellMon"]
#ToolSvc.CaloCellMon.histoPathBase = "/stat"
#ToolSvc.CaloCellMon.OutputLevel=0;
#ToolSvc.CaloCellMon.NPreHistogrammingEvents=50


#athenamon = Algorithm( "athenamon" )
#athenamon.AthenaMonTools = ["BeamDetectorMonTool/beamdetmon"]

tdcbinnum = 100
tdcmin = 0
#tdcmax = 1023
tdcmax = 4096

# ------------- TDC Monitor -------
#CaloMon.AthenaMonTools += ["TBTDCRawMonTool/TDCmon"]
#include( "TBCnv/H8TDCChannelNames.py" )
#ToolSvc.TDCmon.TestTDCMonTool   = FALSE
#ToolSvc.TDCmon.TDCBinNum = tdcbinnum
#ToolSvc.TDCmon.TDCMax = tdcmax
#ToolSvc.TDCmon.TDCMin = tdcmin


# ------------- Phase Monitor -------
#CaloMon.AthenaMonTools += ["TBPhaseMonTool/Phasemon"]
#ToolSvc.Phasemon.PhaseTDCNames = [
#    "frag0x12chan0"]
#ToolSvc.Phasemon.IncludedSamplings += ["EMB2"]
#ToolSvc.Phasemon.TimeMin = 25
#ToolSvc.Phasemon.TimeMax =  3.*25.*ns
#ToolSvc.Phasemon.EnergyCut = 10.*GeV
#ToolSvc.Phasemon.TDCBins = 4096
#ToolSvc.Phasemon.TDCMin = 0
#ToolSvc.Phasemon.TDCMax = 4095


# -- use root histos --
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
#HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
#HistogramPersistencySvc.OutputFile  = "test.root"


NTupleSvc = Service( "NTupleSvc" )
#NTOutputFileString="FILE1 DATAFILE='%(file)s' OPT='NEW'" % {"file" : NTOutputFileName}
NTupleSvc.Output = [ "FILE1 DATAFILE='H8TimingMon.root' OPT='NEW'" ]
#NTupleSvc.Output = [TOutputFileString]




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
MessageSvc.OutputLevel =5

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel =5
#ToolSvc.TBByteStreamCnvTool.OutputLevel =2
#ToolSvc.BPCmon.OutputLevel =3
#ToolSvc.Scintmon.OutputLevel =3
#ToolSvc.ADCmon.OutputLevel =3
#ToolSvc.Scintmon.OutputLevel =3
#TBScintRec.OutputLevel =3
#TBBPCRec.OutputLevel=3;
# Dump all objects available
#StoreGateSvc=Service("StoreGateSvc")
#StoreGateSvc.Dump = True
