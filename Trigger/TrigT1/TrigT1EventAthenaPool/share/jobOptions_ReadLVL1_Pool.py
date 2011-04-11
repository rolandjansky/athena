#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

# DetDescr 
include( "TDRDetDescrAthenaRoot/TDRDetDescrAthenaRoot_jobOptions.py" )

theApp.Dlls += [ "TrigT1Muctpi" ]
theApp.Dlls += [ "TrigT1RoIB" ]
# Algs
theApp.Dlls += [ "CBNT_Athena", "CBNT_Utils" ] 
theApp.TopAlg += [ "CBNT_Athena/CBNT_LVL1" ]
CBNT_LVL1 = Algorithm( "CBNT_LVL1" )
CBNT_LVL1.members = [
 "ReadMuCTPI_RDO",
 "ReadRoIBResult"
]
ReadMuCTPI_RDO = Algorithm( "ReadMuCTPI_RDO" )
ReadMuCTPI_RDO.NtupleLocID     = "/FILE1/MUCTPI/200"
ReadReadRoIBResult = Algorithm( "ReadReadRoIBResult" )
ReadReadRoIBResult.NtupleLocID = "/FILE1/ROIB/201"

# Define input
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = [ "LVL1Pool.root" ]

# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc.OutputLevel      = 2
# switch down Svc output level
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 2 
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 4 
DllClassManager = Algorithm( "DllClassManager" )
DllClassManager.OutputLevel = 4 
#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 1000000
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='lvl1POOL_R.root' OPT='NEW'" ]
# Histogram output, if any. 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
