# TriggerExample_jobOptions.py
# Based on UserAnalysis/AnalysisSkeleton_topOptions.py

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# The AOD input file
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]

# Create the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Add the analysis algorithm:
from AnalysisExamples.AnalysisExamplesConf import TrigAnalysisExample
topSequence += TrigAnalysisExample()
topSequence.TrigAnalysisExample.OutputLevel = INFO

##
##########################################
## Set up trigger configuration service and metadata service
## it relies on, for analysis job without RecExCommon

from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_atlas()

# set up trigger configuration service
from TriggerJobOpts.TriggerConfigGetter import TriggerConfigGetter
cfg =  TriggerConfigGetter("ReadPool")

## set up trigger decision tool
from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
ToolSvc += Trig__TrigDecisionTool( "TrigDecisionTool" )
topSequence.TrigAnalysisExample.TrigDecisionTool = ToolSvc.TrigDecisionTool

## END of trigger setup
##

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = WARNING

# Number of Events to process
theApp.EvtMax = 10

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE
