#=============================================================
#
# jopOptions file for writing BS from ZEBRA DC1 input. 
#
#==============================================================
#--------------------------------------------------------------
#  Read in Zebra MC event
#--------------------------------------------------------------
theApp.setup( ZEBRA )

# specify input file
theApp.EvtSel = "EventSelector"
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
# write out Transient Event Store content in POOL
#--------------------------------------------------------------
include( "AthenaPoolCnvSvc/WriteAthenaPool_jobOptions.py" )

# Define the output Db parameters (the default value are shown)
Stream1.OutputFile  = "LVL1Pool.root"

# Converters:
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )
include( "TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Event parameters
#--------------------------------------------------------------
# For runs: provide a first/last range or a list
EventSelector.runs = [1, 30000]
# Switch of detectors not needed
EventSelector.muons=FALSE
EventSelector.indet=FALSE
EventSelector.pixel=FALSE
EventSelector.sct=FALSE
EventSelector.trt=FALSE
EventSelector.calos=FALSE
EventSelector.tile=FALSE
EventSelector.hec=FALSE
EventSelector.fcal=FALSE
#--------------------------------------------------------------
# Use auditors
#--------------------------------------------------------------
theApp.Dlls += [ "GaudiAud" ]
# write out a summary of the time spent
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
# write out s summary of the memory usage
theAuditorSvc.Auditors  += [ "MemStatAuditor" ]
MemStatAuditor = theAuditorSvc.auditor( "MemStatAuditor" )
MemStatAuditor.OutputLevel = 4 
#--------------------------------------------------------------
# Output levels
#--------------------------------------------------------------
# Set output level threshold 
# (0=NIL 1=VERBOSE, 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel  = 2 
# switch down Svc output level
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.OutputLevel = 2 
ClassIDSvc = Service( "ClassIDSvc" )
ClassIDSvc.OutputLevel = 4 
DllClassManager = Algorithm( "DllClassManager" )
DllClassManager.OutputLevel = 4 
MuonDetMgrDetDescrCnv = Service( "MuonDetMgrDetDescrCnv" )
MuonDetMgrDetDescrCnv.OutputLevel = 4 
# list of output objects key
# EventInfo
Stream1 = Algorithm( "Stream1" )
Stream1.ItemList+=["2101#*"]
# MuCTPI
Stream1.ItemList+=["6270#*"]
# RoIB
Stream1.ItemList+=["6000#*"]
# run LVL1
##include "TrigT1RPCRecRoiSvc/TrigT1RPCRecRoiSvcJobOptions.txt"
##include "TrigT1TGCRecRoiSvc/TrigT1TGCRecRoiSvcJobOptions.txt"
include( "TrigT1Config/TrigT1ConfigJobOptions.py" )

include( "TrigT1Muctpi/TrigT1Muctpi_jobOptions.py" )

include( "TrigT1CTP/TrigT1CTPJobOptions.py" )

include( "TrigT1RoIB/TrigT1RoIBJobOptions.py" )

L1Muctpi = Algorithm( "L1Muctpi" )
L1Muctpi.OutputLevel = 2
#---------------------------------------------------------------
# Ntuple service output
#---------------------------------------------------------------
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
theApp.Dlls += [ "RootHistCnv" ]
theApp.HistogramPersistency = "ROOT"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = [ "FILE1 DATAFILE='lvl1POOL_W.root' OPT='NEW'" ]
# Histogram output, if any.
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
