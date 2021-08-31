#jps.AthenaCommonFlags.AccessMode = "POOLAccess" # use POOL read mode because reading calocells
#svcMgr.EventSelector.InputCollections = jps.AthenaCommonFlags.FilesInput()
from AthenaCommon.GlobalFlags  import globalflags
import AthenaPoolCnvSvc.ReadAthenaPool

if type(theApp).__name__ == "fakeAppMgr": theApp.initialize() #this line cuts off pathena when joboption parsing ... since all outputs now declared

#from RecExConfig import AutoConfiguration
#AutoConfiguration.ConfigureSimulationOrRealData()
#AutoConfiguration.ConfigureGeo()
#AutoConfiguration.ConfigureConditionsTag()
#from AthenaCommon.DetFlags import DetFlags
#DetFlags.detdescr.all_setOff()
#DetFlags.detdescr.Calo_setOn()
#include("RecExCond/AllDet_detDescr.py")

#include( "CaloConditions/CaloConditions_jobOptions.py" )
include( "LArDetDescr/LArDetDescr_joboptions.py" )


include( "RegistrationServices/IOVRegistrationSvc_jobOptions.py" )

IOVBeginRun   = IOVRunNumberMin
IOVEndRun     = IOVRunNumberMax
IOVBeginLB = IOVLBNumberMin
IOVEndLB   = IOVLBNumberMax

import RegistrationServices.IOVRegistrationSvc
regSvc = svcMgr.IOVRegistrationSvc


#svcMgr.IOVDbSvc.DBInstance=""

if "GlobalTag" not in dir():
    GlobalTag     = 'OFLCOND-CSC-00-01-00' #Sadly event his doesn't work for the 14 TeV jetjet sample 'OFLCOND-MC15c-SDR-14-02' #This works for 13 TeV 'OFLCOND-CSC-00-01-00'    #No idea what this is: COMCOND-BLKPST-004-05'

#svcMgr.IOVDbSvc.GlobalTag = GlobalTag

svcMgr.IOVDbSvc.GlobalTag=globalflags.ConditionsTag()

# configure detector description from metadata in input file
from RecExConfig import AutoConfiguration
AutoConfiguration.ConfigureSimulationOrRealData()
AutoConfiguration.ConfigureGeo()
AutoConfiguration.ConfigureConditionsTag()
from AthenaCommon.DetFlags import DetFlags
DetFlags.detdescr.all_setOff()
DetFlags.detdescr.Calo_setOn()
include("RecExCond/AllDet_detDescr.py")

# menu with default configuration for testing
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from TrigConfigSvc.TrigConfigSvcCfg import L1ConfigSvcCfg
CAtoGlobalWrapper(L1ConfigSvcCfg,ConfigFlags)

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["ANALYSIS DATAFILE='myfile_jfex.root' OPT='RECREATE'"]


from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamAOD_Augmented = MSMgr.NewPoolRootStream( "StreamAOD", "xAOD.jFEX.output.root" )
StreamAOD = StreamAOD_Augmented.GetEventStream()

# the jFex containers
StreamAOD.ItemList+=["xAOD::jFexSRJetRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexSRJetRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexLRJetRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexLRJetRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexTauRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexTauRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexSumETRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexSumETRoIAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexMETRoIContainer#*"]
StreamAOD.ItemList+=["xAOD::jFexMETRoIAuxContainer#*"]


# a random selection of containers/keys from parsing checkSG output from original MC, ignoring Aux and removing those
# that seemed to give a crash
# Generic event info
StreamAOD.ItemList+=["xAOD::EventInfo#*"]
StreamAOD.ItemList+=["xAOD::EventAuxInfo#*"]
StreamAOD.ItemList+=["xAOD::EventShape#Kt4EMTopoOriginEventShape"]
StreamAOD.ItemList+=["xAOD::EventShapeAuxInfo#Kt4EMTopoOriginEventShapeAux."]

