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

svcMgr += CfgMgr.THistSvc()
svcMgr.THistSvc.Output += ["ANALYSIS DATAFILE='myfile_jfex.root' OPT='RECREATE'"]

from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
xaodStream = MSMgr.NewPoolRootStream( "StreamXAOD", "xAOD.jFEX.output.root" )

# the jFex containers
xaodStream.AddItem( ["xAOD::jFexSRJetRoIContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexSRJetRoIAuxContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexLRJetRoIContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexLRJetRoIAuxContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexTauRoIContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexTauRoIAuxContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexSumETRoIContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexSumETRoIAuxContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexMETRoIContainer#*"] )
xaodStream.AddItem( ["xAOD::jFexMETRoIAuxContainer#*"] )


# Generic event info
xaodStream.AddItem( ["xAOD::EventInfo#*"] )
xaodStream.AddItem( ["xAOD::EventAuxInfo#*"])
xaodStream.AddItem( ["xAOD::EventShape#Kt4EMTopoOriginEventShape"])
xaodStream.AddItem( ["xAOD::EventShapeAuxInfo#Kt4EMTopoOriginEventShapeAux."])


#Physics Objects
xaodStream.AddItem( ["xAOD::JetContainer#AntiKt4EMTopoJets"])
xaodStream.AddItem( ["xAOD::JetAuxContainer#AntiKt4EMTopoJetsAux."])
xaodStream.AddItem( ["xAOD::MuonContainer#Muons"])
xaodStream.AddItem( ["xAOD::MuonAuxContainer#MuonsAux."])
xaodStream.AddItem( ["xAOD::ElectronContainer#Electrons"])
xaodStream.AddItem( ["xAOD::ElectronAuxContainer#ElectronsAux."])
xaodStream.AddItem( ["xAOD::PhotonContainer#Photons"])
xaodStream.AddItem( ["xAOD::PhotonAuxContainer#PhotonsAux."])
xaodStream.AddItem( ["xAOD::TauJetContainer#TauJets"])
xaodStream.AddItem( ["xAOD::TauJetAuxContainer#TauJetsAux.-VertexedClusters."])
xaodStream.AddItem( ["xAOD::MissingETContainer#MET_Reference_AntiKt4EMTopo"])
xaodStream.AddItem( ["xAOD::MissingETAuxContainer#MET_Reference_AntiKt4EMTopoAux.-ConstitObjectLinks.-ConstitObjectWeights"])
xaodStream.AddItem( ["xAOD::VertexContainer#PrimaryVertices"])
xaodStream.AddItem( ["xAOD::VertexAuxContainer#PrimaryVerticesAux.-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"])
xaodStream.AddItem( ["xAOD::MissingETAssociationMap#METAssoc_AntiKt4EMTopo"])
xaodStream.AddItem( ["xAOD::MissingETAuxAssociationMap#METAssoc_AntiKt4EMTopoAux."])
xaodStream.AddItem( ["xAOD::MissingETContainer#MET_Core_AntiKt4EMTopo"])
xaodStream.AddItem( ["xAOD::MissingETAuxContainer#MET_Core_AntiKt4EMTopoAux.name.mpx.mpy.sumet.source"])
xaodStream.AddItem( ["xAOD::MissingETContainer#MET_Track"])
xaodStream.AddItem( ["xAOD::MissingETAuxContainer#MET_TrackAux.name.mpx.mpy"])



xaodStream.AddItem( ["xAOD::MissingETComponentMap#METMap_Truth"])
xaodStream.AddItem( ["xAOD::EmTauRoIContainer#LVL1EmTauRoIs"])
xaodStream.AddItem( ["xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux."])
xaodStream.AddItem( ["xAOD::EnergySumRoI#LVL1EnergySumRoI"])
xaodStream.AddItem( ["xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux."])
xaodStream.AddItem( ["xAOD::JetRoIContainer#LVL1JetRoIs"])
xaodStream.AddItem( ["xAOD::JetRoIAuxContainer#LVL1JetRoIsAux."])


xaodStream.AddItem( ["xAOD::TruthParticleContainer#*"])
xaodStream.AddItem( ["xAOD::TruthParticleAuxContainer#TruthParticlesAux.-caloExtension"])
xaodStream.AddItem( ["xAOD::TruthVertexContainer#*"])
xaodStream.AddItem( ["xAOD::TruthVertexAuxContainer#*"])
xaodStream.AddItem( ["xAOD::TruthEventContainer#*"])
xaodStream.AddItem( ["xAOD::TruthEventAuxContainer#*"])

xaodStream.AddItem( ["TileCellContainer#MBTSContainer"])
xaodStream.AddItem( ["xAOD::TrigDecision#xTrigDecision"])
xaodStream.AddItem( ["xAOD::TrigNavigation#TrigNavigation"])
xaodStream.AddItem( ["xAOD::TrigConfKeys#TrigConfKeys"])
xaodStream.AddItem( ["HLT::HLTResult#HLTResult_HLT"])
xaodStream.AddItem( ["xAOD::TrigDecisionAuxInfo#xTrigDecisionAux."])
xaodStream.AddItem( ["xAOD::TrigNavigationAuxInfo#TrigNavigationAux."])









#######################################################
log.info("==========================================================")
log.info("Scheduling jFEXDriver")
athAlgSeq += CfgMgr.LVL1__jFEXDriver('MyjFEXDriver')
#athAlgSeq += CfgMgr.LVL1__jFEXNtupleWriter('MyjFEXNtupleWriter')
log.info("==========================================================")
#######################################################
