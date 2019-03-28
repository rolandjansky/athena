
#--------------------------------------------------------------
# Get Global flags
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()
GlobalFlags.InputFormat.set_pool()
GlobalFlags.Print()
include( "AthenaCommon/AthenaCommonFlags.py" )

include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )

#--------------------------------------------------------------
# Pool Converters
#--------------------------------------------------------------
include( "TrigT1EventAthenaPool/TrigT1EventAthenaPool_joboptions.py" )

#--------------------------------------------------------------
# Pool input
#--------------------------------------------------------------
EventSelector = Service( "EventSelector" )
EventSelector.InputCollections = PoolInput

#
# Load the configuration:
#
include( "TrigT1Config/TrigT1Config_jobOptions.py" )

theApp.Dlls += [ "TrigT1CTP" ]
CTP_RDO = Algorithm( "CTP_RDO" )
CTP_RDO.NtupleLocID = "/FILE1/CTP/200"
CTP_RDO.OutputLevel = VERBOSE

#--------------------------------------------------------------
# NTuple output
#--------------------------------------------------------------
theApp.Dlls += ["RootHistCnv"]
theApp.HistogramPersistency = "ROOT"
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output = RootOutput

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel  = INFO
MessageSvc.defaultLimit = 10000000

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = EvtMax
theApp.EvtSel = "EventSelector"
