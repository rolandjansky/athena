# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#====================================================================
# DAOD_FTAG1.py
# This defines DAOD_FTAG1, an unskimmed DAOD format for Run 3.
# It contains the variables and objects needed for 
# ftag algorithm development.
# It requires the reductionConf flag FTAG1 in Reco_tf.py   
#====================================================================

from AthenaCommon import Logging
nanolog = Logging.logging.getLogger('FTAG1')

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
FTAG1Seq = CfgMgr.AthSequencer("FTAG1Seq")
DerivationFrameworkJob += FTAG1Seq

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_FTAG1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_FTAG1Stream )
FTAG1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
FTAG1Stream.AcceptAlgs(["FTAG1Kernel"])

### Thinning and augmentation tools lists
thinningTools       = []
AugmentationTools   = []

# Special sequence 
FTAG1Seq = CfgMgr.AthSequencer("FTAG1Seq")


#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#====================================================================
# Add the kernel for thinning (requires the objects be defined)
FTAG1Seq += CfgMgr.DerivationFramework__DerivationKernel("FTAG1Kernel",
                                                                ThinningTools = thinningTools)


#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
FTAG1SlimmingHelper = SlimmingHelper("FTAG1SlimmingHelper")

FTAG1SlimmingHelper.SmartCollections = ["Electrons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMPFlow",
                                       "AntiKtVR30Rmax4Rmin02PV0TrackJets",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "MET_Baseline_AntiKt4EMPFlow",
                                      ]

FTAG1SlimmingHelper.AllVariables = [
                                       "EventInfo",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "InDetLargeD0TrackParticles",
                                       "BTagging_AntiKt4EMPFlow",
                                       "BTagging_AntiKtVR30Rmax4Rmin02Track",
                                       "BTagging_AntiKt4EMPFlowJFVtx",
                                       "BTagging_AntiKt4EMPFlowSecVtx",
                                       "TruthParticles",
                                       "TruthVertices",
                                        "TruthBottom", "TruthElectrons","TruthMuons","TruthTaus",
                                        ]

from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
if flags.BTagging.Pseudotrack:
    FTAG1SlimmingHelper.AllVariables += [ "InDetPseudoTrackParticles" ]

if flags.BTagging.Trackless:
    FTAG1SlimmingHelper.AllVariables += [
                                        "JetAssociatedPixelClusters",
                                        "JetAssociatedSCTClusters",
                                        ]

# Static content
FtagCommon.add_static_content_to_SlimmingHelper(FTAG1SlimmingHelper)

# Trigger content
FtagCommon.trigger_setup(FTAG1SlimmingHelper)

# Add truth containers
FtagCommon.add_truth_to_SlimmingHelper(FTAG1SlimmingHelper)

# Add ExtraVariables
FtagCommon.add_ExtraVariables_to_SlimmingHelper(FTAG1SlimmingHelper)

# Add trigger matching
PhysCommonTrigger.trigmatching_helper_notau.add_to_slimming(FTAG1SlimmingHelper)
PhysCommonTrigger.trigmatching_helper_tau.add_to_slimming(FTAG1SlimmingHelper)

# Final construction of output stream
FTAG1SlimmingHelper.AppendContentToStream(FTAG1Stream)

