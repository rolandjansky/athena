# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#====================================================================
# Common file used for TOPQ slimming
# Call with:
#     import DerivationFrameworkTop.TOPQCommonSlimming
#     DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQX', TOPQStream)
#====================================================================

#================================
# IMPORTS
#================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
from AthenaCommon.GlobalFlags import globalflags
from DerivationFrameworkTop.TOPQCommonExtraContent import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addOriginCorrectedClusters
from DerivationFrameworkEGamma.ElectronsCPDetailedContent import *
from DerivationFrameworkEGamma.PhotonsCPDetailedContent import *

import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

def setup(TOPQname, stream):
  DFisMC = (globalflags.DataSource()=='geant4')

  TOPQSlimmingHelper = SlimmingHelper(TOPQname + "SlimmingHelper")

  #=====================================================
  # ADD BTagging_AntiKt4EMPFlow COLLECTION TO DICTIONARY
  #=====================================================
  TOPQSlimmingHelper.AppendToDictionary = {
    "BTagging_AntiKt4EMPFlow"                    : "xAOD::BTaggingContainer",
    "BTagging_AntiKt4EMPFlowAux"                 : "xAOD::BTaggingAuxContainer",
    "AntiKtVR30Rmax4Rmin02TrackJets"             : "xAOD::JetContainer"        ,
    "AntiKtVR30Rmax4Rmin02TrackJetsAux"          : "xAOD::JetAuxContainer"     ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track"        : "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"     : "xAOD::BTaggingAuxContainer",
    }

  #================================
  # SMART SLIMMING
  #================================
  TOPQSlimmingHelper.SmartCollections =  []
  TOPQSlimmingHelper.SmartCollections += TOPQSmartSlimmingCollections

  print "TOPQSlimmingHelper.SmartCollections: " , TOPQSlimmingHelper.SmartCollections

  #=================================================================
  # EXTRA VARIABLES FROM DerivationFrameworkTop.TOPQCommonExtraContent
  #=================================================================
  TOPQSlimmingHelper.ExtraVariables = []
  # for jets
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMTopoJets
  # for TOPQDERIV-62
  if TOPQname == 'TOPQ1':
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMTopoJets_ForTOPQ1
  
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMPFlowJets
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKt4EMPFlow
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKt4EMTopo
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets
  # trimmed jets
  if TOPQname == 'TOPQ1':
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets
  # see TOPQDERIV70
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKtVR30Rmax4Rmin02TrackJets
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKtVR30Rmax4Rmin02Track

  # additional variables for electrons/photons objects
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Electrons
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_ForwardElectrons
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Photons
  # additional variables for muons (and associated tracks)
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Muons
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_CombinedMuonTrackParticles
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_ExtrapolatedMuonTrackParticles
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_MuonSpectrometerTrackParticles
  # additional variables for inner detector
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_InDetTrackParticles
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_PrimaryVertices
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_InDetForwardTrackParticles
  # additional variables for tau
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Taus
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TauJets
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TauTracks
  # additional variables for tracks in jets
  #TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt2PV0TrackJets
  TOPQSlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
  TOPQSlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
  # additional variables for clusters
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_egammaClusters;
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_CaloCalTopoClusters;
  # MET
  TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_MET_Track;
  # electron and photon shower information
  TOPQSlimmingHelper.ExtraVariables += ElectronsCPDetailedContent
  TOPQSlimmingHelper.ExtraVariables += PhotonsCPDetailedContent

  # Also write the origin-corrected clusters for both EMTopo (small-R-jets) and
  # LCTopo (large-R-jets).
  addOriginCorrectedClusters(TOPQSlimmingHelper, writeLC=True, writeEM=True)

  if DFisMC:
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Photons_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Electrons_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_ForwardElectrons_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Muons_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_CombinedMuonTrackParticles_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_MuonSpectrometerTrackParticles_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_ExtrapolatedMuonTrackParticles_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_IndetForwardTrackParticles_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Taus_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMTopoJets_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMPFlowJets_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt2PV0TrackJets_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_Truth
    if TOPQname == 'TOPQ1':
      TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets_Truth
    
  # add these trigger variables to all MC and data (TOPQ4 only)
  if DFisMC or TOPQname == 'TOPQ4':
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTag_HLT
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_EFJet
    # there should be a way to automatize this !
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_GSCJet
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_SplitJet
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_a10r_tcemsubjesFS
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_a10r_tcemsubjesISFS
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_a10tclcwsubjesFS
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_a10ttclcwjesFS
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_a4tcemsubjesISFS
    TOPQSlimmingHelper.ExtraVariables +  TOPQExtraVariables_LVL1EnergySumRoI
  # at truth level
  if DFisMC:
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4TruthJets
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4TruthWZJets
    #TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10TruthJets
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TruthEvents  
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TruthParticles  
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TruthVertices
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_MET_Truth
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_MET_TruthRegions

  print "TOPQSlimmingHelper.ExtraVariables: " , TOPQSlimmingHelper.ExtraVariables

  #================================
  # CREATED ON-THE-FLY COLLECTIONS
  #================================
  TOPQSlimmingHelper.StaticContent = []
  TOPQSlimmingHelper.StaticContent += TOPQStaticContent
  
  # for TOPQDERIV-69
  if TOPQname == 'TOPQ1':
    TOPQSlimmingHelper.StaticContent += TOPQStaticContentV0

  if DFisMC:
    TOPQSlimmingHelper.StaticContent += TOPQStaticContentTruth

  print "TOPQSlimmingHelper.StaticContent: " , TOPQSlimmingHelper.StaticContent

  #================================
  # TRIGGER CONTENT
  #================================
  # this saves the trigger objects generated by the trigger
  # that is: if the trigger finds an electron at a specific eta and phi,
  # this will save the trigger electron object
  # the trigger navigation associates this object with a HLT_XXX chain
  # this is eta/phi-matched with the offline electron or muon objects
  # and the trigger navigation is checked to know if the offline object detected
  # corresponds to the same object that is matched to the successful trigger chain
  TOPQSlimmingHelper.IncludeMuonTriggerContent   = True
  TOPQSlimmingHelper.IncludeEGammaTriggerContent = True
  TOPQSlimmingHelper.IncludeTauTriggerContent    = True
  TOPQSlimmingHelper.IncludeJetTriggerContent    = True
  if TOPQname == 'TOPQ4':
    TOPQSlimmingHelper.IncludeBJetTriggerContent   = True

  #====================================
  # ADD AntiKT4EMPFlowJets MET OUTPUT
  #===================================
  # https://gitlab.cern.ch/atlas/athena/blob/21.2/PhysicsAnalysis/DerivationFramework/DerivationFrameworkJetEtMiss/python/METCommon.py#L28    
  # needs to be after where AllVariables[] and StaticContent[] are created
  # adds METAssoc_XXXX, MET_Core_XXXX, and MET_Reference_XXXX
  # uses DerivationFrameworkJetEtMiss.METCommon.py
  addMETOutputs(TOPQSlimmingHelper, [TOPQname], ["AntiKt4EMTopo","AntiKt4EMPFlow"])

  #=======================
  # ADD CUSTOM JET OUTPUT 
  #=======================
  addJetOutputs(TOPQSlimmingHelper,[TOPQname], 
                ["AntiKt4EMTopoJets", # smart list
                 "AntiKt4EMPFlowJets",
                 "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                 "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
                 "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
                 ],
                ["AntiKt3PV0TrackJets",
                 "AntiKt4PV0TrackJets",
                 "AntiKt4LCTopoJets",
                 "AntiKt10LCTopoJets",
                 "AntiKt10LCTopoCSSKJets",
                 "AntiKt10TruthWZJets",
                 ] # veto list
                )

  #================================
  # THIS NEEDS TO BE THE LAST LINE
  #================================
  TOPQSlimmingHelper.AppendContentToStream(stream)
# end setup(TOPQname, stream)
