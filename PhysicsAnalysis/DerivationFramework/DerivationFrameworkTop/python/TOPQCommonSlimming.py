# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig

def setup(TOPQname, stream):
    DFisMC = (globalflags.DataSource()=='geant4')

    TOPQSlimmingHelper = SlimmingHelper(TOPQname + "SlimmingHelper")

    #=====================================================
    # ADD BTagging_AntiKt4EMPFlow COLLECTION TO DICTIONARY
    #=====================================================
    TOPQSlimmingHelper.AppendToDictionary = {
        "BTagging_AntiKt4EMPFlow"                    : "xAOD::BTaggingContainer",
        "BTagging_AntiKt4EMPFlowAux"                 : "xAOD::BTaggingAuxContainer",
        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810"             : "xAOD::JetContainer"        ,
        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201810Aux"          : "xAOD::JetAuxContainer"     ,
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810"        : "xAOD::BTaggingContainer"   ,
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810Aux"     : "xAOD::BTaggingAuxContainer",
        # Include new retrained b-tagging with tag 201903
        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"             : "xAOD::JetContainer"        ,
        "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903Aux"          : "xAOD::JetAuxContainer"     ,
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903"        : "xAOD::BTaggingContainer"   ,
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903Aux"     : "xAOD::BTaggingAuxContainer",
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtx"     : "xAOD::BTagVertexContainer"   ,
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903JFVtxAux"  : "xAOD::BTagVertexAuxContainer",
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtx"    : "xAOD::VertexContainer"   ,
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903SecVtxAux" : "xAOD::VertexAuxContainer",
        "AntiKt8EMPFlowJets"                                :   "xAOD::JetContainer"        ,
        "AntiKt8EMPFlowJetsAux"                             :   "xAOD::JetAuxContainer"     ,
        "AntiKt8EMPFlowExKt2GASubJets"                      :   "xAOD::JetContainer"        ,
        "AntiKt8EMPFlowExKt2GASubJetsAux"                   :   "xAOD::JetAuxContainer"     ,

    }

    # list of existing containers with a smart list is available here :
    # DerivationFrameworkCore/python/FullListOfSmartContainers.py
    TOPQSlimmingHelper.SmartCollections =  [
        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
        "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
        "AntiKt10UFOCSSKJets",
        "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets",
        "AntiKt4EMPFlowJets",
        "AntiKt4EMPFlowJets_BTagging201810",
        "AntiKt4EMPFlowJets_BTagging201903",
        "AntiKt4EMTopoJets",
        "AntiKt4EMTopoJets_BTagging201810",        
        "AntiKt4TruthDressedWZJets",
        "AntiKt4TruthJets",
        "AntiKt4TruthWZJets",
        "BTagging_AntiKt4EMPFlow_201810",
        "BTagging_AntiKt4EMPFlow_201903",
        "BTagging_AntiKt4EMTopo_201810",
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201810",
        "BTagging_AntiKtVR30Rmax4Rmin02Track_201903",
        "Electrons",
        "HLT_xAOD__JetContainer_a4tcemsubjesFS",
        "HLT_xAOD__TrigMissingETContainer_TrigEFMissingET",
        "InDetTrackParticles",
        "MET_Reference_AntiKt4EMPFlow",
        "MET_Reference_AntiKt4EMTopo",
        "Muons",
        "Photons",
        "PrimaryVertices",
        "TauJets",        
    ]

    print "TOPQSlimmingHelper.SmartCollections: " , TOPQSlimmingHelper.SmartCollections

    #=================================================================
    # EXTRA VARIABLES FROM DerivationFrameworkTop.TOPQCommonExtraContent
    #=================================================================
    TOPQSlimmingHelper.ExtraVariables = []
    # for jets
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMTopoJets
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMPFlowJets
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKt4EMPFlow_201810
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKt4EMPFlow_201903
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKt4EMTopo_201810
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets

    # additional variables for electrons/photons objects
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Electrons
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_ForwardElectrons
    # additional variables for muons (and associated tracks)
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Muons
    TOPQSlimmingHelper.AllVariables += ["CombinedMuonTrackParticles"]
    TOPQSlimmingHelper.AllVariables += ["ExtrapolatedMuonTrackParticles"]
    TOPQSlimmingHelper.AllVariables += ["MuonSpectrometerTrackParticles"]
    # additional variables for inner detector
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_InDetTrackParticles
    TOPQSlimmingHelper.AllVariables += ["InDetForwardTrackParticles"]
    # additional variables for tau
    TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TauTracks
    # additional variables for tracks in jets
    #TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt2PV0TrackJets
    TOPQSlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptVariablesForDxAOD()
    TOPQSlimmingHelper.ExtraVariables += JetTagConfig.GetExtraPromptTauVariablesForDxAOD()
    TOPQSlimmingHelper.ExtraVariables += LepTagConfig.GetExtraImprovedPromptVariablesForDxAOD()
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
        # Additional truth variables for existing collections
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_ForwardElectrons_Truth
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Muons_Truth
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_Taus_Truth
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMTopoJets_Truth
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMPFlowJets_Truth
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_Truth
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4TruthJets
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4TruthWZJets
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4TruthDressedWZJets
        # Additional variables for truth-only collections
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TruthEvents
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TruthParticles
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_TruthVertices
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_MET_TruthRegions
        # Additional trigger information for MC
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTag_HLT
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_EFJet
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_SplitJet

    if TOPQname == 'TOPQ1':
        # for TOPQDERIV-62 and TOPQDERIV-98
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt4EMPFlowJets_ForTOPQ1
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt8EMPFlowJets_ForTOPQ1
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt8EMPFlowExKt2GASubJets_ForTOPQ1
        # see TOPQDERIV70
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKtVR30Rmax4Rmin02TrackJets
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKtVR30Rmax4Rmin02Track
        # see TOPDERIV-93
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKtVR30Rmax4Rmin02TrackJets_201903
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_SecVtx_AntiKtVR30Rmax4Rmin02TrackJets_201903
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKtVR30Rmax4Rmin02Track_201903
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoJets
        # Xbb tagger extra variables
        from DerivationFrameworkFlavourTag.HbbCommon import xbbTaggerExtraVariables
        TOPQSlimmingHelper.ExtraVariables += xbbTaggerExtraVariables
    elif TOPQname == 'TOPQ4':
        # see TOPQDERIV70
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKtVR30Rmax4Rmin02TrackJets
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKtVR30Rmax4Rmin02Track
        # see TOPDERIV-93
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKtVR30Rmax4Rmin02TrackJets_201903
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_AntiKtVR30Rmax4Rmin02Track_201903
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTagging_SecVtx_AntiKtVR30Rmax4Rmin02TrackJets_201903
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_AntiKt10LCTopoJets
        # add these trigger variables to both data and MC
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_BTag_HLT
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_EFJet
        TOPQSlimmingHelper.ExtraVariables += TOPQExtraVariables_HLT_SplitJet

    print "TOPQSlimmingHelper.ExtraVariables: " , TOPQSlimmingHelper.ExtraVariables

    #================================
    # CREATED ON-THE-FLY COLLECTIONS
    #================================
    TOPQSlimmingHelper.StaticContent = [
        'xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets',
        'xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.',
        'xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets',
        'xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.',
        'xAOD::JetContainer#AntiKt10TruthSoftDropBeta100Zcut10Jets',
        'xAOD::JetAuxContainer#AntiKt10TruthSoftDropBeta100Zcut10JetsAux.',
    ]

    if TOPQname == 'TOPQ1':
        # for TOPQDERIV-69
        TOPQSlimmingHelper.StaticContent += [
            'xAOD::VertexContainer#TOPQ1RecoV0Candidates',
            'xAOD::VertexAuxContainer#TOPQ1RecoV0CandidatesAux'
                + '.-vxTrackAtVertex'
                + '.-vertexType'
                + '.-neutralParticleLinks'
                + '.-neutralWeights'
                + '.-KshortLink'
                + '.-LambdaLink'
                + '.-LambdabarLink'
                + '.-gamma_fit'
                + '.-gamma_mass'
                + '.-gamma_massError'
                + '.-gamma_probability',
        ]
    elif TOPQname == 'TOPQ5':
        TOPQSlimmingHelper.StaticContent += [
            'xAOD::VertexContainer#TOPQ5RefittedPrimaryVertices',
            'xAOD::VertexAuxContainer#TOPQ5RefittedPrimaryVerticesAux.',
        ]
    elif TOPQname == 'TOPQ6':
        # for TOPQDERIV-69
        TOPQSlimmingHelper.StaticContent += [
            'xAOD::VertexContainer#TOPQ6RecoV0Candidates',
            'xAOD::VertexAuxContainer#TOPQ6RecoV0CandidatesAux'
                + '.-vxTrackAtVertex'
                + '.-vertexType'
                + '.-neutralParticleLinks'
                + '.-neutralWeights'
                + '.-KshortLink'
                + '.-LambdaLink'
                + '.-LambdabarLink'
                + '.-gamma_fit'
                + '.-gamma_mass'
                + '.-gamma_massError'
                + '.-gamma_probability',
        ]

    if DFisMC:
        TOPQSlimmingHelper.StaticContent += [
            'xAOD::TruthParticleContainer#TruthElectrons',
            'xAOD::TruthParticleAuxContainer#TruthElectronsAux.',
            'xAOD::TruthParticleContainer#TruthMuons',
            'xAOD::TruthParticleAuxContainer#TruthMuonsAux.',
            'xAOD::TruthParticleContainer#TruthPhotons',
            'xAOD::TruthParticleAuxContainer#TruthPhotonsAux.',
            'xAOD::TruthParticleContainer#TruthTaus',
            'xAOD::TruthParticleAuxContainer#TruthTausAux.',
            'xAOD::TruthParticleContainer#TruthNeutrinos',
            'xAOD::TruthParticleAuxContainer#TruthNeutrinosAux.',
        ]

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
                  # Add the following smart jet collections
                  ["AntiKt4EMTopoJets",
                   "AntiKt4EMPFlowJets",
                   "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                   "AntiKt10TrackCaloClusterTrimmedPtFrac5SmallR20Jets",
                   "AntiKt10UFOCSSKJets",
                   "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets", 
                 ],
                  # Veto the following jet containers in the output
                  ["AntiKt3PV0TrackJets",
                   "AntiKt4PV0TrackJets",
                   "AntiKt4LCTopoJets",
                   "AntiKt10TrackCaloClusterJets",
                   "AntiKt10LCTopoJets",
                   "AntiKt10LCTopoCSSKJets",
                   "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
                   "AntiKt10TruthWZJets",
                  ])

    #================================
    # THIS NEEDS TO BE THE LAST LINE
    #================================
    TOPQSlimmingHelper.AppendContentToStream(stream)
# end setup(TOPQname, stream)
