# Options to set some configuration externally
evtmax=5000
MetPerfMonFile = 'METPerformance.root'

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

#input files
ServiceMgr.EventSelector.InputCollections = [ 
"AOD.pool.root"
                                            ]

include ( "MissingETPerformance/MissingETData_jobOptions.py" )
include ( "MissingETPerformance/MissingETMuonData_jobOptions.py" )
include ( "MissingETPerformance/MissingETEventSelector_jobOptions.py" )
include ( "MissingETPerformance/ResolutionTool_jobOptions.py" )
include ( "MissingETPerformance/BasicPlots_jobOptions.py" )
include ( "MissingETPerformance/MissingETComposition_jobOptions.py" )
include ( "MissingETPerformance/Linearity_jobOptions.py" )
include ( "MissingETPerformance/JetVariablesTool_jobOptions.py" )
include ( "MissingETPerformance/TopoClusterVariablesTool_jobOptions.py" )
#include ( "MissingETPerformance/TrigMissingET_jobOptions.py" )
#include ( "MissingETPerformance/TrigVsOfflineMissingET_jobOptions.py" )
#include ( "MissingETPerformance/EtaRings_jobOptions.py" )
#include ( "MissingETPerformance/FakeMissingET_jobOptions.py" )
#include ( "MissingETPerformance/MuonTool_jobOptions.py" )
#include ( "MissingETPerformance/ContainerComparatorTool_jobOptions.py" )
#include ( "MissingETPerformance/ZMuMu_jobOptions.py" )
#include ( "MissingETPerformance/Zee_jobOptions.py" )
#include ( "MissingETPerformance/PileUp_jobOptions.py" )

# MET perfmon
from MissingETPerformance.MissingETPerformanceConf import MissingETPerformance
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += MissingETPerformance()
MissingETPerformance = MissingETPerformance()

############# The properties of the MissingETPerformance Algorithm
MissingETPerformance.OutputLevel = WARNING

############## trigger
#whether to require a trigger from the list to have fired
MissingETPerformance.FilterOnTrigger = False
#List of trigger names to accept
MissingETPerformance.TriggerNames = ["EF_e15_medium","EF_e5_medium","EF_J50"]

#turn on/off individual tools here
MissingETPerformance.doEventSelector                    = False
MissingETPerformance.doBasicPlotsTool                   = True
MissingETPerformance.doResolutionTool                   = True
MissingETPerformance.doLinearityTool                    = False
MissingETPerformance.doEtaRingsTool                     = False
MissingETPerformance.doTrigMissingETTool                = False
MissingETPerformance.doTrigVsOfflineMissingETTool       = False
MissingETPerformance.doFakeMissingETTool                = False
MissingETPerformance.doMuonTool                         = False
MissingETPerformance.doContainerComparatorTool          = False
MissingETPerformance.doZMuMuTool                        = False
MissingETPerformance.doZeeTool                          = False
MissingETPerformance.doPileUpTool                       = False
MissingETPerformance.doMissingETCompositionTool         = False

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
aantStr = "AANT DATAFILE='%s' OPT='RECREATE'" % (MetPerfMonFile)
ServiceMgr.THistSvc.Output = [ aantStr ]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.OutputName = MetPerfMonFile
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING

# Number of Events to process
theApp.EvtMax = evtmax

AthenaEventLoopMgr = Service ( "AthenaEventLoopMgr")
try:
  AthenaEventLoopMgr.EventPrintoutInterval = 100
except Exception:
  print 'EventPrintoutInterval exception, defaults to 1'
