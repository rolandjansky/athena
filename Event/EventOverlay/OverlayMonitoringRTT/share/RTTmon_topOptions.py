# Piyali Banerjee UdeM

# get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ToolSvc

from AthenaCommon.AppMgr import ServiceMgr
# Event selector
import AthenaPoolCnvSvc.ReadAthenaPool
#EventSelector.BackNavigation = True

# Particle Properties
from PartPropSvc.PartPropSvcConf import PartPropSvc

# the POOL converters
include( "ParticleBuilderOptions/ESD_PoolCnv_jobOptions.py" )
include( "ParticleBuilderOptions/AOD_PoolCnv_jobOptions.py")
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")
include( "EventAthenaPool/EventAthenaPool_joboptions.py" )

# The RDO/AOD/ESD input file
ServiceMgr.EventSelector.InputCollections = [ "/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000058.pool.root.1","/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000004.pool.root.1","/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000188.pool.root.1","/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000104.pool.root.1","/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000014.pool.root.1","/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000127.pool.root.1","/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000090.pool.root.1","/scratchdisk1/piyali/overlay_data/mc08.105014.J5_pythia_jetjet.digit.RDO.e344_s522_d180_tid078353/RDO.078353._000003.pool.root.1" ]

# Athena-Aware NTuple making Tools
CBNTAthenaAware = True
include ("CBNT_Athena/CBNT_AthenaAware_jobOptions.py")
include ("CBNT_Athena/CBNT_EventInfo_jobOptions.py")

# list of the algorithms to be executed at run time
#this is used in SVN
#this is used in the original codeing
from OverlayMonitoringRTT.OverlayMonitoringRTTConf import rttmon 
topSequence.CBNT_AthenaAware += rttmon( "rttmon",OutputLevel = INFO)
rttmon = rttmon()

rttmon.FileFromRDO          = True    
rttmon.FileFromESD          = False
rttmon.FileFromAOD          = False 
rttmon.FileFromNTPL         = False
rttmon.eventfile            = "RTToutput.dat"
rttmon.PileUpEventInfo      = "OverlayEvent" 
rttmon.OutputLevel          = DEBUG
rttmon.EventInfo            = "MyEvent"
rttmon.JetContainerName     = "Cone4H1TowerJets"
rttmon.MuonContainer        = "MuidMuonCollection"
rttmon.ElectronAODContainer = "ElectronAODCollection"
rttmon.JetEleContainerName  = "JetElements"
rttmon.TriggerTowerContainer = "TriggerTowers"
rttmon.ElectronEtCut        = 10.0*GeV
rttmon.ElectronEtaCut       = 2.5
rttmon.etnbins               = 100;
rttmon.etmin                = 0.0;
rttmon.etmax                = 50.0;
rttmon.jele_Etbins           = 100;
rttmon.jele_Etmin            = 0.0;
rttmon.jele_Etmax            = 25.0;
           
##########################################
# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = [ "AANT DATAFILE='rttmon.aan.root' OPT='RECREATE'" ]
from AnalysisTools.AnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
AANTupleStream = AANTupleStream()
AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
AANTupleStream.OutputName = 'rttmon.aan.root'
AANTupleStream.WriteInputDataHeader = True
AANTupleStream.OutputLevel = DEBUG
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = DEBUG

# Number of Events to process
theApp.EvtMax = -1


###################### For interactive analysis
#include ("PyAnalysisCore/InitPyAnalysisCore.py")

from GaudiSvc.GaudiSvcConf import AuditorSvc
ServiceMgr.AuditorSvc.Auditors  += [ "ChronoAuditor"]

AthenaPoolCnvSvc = Service("AthenaPoolCnvSvc")
AthenaPoolCnvSvc.UseDetailChronoStat = TRUE


