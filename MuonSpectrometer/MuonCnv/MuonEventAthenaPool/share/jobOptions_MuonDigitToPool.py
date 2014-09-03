#--------------------------------------------------------------
# jobOptions for writing Digit to POOL
#--------------------------------------------------------------
# souce of Digit
include( "MuonDigitZebraCnv/G3MuonDigit_jobOptions.py" )

# configure Muon system
include( "MuonEventAthenaPool/ConfigureMuons.py" )

# read Muon Digits by CBNT
include( "MuonEventAthenaPool/ReadMuonDigit.py" )

#--------------------------------------------------------------
# write out Transient Event Store content in POOL
#--------------------------------------------------------------
include( "MuonEventAthenaPool/MuonDigitToPool_jobOptions.py" )

#--------------------------------------------------------------
# Event parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
# Output levels
#--------------------------------------------------------------
# Set output level threshold 
MessageSvc.OutputLevel  = 3 
# switch down Svc output level
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 3 
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 4 
DllClassManager = Algorithm( "DllClassManager" )
DllClassManager.OutputLevel = 4 
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='muonPoolDigit_W.root' OPT='NEW'" ]
# Histogram output, if any. 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
