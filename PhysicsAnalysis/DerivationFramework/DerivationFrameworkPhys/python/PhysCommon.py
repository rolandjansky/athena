# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
from DerivationFrameworkJetEtMiss.JetCommon import addEventCleanFlags, addBadBatmanFlag, addDistanceInTrain, addDAODJets, addQGTaggerTool, getPFlowfJVT, addJetTruthLabel, addVRTrackJetMoments, addSidebandEventShape
from JetRecConfig.StandardSmallRJets import AntiKt4EMTopo,AntiKt4EMPFlow,AntiKtVR30Rmax4Rmin02PV0Track
from JetRecConfig.StandardLargeRJets import AntiKt10LCTopoTrimmed
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

jetList = [AntiKt4EMTopo,
           AntiKt4EMPFlow,
           AntiKtVR30Rmax4Rmin02PV0Track,
           AntiKt10LCTopoTrimmed]

addDAODJets(jetList,DerivationFrameworkJob)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=DerivationFrameworkJob,labelname="R10TruthLabel_R21Consolidated")
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=DerivationFrameworkJob,labelname="R10TruthLabel_R21Precision")

# q/g tagging variables
addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=DerivationFrameworkJob)
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=DerivationFrameworkJob)

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=DerivationFrameworkJob)

# Special rho definition for PFlow jets
addSidebandEventShape(sequence=DerivationFrameworkJob)

# extra variables for VR jets
addVRTrackJetMoments(jetalg="AntiKtVR30Rmax4Rmin02PV0Track",sequence=DerivationFrameworkJob)

# Event cleaning flags
addEventCleanFlags(sequence=DerivationFrameworkJob)
# Bad batman flag for events with large EMEC-IW Noise
addBadBatmanFlag(sequence=DerivationFrameworkJob)
# Distance in train
addDistanceInTrain(sequence=DerivationFrameworkJob)

scheduleStandardMETContent(sequence=DerivationFrameworkJob, algname="METAssociationAlg")

#====================================================================
# Tau   
#====================================================================

# Add low-pt di-tau reconstruction
from DerivationFrameworkTau.TauCommon import addDiTauLowPt
addDiTauLowPt(Seq=DerivationFrameworkJob)

# Add tau WP decoration
from DerivationFrameworkTau.TauCommon import addTauWPDecoration
addTauWPDecoration(Seq=DerivationFrameworkJob, evetoFixTag="v1")

#====================================================================
# FLAVOUR TAGGING   
#====================================================================
from DerivationFrameworkFlavourTag.FtagRun3DerivationConfig import FtagJetCollections
FtagJetCollections(['AntiKt4EMPFlowJets','AntiKtVR30Rmax4Rmin02TrackJets'],DerivationFrameworkJob)

