include( "AthenaCommon/Atlas.UnixStandardJob.py" )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

include( "TileDetMgrDetDescrCnv/TileDetMgrDetDescrCnv_jobOptions.py" )

#
#---- EMTB-Files input --------------------
#
theApp.Dlls += ["LArTBCnv"]
theApp.ExtSvc += [ "LArTBCnvSvc" , "LArTBEventSelector/EventSelector"]
theApp.EvtSel = "EventSelector"
EventPersistencySvc = Service( "EventPersistencySvc" )
EventPersistencySvc.CnvServices     += [ "LArTBCnvSvc" ]
EventSelector = Service( "EventSelector" )
EventSelector.InputFiles = ["/afs/cern.ch/user/r/rmcphers/w0/data/rd13_run220113.dat"]
#--- LArMonitoring ---------------------------
theApp.Dlls += [ "AthenaMonitoring"]
theApp.TopAlg += [ "AthenaMon/athenamon" ]
theApp.Dlls += [ "TBMonitoring" ]
athenamon = Algorithm( "athenamon" )
athenamon.AthenaMonTools = ["BeamDetectorMonTool/beamdetmon"]
# ToolSvc.beamdetmon.histoPathBase = "/toto";
ToolSvc = Service( "ToolSvc" )
ToolSvc.beamdetmon.MonitorBPC = TRUE
ToolSvc.beamdetmon.MonitorBPCRaw = FALSE
ToolSvc.beamdetmon.MonitorScint = FALSE
ToolSvc.beamdetmon.MonitorTrack = FALSE
ToolSvc.beamdetmon.MonitorTDC = FALSE
ToolSvc.beamdetmon.MonitorTrigPat = TRUE
ToolSvc.beamdetmon.FakeDetectors = FALSE
ToolSvc.beamdetmon.BPCNames = ["BPC1","BPC2"]
ToolSvc.beamdetmon.BPCMaxPos = 10
ToolSvc.beamdetmon.BPCMinPos = -10
ToolSvc.beamdetmon.BPCBinNum = 100
ToolSvc.beamdetmon.TDCBinNum = 100
ToolSvc.beamdetmon.TDCMax = 1023
ToolSvc.beamdetmon.TDCMin = 0
# ToolSvc.beamdetmon.TriggFlags = {0x00000001,0x00000002,0x00000004,0x00000008,0x00000010,0x00000011};
ToolSvc.beamdetmon.TriggFlags = [1,2,4,8,16,32]
# -- use root histos --
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "test.root"
# -- Use auditors --
theApp.Dlls += [ "GaudiAud" ]
# write out a summary of the time spent
#
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
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
# Dump all objects available
#StoreGateSvc.Dump = true ;
