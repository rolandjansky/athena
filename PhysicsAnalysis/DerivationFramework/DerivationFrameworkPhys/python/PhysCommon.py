# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#====================================================================
# PhysCommon.py
# This defines all of the containers needed both in PHYS and PHYSLITE
# Using it avoids name clashes when running in trains
# In principle it can also be used by other formats who want to take
# advantage of PHYS/PHYSLITE containers
#====================================================================
from AthenaCommon.AppMgr import ToolSvc
from AthenaCommon import CfgMgr
from DerivationFrameworkCore.DerivationFrameworkMaster import DerivationFrameworkIsMonteCarlo, DerivationFrameworkJob
from DerivationFrameworkInDet import InDetCommon
from DerivationFrameworkEGamma import EGammaCommon
from DerivationFrameworkMuons import MuonsCommon
# Common augmentations ("DFCommon")
InDetCommon.makeInDetDFCommon()
EGammaCommon.makeEGammaDFCommon()
MuonsCommon.makeMuonsDFCommon()
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets, addBadBatmanFlag, addDistanceInTrain
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDAODJets, addDefaultTrimmedJets, addJetTruthLabel, addQGTaggerTool, getPFlowfJVT, addEventCleanFlags
from DerivationFrameworkJetEtMiss.METCommon import scheduleStandardMETContent

### Augmentation tools lists
AugmentationTools   = []

#====================================================================
# MONTE CARLO TRUTH
#====================================================================
if (DerivationFrameworkIsMonteCarlo):
   from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addMiniTruthCollectionLinks,addHFAndDownstreamParticles,addPVCollection
   #import DerivationFrameworkHiggs.TruthCategories
   # Add charm quark collection
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthCollectionMaker
   PhysCommonTruthCharmTool = DerivationFramework__TruthCollectionMaker(name                    = "PhysCommonTruthCharmTool",
                                                                        NewCollectionName       = "TruthCharm",
                                                                        KeepNavigationInfo      = False,
                                                                        ParticleSelectionString = "(abs(TruthParticles.pdgId) == 4)",
                                                                        Do_Compress             = True)
   ToolSvc += PhysCommonTruthCharmTool 
   DerivationFrameworkJob += CfgMgr.DerivationFramework__CommonAugmentation("PhysCommonTruthCharmKernel",AugmentationTools=[PhysCommonTruthCharmTool])
   # Add HF particles
   addHFAndDownstreamParticles(DerivationFrameworkJob)
   # Add standard truth
   addStandardTruthContents(DerivationFrameworkJob,prefix='DFCommon_')
   # Update to include charm quarks and HF particles - require a separate instance to be train safe
   from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthNavigationDecorator
   PhysCommonTruthNavigationDecorator = DerivationFramework__TruthNavigationDecorator( name="PhysCommonTruthNavigationDecorator",
          InputCollections=["TruthElectrons", "TruthMuons", "TruthPhotons", "TruthTaus", "TruthNeutrinos", "TruthBSM", "TruthBottom", "TruthTop", "TruthBoson","TruthCharm","TruthHFWithDecayParticles"])
   ToolSvc += PhysCommonTruthNavigationDecorator
   DerivationFrameworkJob.DFCommon_MCTruthNavigationDecoratorKernel.AugmentationTools = [PhysCommonTruthNavigationDecorator]
   # Re-point links on reco objects
   addMiniTruthCollectionLinks(DerivationFrameworkJob)
   addPVCollection(DerivationFrameworkJob)
   # Set appropriate truth jet collection for tau truth matching
   ToolSvc.DFCommonTauTruthMatchingTool.TruthJetContainerName = "AntiKt4TruthDressedWZJets"

#====================================================================
# JET/MET   
#====================================================================

# TODO: UFO jets to be added in the future
largeRJetCollections = [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets" #, "AntiKt10UFOCSSKSoftDropBeta100Zcut10Jets"
]

OutputJets["PhysCommon"] = largeRJetCollections
jetList = ["AntiKt4EMTopoJets",
           "AntiKt4EMPFlowJets",
           "AntiKt4EMPFlowJets_BTagging201903",
           "AntiKtVR30Rmax4Rmin02PV0TrackJets",
           "AntiKtVR30Rmax4Rmin02TrackJets_BTagging201903"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PhysCommon"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

addDAODJets(jetList,DerivationFrameworkJob,"PhysCommon")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(DerivationFrameworkJob,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20')
addDefaultTrimmedJets(DerivationFrameworkJob,"PhysCommon",dotruth=add_largeR_truth_jets, linkVRGhosts=True)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=DerivationFrameworkJob,labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=DerivationFrameworkJob)
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=DerivationFrameworkJob)

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=DerivationFrameworkJob)

# Event cleaning flags
addEventCleanFlags(sequence=DerivationFrameworkJob)
# Bad batman flag for events with large EMEC-IW Noise
addBadBatmanFlag(sequence=DerivationFrameworkJob)
# Distance in train
addDistanceInTrain(sequence=DerivationFrameworkJob)

scheduleStandardMETContent(sequence=DerivationFrameworkJob, algname="METAssociationAlg")

#====================================================================
# EGAMMA
#====================================================================

if DerivationFrameworkIsMonteCarlo:
   # Schedule the two energy density tools for running after the pseudojets are created.
   for alg in ['EDTruthCentralAlg', 'EDTruthForwardAlg']:
      if hasattr(DerivationFrameworkJob, alg):
         edtalg = getattr(DerivationFrameworkJob, alg)
         delattr(DerivationFrameworkJob, alg)
         DerivationFrameworkJob += edtalg

#====================================================================
# Tau   
#====================================================================

# Add low-pt di-tau reconstruction
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt(Seq=DerivationFrameworkJob)

#====================================================================
# FLAVOUR TAGGING   
#====================================================================
from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollections
FtagJetCollections(['AntiKt4EMPFlowJets', 'AntiKtVR30Rmax4Rmin02TrackJets'],DerivationFrameworkJob)

