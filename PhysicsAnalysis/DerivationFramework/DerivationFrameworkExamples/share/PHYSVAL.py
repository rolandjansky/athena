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

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("PHYSVALKernel")

#====================================================================
# JET/MET   
#====================================================================

OutputJets["PHYSVAL"] = ["AntiKtVR30Rmax4Rmin02TrackJets"]

#from JetRecTools.JetRecToolsConfig import ctm
#clustLCOrigSeq = ctm.buildConstitModifSequence( "JetConstitSeq_LCOrigin",
#                                                OutputContainer='LCOriginTopoClusters',
#                                                InputContainer= 'CaloCalTopoClusters',
#                                                modList = [  'lc_origin' ] )
#
#clustEMOrigSeq = ctm.buildConstitModifSequence( "JetConstitSeq_EMOrigin",
#                                                OutputContainer='EMOriginTopoClusters',
#                                                InputContainer= 'CaloCalTopoClusters',
#                                                modList = [  'em_origin' ] )
#
#from JetRec.JetRecConf import JetAlgorithm
#clustSeqAlg = JetAlgorithm("ClusterModifiers", Tools = [clustLCOrigSeq,clustEMOrigSeq])
#DerivationFrameworkJob += clustSeqAlg

reducedJetList = [ "AntiKt10PV0TrackJets",
                   "AntiKt4TruthWZJets",
                   "AntiKt10TruthJets",
                   "AntiKt10TruthWZJets",
                   "AntiKt10LCTopoJets"
                   "AntiKt2PV0TrackJets",
                   "AntiKt4PV0TrackJets",
                   "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,DerivationFrameworkJob,"PHYSVAL")
updateJVT_xAODColl("AntiKt4EMTopo")

#====================================================================
# FLAVOUR TAGGING   
#====================================================================

FlavorTagInit(isFTAG1 = True, JetCollections  = [ 'AntiKt4EMTopoJets',
                                                  'AntiKtVR30Rmax4Rmin02TrackJets',
                                                  'AntiKt4EMPFlowJets'], Sequencer = DerivationFrameworkJob)

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
PHYSVALSlimmingHelper.AllVariables = [ "Electrons", "Photons", "Muons", "PrimaryVertices", "InDetTrackParticles",
                                       "AntiKt4EMTopoJets","AntiKt4LCTopoJets", "AntiKt4EMPFlowJets",
                                       "AntiKt10PV0TrackJets", "AntiKt4TruthWZJets", "AntiKt10TruthJets",
                                       "AntiKt10TruthWZJets", "AntiKt10LCTopoJets", "AntiKt2PV0TrackJets",
                                       "AntiKt4PV0TrackJets", "AntiKt4TruthJets",
                                       "Kt4EMPFlowEventShape","Kt4LCTopoOriginEventShape","Kt4EMTopoOriginEventShape",
                                       "LCOriginTopoClusters","EMOriginTopoClusters",
                                       "BTagging_AntiKt4EMTopoJFVtx",
#                                       "BTagging_AntiKt4EMTopoSecVtx",
                                       "BTagging_AntiKt4EMTopo",
                                       "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx",
#                                       "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "BTagging_AntiKt4EMPFlowJFVtx",
#                                       "BTagging_AntiKt4EMPFlowSecVtx",
                                       "BTagging_AntiKt4EMPFlow",
                                       "TruthParticles", "TruthEvents", "TruthVertices"
                                     ]

#PHYSVALSlimmingHelper.ExtraVariables = [ "BTagging_AntiKt4EMTopoSecVtx.-vxTrackAtVertex",
#                                         "BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx.-vxTrackAtVertex",
#                                         "BTagging_AntiKt4EMPFlowSecVtx.-vxTrackAtVertex" ]


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

#----------------------------------------------------------------------
# NamesAndTypes lookup table for on-the-fly containers
PHYSVALSlimmingHelper.AppendToDictionary = {
  "AntiKtVR30Rmax4Rmin02TrackJets"             :   "xAOD::JetContainer"        ,
  "AntiKtVR30Rmax4Rmin02TrackJetsAux"          :   "xAOD::JetAuxContainer"     ,
  "BTagging_AntiKt4EMTopoJFVtx"                :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt4EMTopoJFVtxAux"             :   "xAOD::BTaggingAuxContainer",
  #"BTagging_AntiKt4EMTopoSecVtx"               :   "xAOD::VertexContainer"     ,
  #"BTagging_AntiKt4EMTopoSecVtxAux"            :   "xAOD::VertexAuxContainer"  ,
  "BTagging_AntiKt4EMTopo"                     :   "xAOD::VertexContainer"     ,
  "BTagging_AntiKt4EMTopoAux"                  :   "xAOD::VertexAuxContainer"  ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtx"   :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackJFVtxAux":   "xAOD::BTaggingAuxContainer",
  #"BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtx"  :   "xAOD::VertexContainer"     ,
  #"BTagging_AntiKtVR30Rmax4Rmin02TrackSecVtxAux"  :"xAOD::VertexAuxContainer"  ,  
  "BTagging_AntiKtVR30Rmax4Rmin02Track"        :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"     :   "xAOD::BTaggingAuxContainer",
  "BTagging_AntiKt4EMPFlowJFVtx"               :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt4EMPFlowJFVtxAux"            :   "xAOD::BTaggingAuxContainer",
  #"BTagging_AntiKt4EMPFlowSecVtx"              :   "xAOD::VertexContainer"     ,
  #"BTagging_AntiKt4EMPFlowSecVtxAux"           :   "xAOD::VertexAuxContainer"  ,
  "BTagging_AntiKt4EMPFlow"                    :   "xAOD::BTaggingContainer"   ,
  "BTagging_AntiKt4EMPFlowAux"                 :   "xAOD::BTaggingAuxContainer",
  }
#----------------------------------------------------------------------


# Final construction of output stream
PHYSVALSlimmingHelper.AppendContentToStream(PHYSVALStream)

