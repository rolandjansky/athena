# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#====================================================================
# DAOD_FTAG2.py
# This defines DAOD_FTAG2, an unskimmed DAOD format for Run 3.
# It contains the variables and objects needed for
# 2-lepton based ftag calibration.
# It requires the reductionConf flag FTAG2 in Reco_tf.py   
#====================================================================

from AthenaCommon import Logging
nanolog = Logging.logging.getLogger('FTAG2')

from DerivationFrameworkCore.DerivationFrameworkMaster import buildFileName, DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkPhys import PhysCommon
from DerivationFrameworkPhys import PhysCommonTrigger
from DerivationFrameworkEGamma import ElectronsCPDetailedContent
from DerivationFrameworkJetEtMiss import METCommon
from DerivationFrameworkJetEtMiss.METCommon import scheduleMETAssocAlg
from DerivationFrameworkCore import LHE3WeightMetadata
from DerivationFrameworkFlavourTag import FtagCommon
import re


#====================================================================
# Set up sequence for this format and add to the top sequence 
#====================================================================
FTAG2Seq = CfgMgr.AthSequencer("FTAG2Seq")
DerivationFrameworkJob += FTAG2Seq

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_FTAG2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG2Stream )
FTAG2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
FTAG2Stream.AcceptAlgs(["FTAG2Kernel"])

### Thinning and augmentation tools lists
thinningTools       = []
AugmentationTools   = []

# Special sequence 
FTAG2Seq = CfgMgr.AthSequencer("FTAG2Seq")

#====================================================================
# SKIMMING TOOLS
# (SKIMMING = REMOVING WHOLE EVENTS THAT FAIL CRITERIA)
# Create skimming tool, and create + add kernel to sequence
#====================================================================
lepton_skimming_expression = 'count( (Muons.pt > 18*GeV) && (0 == Muons.muonType || 1 == Muons.muonType || 4 == Muons.muonType) ) + count(( Electrons.pt > 18*GeV) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))) >= 2 && count( (Muons.pt > 25*GeV) && (0 == Muons.muonType || 1 == Muons.muonType || 4 == Muons.muonType) ) + count(( Electrons.pt > 25*GeV) && ((Electrons.Loose) || (Electrons.DFCommonElectronsLHLoose))) >= 1'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
FTAG2StringSkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "FTAG2StringSkimmingTool",
                                  expression = lepton_skimming_expression 
                                  )

ToolSvc += FTAG2StringSkimmingTool
FTAG2Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG2SkimKernel",
        SkimmingTools = [FTAG2StringSkimmingTool],
        )


# TrackParticles associated with small-R jets
track_selection_string = "InDetTrackParticles.numberOfSCTHits + InDetTrackParticles.numberOfPixelHits > 1"

from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__JetTrackParticleThinning
FTAG2AktVRJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "FTAG2AktVRJetTPThinningTool",
        StreamName              = FTAG2Stream.Name,
        JetKey                  = "AntiKtVR30Rmax4Rmin02PV0TrackJets",
        SelectionString         = "AntiKtVR30Rmax4Rmin02PV0TrackJets.pt > 7*GeV",
        TrackSelectionString    = track_selection_string,
        InDetTrackParticlesKey  = "InDetTrackParticles",
        )
ToolSvc += FTAG2AktVRJetTPThinningTool
thinningTools.append(FTAG2AktVRJetTPThinningTool)

FTAG2Akt4PFlowJetTPThinningTool = DerivationFramework__JetTrackParticleThinning( name          = "FTAG2Akt4PFlowJetTPThinningTool",
        StreamName              = FTAG2Stream.Name,
        JetKey                  = "AntiKt4EMPFlowJets",
        SelectionString         = "AntiKt4EMPFlowJets.pt > 15*GeV",
        TrackSelectionString    = track_selection_string,
        InDetTrackParticlesKey  = "InDetTrackParticles",
        )
ToolSvc += FTAG2Akt4PFlowJetTPThinningTool
thinningTools.append(FTAG2Akt4PFlowJetTPThinningTool)

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
FTAG2Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG2Kernel",
        ThinningTools = thinningTools)


#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
FTAG2SlimmingHelper = SlimmingHelper("FTAG2SlimmingHelper")

FTAG2SlimmingHelper.SmartCollections = ["Electrons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                      ]

FTAG2SlimmingHelper.AllVariables = [   
                                       "EventInfo",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "BTagging_AntiKt4EMPFlow",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "BTagging_AntiKt4EMPFlowJFVtx",
                                       "BTagging_AntiKt4EMPFlowSecVtx",
                                       "TruthBottom", "TruthElectrons","TruthMuons","TruthTaus",
                                       ]

# Static content
FtagCommon.add_static_content_to_SlimmingHelper(FTAG2SlimmingHelper)

# Trigger content
FtagCommon.trigger_setup(FTAG2SlimmingHelper,"FTAG2")

# Add truth containers
FtagCommon.add_truth_to_SlimmingHelper(FTAG2SlimmingHelper)

# Add ExtraVariables
FtagCommon.add_ExtraVariables_to_SlimmingHelper(FTAG2SlimmingHelper)

# Add trigger matching
PhysCommonTrigger.trigmatching_helper_notau.add_to_slimming(FTAG2SlimmingHelper)
PhysCommonTrigger.trigmatching_helper_tau.add_to_slimming(FTAG2SlimmingHelper)

# Final construction of output stream
FTAG2SlimmingHelper.AppendContentToStream(FTAG2Stream)