#Physics Objects
StreamAOD.ItemList+=["xAOD::JetContainer#AntiKt4EMTopoJets"]
StreamAOD.ItemList+=["xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux."]
StreamAOD.ItemList+=["xAOD::MuonContainer#Muons"]
StreamAOD.ItemList+=["xAOD::MuonAuxContainer#MuonsAux."]
StreamAOD.ItemList+=["xAOD::ElectronContainer#Electrons"]
StreamAOD.ItemList+=["xAOD::ElectronAuxContainer#ElectronsAux."]
StreamAOD.ItemList+=["xAOD::PhotonContainer#Photons"]
StreamAOD.ItemList+=["xAOD::PhotonAuxContainer#PhotonsAux."]
StreamAOD.ItemList+=["xAOD::TauJetContainer#TauJets"]
StreamAOD.ItemList+=["xAOD::TauJetAuxContainer#TauJetsAux.-VertexedClusters."]
StreamAOD.ItemList+=["xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo"]
StreamAOD.ItemList+=["xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.-ConstitObjectLinks.-ConstitObjectWeights"]
StreamAOD.ItemList+=["xAOD::VertexContainer#PrimaryVertices"]
StreamAOD.ItemList+=["xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"]
StreamAOD.ItemList+=["xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMTopo"]
StreamAOD.ItemList+=["xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMTopoAux."]
StreamAOD.ItemList+=["xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo"]
StreamAOD.ItemList+=["xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.name.mpx.mpy.sumet.source"]
StreamAOD.ItemList+=["xAOD::MissingETContainer#MET_Track"]
StreamAOD.ItemList+=["xAOD::MissingETAuxContainer#MET_TrackAux.name.mpx.mpy"]



StreamAOD.ItemList+=["xAOD::MissingETComponentMap#METMap_Truth"]
StreamAOD.ItemList+=["xAOD::EmTauRoIContainer#LVL1EmTauRoIs"]
StreamAOD.ItemList+=["xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux."]
StreamAOD.ItemList+=["xAOD::EnergySumRoI#LVL1EnergySumRoI"]
StreamAOD.ItemList+=["xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux."]
StreamAOD.ItemList+=["xAOD::JetRoIContainer#LVL1JetRoIs"]
StreamAOD.ItemList+=["xAOD::JetRoIAuxContainer#LVL1JetRoIsAux."]


StreamAOD.ItemList+=["xAOD::TruthParticleContainer#*"]
StreamAOD.ItemList+=["xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension"]
StreamAOD.ItemList+=["xAOD::TruthVertexContainer#*"]
StreamAOD.ItemList+=["xAOD::TruthVertexAuxContainer#*"]
StreamAOD.ItemList+=["xAOD::TruthEventContainer#*"]
StreamAOD.ItemList+=["xAOD::TruthEventAuxContainer#*"]

StreamAOD.ItemList+=["TileCellContainer#MBTSContainer"]
StreamAOD.ItemList+=["xAOD::TrigDecision#xTrigDecision"]
StreamAOD.ItemList+=["xAOD::TrigNavigation#TrigNavigation"]
StreamAOD.ItemList+=["xAOD::TrigConfKeys#TrigConfKeys"]
StreamAOD.ItemList+=["HLT::HLTResult#HLTResult_HLT"]
StreamAOD.ItemList+=["xAOD::TrigDecisionAuxInfo#xTrigDecisionAux."]
StreamAOD.ItemList+=["xAOD::TrigNavigationAuxInfo#TrigNavigationAux."]







#######################################################
log.info("==========================================================")
log.info("Scheduling jFEXDriver")
athAlgSeq += CfgMgr.LVL1__jFEXDriver('MyjFEXDriver')
#athAlgSeq += CfgMgr.LVL1__jFEXNtupleWriter('MyjFEXNtupleWriter')
log.info("==========================================================")
#######################################################
