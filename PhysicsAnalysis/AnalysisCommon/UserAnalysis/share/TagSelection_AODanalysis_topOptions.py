# Author: Ketevi A. Assamagan
# BNL, June 12, 2004

#get a handle on the ServiceManager which holds all the services
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

# The TAG input file and TAG event selection
ServiceMgr.EventSelector.InputCollections = [ "TAG.pool.root" ]
ServiceMgr.EventSelector.Query="NLooseMuon>0 || NLooseElectron>0"
ServiceMgr.EventSelector.CollectionType = "ExplicitROOT"

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

############# The properties of the AnalysisSkeleton Algorithm
from UserAnalysis.UserAnalysisConf import AnalysisSkeleton
topSequence.CBNT_AthenaAware += AnalysisSkeleton()
AnalysisSkeleton = AnalysisSkeleton()

AnalysisSkeleton.ElectronContainer = "ElectronAODCollection"
AnalysisSkeleton.MCParticleContainer = "SpclMC"
AnalysisSkeleton.DeltaRMatchCut = 0.2
AnalysisSkeleton.MaxDeltaR = 0.9999
AnalysisSkeleton.ElectronEtCut  = 10.0*GeV
AnalysisSkeleton.ElectronEtaCut = 2.5
AnalysisSkeleton.ElectronCone   = 0.9
AnalysisSkeleton.OutputLevel = INFO

##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='AnalysisSkeleton.TagSel.aan.root' OPT='RECREATE'"]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'AnalysisSkeleton.TagSel.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = WARNING

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = ERROR

# Number of Events to process
theApp.EvtMax = 500

###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")

###################### Detail time measurement and auditors
# Use auditors
#theApp.AuditAlgorithms=True
#theApp.Dlls += [ "GaudiAud" ]

theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
#topSequence += AthenaPoolCnvSvc()
ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = TRUE

#StoreGateSvc = Service( "StoreGateSvc" )
#StoreGateSvc.Dump = True 
#MessageSvc.OutputLevel = DEBUG

