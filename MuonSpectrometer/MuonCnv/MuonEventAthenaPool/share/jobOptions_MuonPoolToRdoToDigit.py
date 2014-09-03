# Load Muon POOL
include( "MuonEventAthenaPool/MuonPoolToRdo_jobOptions.py" )

# configure Muon system
include( "MuonEventAthenaPool/ConfigureMuons.py" )

# convert RDO to Digit
include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )

# read Muon Digits by CBNT
theApp.Dlls += [ "CBNT_Athena", "CBNT_Utils" ] 
theApp.TopAlg += [ "CBNT_Athena/CBNT_MuonRIO" ]
CBNT_MuonRIO = Algorithm( "CBNT_MuonRIO" )
CBNT_MuonRIO.members = [
  "ReadMdtDigit",
  "ReadRpcDigit",
  "ReadTgcDigit",
  "ReadCscDigit" 
]
ReadMdtDigit = Algorithm( "ReadMdtDigit" )
ReadMdtDigit.NtupleLocID = "/FILE1/MDT/145" 
ReadRpcDigit = Algorithm( "ReadRpcDigit" )
ReadRpcDigit.NtupleLocID = "/FILE1/RPC/143" 
ReadTgcDigit = Algorithm( "ReadTgcDigit" )
ReadTgcDigit.NtupleLocID = "/FILE1/TGC/147" 
ReadCscDigit = Algorithm( "ReadCscDigit" )
ReadCscDigit.NtupleLocID = "/FILE1/CSC/149" 
#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 3
# switch down Svc output level
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 3 
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 4 
DllClassManager = Algorithm( "DllClassManager" )
DllClassManager.OutputLevel = 4 
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000000
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='muonPoolDigit_R.root' OPT='NEW'" ]
# Histogram output, if any. 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
