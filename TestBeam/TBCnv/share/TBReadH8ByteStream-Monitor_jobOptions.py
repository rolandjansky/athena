include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_H8_joboptions.py" )

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
ByteStreamInputSvc.RunNumber = [1000613]
#ByteStreamInputSvc.RunNumber = [1000536]


ToolSvc.TBByteStreamCnvTool.Dump = False

# --- TBRec --------------
theApp.Dlls += [ "TBRec" ]

##theApp.TopAlg += ["TBPhaseRec/TBPhaseRec"]
##TBPhaseRec =  Algorithm("TBPhaseRec")
##TBPhaseRec.PhaseTDCNames = ["frag0x12chan0","frag0x12chan1","asdasd"]
##TBPhaseRec.PhaseTDCToTime  = [-0.05*ns, -0.05*ns]  #  per TDC
##TBPhaseRec.PhaseTDCwac     = [ 660., 400.]  #  wrap around constants
##TBPhaseRec.PhaseTDCMin     = [    320.,     320.]  #  to help choosing the best TDC
##TBPhaseRec.TTCClockPeriod  = 25.*ns
##TBPhaseRec.TimeBins        = 25
##TBPhaseRec.TimeUnits       = ns
##include( "TBRec/H6PhaseRec_jobOptions.py" )





theApp.TopAlg += ["TBBPCRec/TBBPCRec"]
TBBPCRec =  Algorithm("TBBPCRec")
TBBPCRec.SGkey="BPCRawCont"
include( "TBRec/H8BPCRec_jobOptions.py" )
TBBPCRec.CalibFileName = "H8BPCCalib.txt"

theApp.TopAlg += ["TBScintillatorRec"]
TBScintRec =  Algorithm("TBScintillatorRec")
include( "TBRec/H8ScintRec_jobOptions.py" )


#--- Monitoring ---------------------------
theApp.Dlls += [ "AthenaMonitoring"]
theApp.TopAlg += [ "AthenaMon/athenamon" ]
theApp.Dlls += [ "TBMonitoring" ]
athenamon = Algorithm( "athenamon" )
#athenamon.AthenaMonTools = ["BeamDetectorMonTool/beamdetmon"]

tdcbinnum = 100
tdcmin = 0
#tdcmax = 1023
tdcmax = 4096

# ------------- BPC Monitor -------
athenamon.AthenaMonTools += ["TBBPCMonTool/BPCmon"]
ToolSvc.BPCmon.MonitorBPC      = TRUE
ToolSvc.BPCmon.MonitorBPCRaw   = TRUE
ToolSvc.BPCmon.FakeDetectors   = FALSE
# H8 BPCs
ToolSvc.BPCmon.BPCNames = ["BPC-1","BPC0","BPC1","BPC2"]
#ToolSvc.BPCmon.BPCNames = ["BPC-2","BPC-1","BPC0"]
ToolSvc.BPCmon.BPCMaxPos = 40
ToolSvc.BPCmon.BPCMinPos = -40
ToolSvc.BPCmon.BPCBinNum = 100

ToolSvc.BPCmon.BookAtFirstEvent = FALSE # If false, histos are booked before 1st event (no run number in histo title)

ToolSvc.BPCmon.TDCBinNum = tdcbinnum
ToolSvc.BPCmon.TDCMax = tdcmax
ToolSvc.BPCmon.TDCMin = tdcmin

# ------------- Scint Monitor -------
athenamon.AthenaMonTools += ["TBScintillatorMonTool/Scintmon"]
ToolSvc.Scintmon.FakeDetectors   = FALSE
# H8 scint names :
ToolSvc.Scintmon.ScintNames = ["S0","S1","S2-Up","S2-Down","S3-Left","S3-Right","C1","C2","muTag","muHalo","muVeto"
                               "SC1","SC2","MuWall0","MuWall1","MuWall2","MuWall3","MuWall4","MuWall5","MuWall6","MuWall7",
                               "MuWall8","MuWall9","MuWall10","MuWall11"]
                               

# TDC range : 0-4096; 1 TDC=35 ps -> 0-1434
# No pedestals yet.
ToolSvc.Scintmon.BookAtFirstEvent = FALSE # If false, histos are booked before 1st event (no run number in histo title)
ToolSvc.Scintmon.TDCBinNum = 300
ToolSvc.Scintmon.TDCMax = 150
ToolSvc.Scintmon.TDCMin = 0

# ------------- ADC Monitor -------
athenamon.AthenaMonTools += ["TBADCRawMonTool/ADCmon"]
include( "TBCnv/H8ADCChannelNames.py" )
ToolSvc.ADCmon.TestADCMonTool   = FALSE
ToolSvc.ADCmon.ADCBinNum = tdcbinnum
ToolSvc.ADCmon.ADCMax = tdcmax
ToolSvc.ADCmon.ADCMin = tdcmin

# ------------- TDC Monitor -------
athenamon.AthenaMonTools += ["TBTDCRawMonTool/TDCmon"]
include( "TBCnv/H8TDCChannelNames.py" )
ToolSvc.TDCmon.TestTDCMonTool   = FALSE
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
theApp.EvtMax = 100

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel =3

AthenaEventLoopMgr = Service( "AthenaEventLoopMgr" )
AthenaEventLoopMgr.OutputLevel =3
ToolSvc.TBByteStreamCnvTool.OutputLevel =3
ToolSvc.BPCmon.OutputLevel =3
ToolSvc.Scintmon.OutputLevel =3
ToolSvc.ADCmon.OutputLevel =3
ToolSvc.Scintmon.OutputLevel =3
TBScintRec.OutputLevel =3
TBBPCRec.OutputLevel=3;
# Dump all objects available
#StoreGateSvc=Service("StoreGateSvc")
#StoreGateSvc.Dump = True
