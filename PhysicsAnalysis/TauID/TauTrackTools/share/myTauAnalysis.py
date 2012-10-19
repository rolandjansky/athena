import AthenaCommon.AtlasUnixStandardJob

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

DetDescrVersion = "ATLAS-CSC-02-00-00"
include( "RecExCond/AllDet_detDescr.py" )

# The AOD input file
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

# list of the algorithms to be executed at run time
from TauAnalysis.TauAnalysisConf import TauAnalysis__TruthAnalysis
from TauAnalysis.TauAnalysisConf import TauAnalysis__TrackingEfficiency
job += TauAnalysis__TruthAnalysis( "TruthAnalysis" )
job += TauAnalysis__TrackingEfficiency()


############# The properties of the Algorithm

TauAnalysis__TruthAnalysis.McParticleContainer = "SpclMC"
TauAnalysis__TruthAnalysis.InputTruthParticleContainer = "Spc1MC"
TauAnalysis__TruthAnalysis.OutputTauTruthParticleContainer = "TrueHadronicDecayProducts"
TauAnalysis__TruthAnalysis.TrackParticleContainer = "TrackParticleCandidate"
TauAnalysis__TruthAnalysis.OutputLevel = DEBUG

IsAtlfast = False

##################################
from GaudiSvc.GaudiSvcConf import THistSvc
if not hasattr(ServiceMgr, 'THistSvc'):
    ServiceMgr += THistSvc()

THistSvc = ServiceMgr.THistSvc
THistSvc.Output += ["histfile DATAFILE='hist1.root' OPT='RECREATE'"]


# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MsgSvc = Service( "MessageSvc" )
MsgSvc.OutputLevel = 3

# Number of Events to process
theApp.EvtMax = 10

###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE


