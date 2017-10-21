#====================================================================
# DAOD_PHYSVAL.py
# This defines DAOD_PHYSVAL, which produces a full set of 
# reconstructed object containers for the purposes of making physics 
# validation plots. In addition it makes a full copy of the AOD
# contents. No skimming, slimming or thinning is run.
# It requires the reductionConf flag PHYSVAL in Reco_tf.py   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
from DerivationFrameworkFlavourTag.HbbCommon import *

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PHYSVALKernel")

#====================================================================
# JET/MET   
#====================================================================

OutputJets["PHYSVAL"] = ["AntiKtVR30Rmax4Rmin02TrackJets",
                         "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                         "AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                         "AntiKt4EMTopoLowPtJets",
                         "AntiKt4LCTopoLowPtJets",
                         "AntiKt4EMPFlowLowPtJets"]

reducedJetList = [ "AntiKt10PV0TrackJets",
                   "AntiKt4TruthWZJets",
                   "AntiKt10TruthJets",
                   "AntiKt10TruthWZJets",
                   "AntiKt10LCTopoJets",
                   "AntiKt2PV0TrackJets",
                   "AntiKt4PV0TrackJets",
                   "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,DerivationFrameworkJob,"PHYSVAL")
updateJVT_xAODColl("AntiKt4EMTopo")
addAntiKt4LowPtJets(DerivationFrameworkJob,"PHYSVAL")

#====================================================================
# FLAVOUR TAGGING   
#====================================================================
# alias for VR
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

addDefaultTrimmedJets(DerivationFrameworkJob,"PHYSVAL",dotruth=True)

#===================================================================
# Variable Radius (VR) Jets 
#===================================================================

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(DerivationFrameworkJob, "AntiKtVR30Rmax4Rmin02Track", "GhostVR30Rmax4Rmin02TrackJet",
          VRJetAlg="AntiKt", VRJetRadius=0.4, VRJetInputs="pv0track", #or should this be lctopo?
          ghostArea = 0 , ptmin = 2000, ptminFilter = 7000,
          variableRMinRadius = 0.02, variableRMassScale = 30000, calibOpt = "none")

FlavorTagInit(JetCollections  = [ 'AntiKt4EMTopoJets','AntiKt4EMPFlowJets'], Sequencer = DerivationFrameworkJob)

#====================================================================
# SET UP STREAM   
#====================================================================

streamName = derivationFlags.WriteDAOD_PHYSVALStream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_PHYSVALStream )
PHYSVALStream = MSMgr.NewPoolRootStream( streamName, fileName )
PHYSVALStream.AcceptAlgs(["PHYSVALKernel"])

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
PHYSVALSlimmingHelper = SlimmingHelper("PHYSVALSlimmingHelper")
PHYSVALSlimmingHelper.AllVariables = [ "Electrons", "ForwardElectrons", "Photons", "Muons", "PrimaryVertices", "InDetTrackParticles",
                                       "AntiKt4EMTopoJets","AntiKt4LCTopoJets", "AntiKt4EMPFlowJets",
                                       "AntiKt10PV0TrackJets", "AntiKt4TruthWZJets", "AntiKt10TruthJets",
                                       "AntiKt10TruthWZJets", "AntiKt10LCTopoJets", "AntiKt2PV0TrackJets",
                                       "AntiKt4PV0TrackJets", "AntiKt4TruthJets",
                                       "AntiKt4EMTopoLowPtJets","AntiKt4LCTopoLowPtJets","AntiKt4EMPFlowLowPtJets",
                                       "JetETMissNeutralParticleFlowObjects", "JetETMissChargedParticleFlowObjects",
                                       "Kt4EMPFlowEventShape","Kt4LCTopoOriginEventShape","Kt4EMTopoOriginEventShape",
                                       "LCOriginTopoClusters","EMOriginTopoClusters","CaloCalTopoClusters",
                                       "BTagging_AntiKt4EMTopoJFVtx",
                                       "BTagging_AntiKt4EMTopo",
                                       "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "BTagging_AntiKt4EMPFlowJFVtx",
                                       "BTagging_AntiKt4EMPFlow",
                                       "BTagging_AntiKt2Track",
                                       "METAssoc_AntiKt4EMTopo","METAssoc_AntiKt4LCTopo","METAssoc_AntiKt4EMPFlow",
                                       "MET_Core_AntiKt4EMTopo","MET_Core_AntiKt4LCTopo","MET_Core_AntiKt4EMPFlow",
                                       "MET_Reference_AntiKt4EMTopo","MET_Reference_AntiKt4LCTopo","MET_Reference_AntiKt4EMPFlow",
                                       "MET_LocHadTopo","MET_LocHadTopoRegions","MET_EMTopo","MET_EMTopoRegions",
                                       "MET_Truth","MET_TruthRegions","MET_Track","MET_Calo",
                                       "TauJets", "TauTracks", 
                                       "TruthParticles", "TruthEvents", "TruthVertices", "MuonTruthParticles", "egammaTruthParticles",
                                       "CombinedMuonTrackParticles",
                                       "ExtrapolatedMuonTrackParticles",
                                       "MuonSpectrometerTrackParticles",
                                       "MSOnlyExtrapolatedMuonTrackParticles",
                                       "InDetForwardTrackParticles"
                                     ]

PHYSVALSlimmingHelper.SmartCollections = [ "Electrons", "Photons", "Muons", "PrimaryVertices", "InDetTrackParticles",
                                           "AntiKt4EMTopoJets","AntiKt4LCTopoJets", "AntiKt4EMPFlowJets",
                                           "BTagging_AntiKt4EMTopo",
                                           "MET_Reference_AntiKt4EMTopo","MET_Reference_AntiKt4LCTopo","MET_Reference_AntiKt4EMPFlow",
                                           "TauJets",
                                           ]

PHYSVALSlimmingHelper.ExtraVariables = [ "BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex",
                                         "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx.-vxTrackAtVertex",
                                         "BTagging_AntiKt4EMPFlowSecVtx.-vxTrackAtVertex",
                                         "CaloCalTopoClusters.rawEta.rawPhi.rawE.rawM.calEta.calPhi.calE.calM.AVG_LAR_Q.AVG_TILE_Q.BADLARQ_FRAC.CENTER_LAMBDA.ENG_BAD_CELLS.ENG_POS.ISOLATION.N_BAD_CELLS.SECOND_R",
                                         "TauNeutralParticleFlowObjects.pt.eta.phi.m.bdtPi0Score.nPi0Proto",
                                         "TauChargedParticleFlowObjects.pt.eta.phi.m.bdtPi0Score",
                                         ]


# Trigger content
PHYSVALSlimmingHelper.IncludeJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeMuonTriggerContent = True
PHYSVALSlimmingHelper.IncludeEGammaTriggerContent = True
PHYSVALSlimmingHelper.IncludeJetTauEtMissTriggerContent = True
PHYSVALSlimmingHelper.IncludeJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeTauTriggerContent = True
PHYSVALSlimmingHelper.IncludeEtMissTriggerContent = True
PHYSVALSlimmingHelper.IncludeBJetTriggerContent = True
PHYSVALSlimmingHelper.IncludeBPhysTriggerContent = True
PHYSVALSlimmingHelper.IncludeMinBiasTriggerContent = True

# Add the jet containers to the stream (defined in JetCommon if import needed)
addJetOutputs(PHYSVALSlimmingHelper,["PHYSVAL"])

#----------------------------------------------------------------------
# NamesAndTypes lookup table for on-the-fly containers
PHYSVALSlimmingHelper.AppendToDictionary = {
  "AntiKt10PV0TrackJets"                       :   "xAOD::JetContainer"        ,
  "AntiKt10PV0TrackJetsAux"                    :   "xAOD::JetAuxContainer"     ,
  "AntiKt4TruthWZJets"                         :   "xAOD::JetContainer"        ,
  "AntiKt4TruthWZJetsAux"                      :   "xAOD::JetAuxContainer"     ,
  "AntiKt10TruthJets"                          :   "xAOD::JetContainer"        ,
  "AntiKt10TruthJetsAux"                       :   "xAOD::JetAuxContainer"     ,
  "AntiKt10TruthWZJets"                        :   "xAOD::JetContainer"        ,
  "AntiKt10TruthWZJetsAux"                     :   "xAOD::JetAuxContainer"     ,
  "AntiKt10LCTopoJets"                         :   "xAOD::JetContainer"        ,
  "AntiKt10LCTopoJetsAux"                      :   "xAOD::JetAuxContainer"     ,
  "AntiKt2PV0TrackJets"                        :   "xAOD::JetContainer"        ,
  "AntiKt2PV0TrackJetsAux"                     :   "xAOD::JetAuxContainer"     ,
  "AntiKt4PV0TrackJets"                        :   "xAOD::JetContainer"        ,
  "AntiKt4PV0TrackJetsAux"                     :   "xAOD::JetAuxContainer"     ,
  "AntiKt4EMTopoLowPtJets"                     :   "xAOD::JetContainer"        ,
  "AntiKt4EMTopoLowPtJetsAux"                  :   "xAOD::JetAuxContainer"     ,
  "AntiKt4LCTopoLowPtJets"                     :   "xAOD::JetContainer"        ,
  "AntiKt4LCTopoLowPtJetsAux"                  :   "xAOD::JetAuxContainer"     ,
  "AntiKt4EMPFlowLowPtJets"                    :   "xAOD::JetContainer"        ,
  "AntiKt4EMPFlowLowPtJetsAux"                 :   "xAOD::JetAuxContainer"     ,
  "AntiKt4TruthJets"                           :   "xAOD::JetContainer"        ,
  "AntiKt4TruthJetsAux"                        :   "xAOD::JetAuxContainer"     ,
  "AntiKtVR30Rmax4Rmin02TrackJets"             :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"          :   "xAOD::JetAuxContainer"     ,
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets"   :   "xAOD::JetContainer"        ,   
  "AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux":   "xAOD::JetAuxContainer"     ,
  "AntiKt10TruthTrimmedPtFrac5SmallR20Jets"    :   "xAOD::JetContainer"        ,
  "AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux" :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx"   :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtxAux":   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx"  :   "xAOD::VertexContainer"     ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtxAux"  :"xAOD::VertexAuxContainer"  ,  
  "BTagging_AntiKtVR30Rmax4Rmin02Track"        :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"     :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt4EMPFlowJFVtx"               :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt4EMPFlowJFVtxAux"            :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt4EMPFlowSecVtx"              :   "xAOD::VertexContainer"     ,
  "BTagging_AntiKt4EMPFlowSecVtxAux"           :   "xAOD::VertexAuxContainer"  ,
  "BTagging_AntiKt4EMPFlow"                    :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt4EMPFlowAux"                 :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt2Track"                      :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt2TrackAux"                   :   "xAOD::BTaggingAuxContainer",
  "LCOriginTopoClusters"                       :   "xAOD::CaloClusterContainer",
  "LCOriginTopoClustersAux"                    :   "xAOD::ShallowAuxContainer",
  "EMOriginTopoClusters"                       :   "xAOD::CaloClusterContainer",
  "EMOriginTopoClustersAux"                    :   "xAOD::ShallowAuxContainer", 
  }
#----------------------------------------------------------------------


# Final construction of output stream
PHYSVALSlimmingHelper.AppendContentToStream(PHYSVALStream)

