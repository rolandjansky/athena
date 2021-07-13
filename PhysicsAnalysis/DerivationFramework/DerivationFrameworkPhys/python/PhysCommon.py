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
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDAODJets, addDefaultTrimmedJets, addJetTruthLabel, addQGTaggerTool, getPFlowfJVT, addEventCleanFlags
from DerivationFrameworkJetEtMiss.METCommon import scheduleStandardMETContent
from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType
from DerivationFrameworkTrigger.TriggerMatchingHelper import TriggerMatchingHelper
import re

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
# TRIGGER CONTENT   
#====================================================================
## See https://twiki.cern.ch/twiki/bin/view/Atlas/TriggerAPI
## Get single and multi mu, e, photon triggers
## Jet, tau, multi-object triggers not available in the matching code
allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
trig_tau = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, livefraction=0.8)
## Add cross-triggers for some sets
trig_em = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
trig_et = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.tau, livefraction=0.8)
trig_mt = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.tau, livefraction=0.8)
## Note that this seems to pick up both isolated and non-isolated triggers already, so no need for extra grabs
trig_txe = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.tau, additionalTriggerType=TriggerType.xe, livefraction=0.8)
#
## Merge and remove duplicates
trigger_names_full_notau = list(set(trig_el+trig_mu+trig_g+trig_em+trig_et+trig_mt))
trigger_names_full_tau = list(set(trig_tau+trig_txe))
#
## Now reduce the list...
trigger_names_notau = []
trigger_names_tau = []
from AthenaConfiguration.AutoConfigFlags import GetFileMD
from AthenaConfiguration.AllConfigFlags import ConfigFlags

if ConfigFlags.Trigger.EDMVersion == 3:
   r_tau = re.compile("HLT_.*tau.*")
   r_notau = re.compile("HLT_[1-9]*(e|mu).*") 
   r_veto = re.compile("HLT_.*(LRT).*")   
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      result_tau = r_tau.match(chain_name)
      result_notau = r_notau.match(chain_name)
      result_veto = r_veto.match(chain_name)
      if result_tau is not None and result_veto is None: trigger_names_tau.append(chain_name)
      if result_notau is not None and result_veto is None: trigger_names_notau.append(chain_name)
   trigger_names_notau = set(trigger_names_notau) - set(trigger_names_tau)
   trigger_names_notau = list(trigger_names_notau)
else:
   for chain_name in GetFileMD(ConfigFlags.Input.Files)['TriggerMenu']['HLTChains']:
      if chain_name in trigger_names_full_notau: trigger_names_notau.append(chain_name)
      if chain_name in trigger_names_full_tau:   trigger_names_tau.append(chain_name) 
# Create trigger matching decorations
PhysCommon_trigmatching_helper_notau = TriggerMatchingHelper(name='PhysCommonTriggerMatchingToolNoTau',
        trigger_list = trigger_names_notau, add_to_df_job=True)
PhysCommon_trigmatching_helper_tau = TriggerMatchingHelper(name='PhysCommonTriggerMatchingToolTau',
        trigger_list = trigger_names_tau, add_to_df_job=True, DRThreshold=0.2)

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
           "AntiKt2PV0TrackJets",
           "AntiKt4PV0TrackJets",
           "AntiKtVR30Rmax4Rmin02PV0TrackJets"]

if (DerivationFrameworkIsMonteCarlo):
   OutputJets["PhysCommon"].append("AntiKt10TruthTrimmedPtFrac5SmallR20Jets")

addDAODJets(jetList,DerivationFrameworkJob,"PhysCommon")
add_largeR_truth_jets = DerivationFrameworkIsMonteCarlo and not hasattr(DerivationFrameworkJob,'jetalgAntiKt10TruthTrimmedPtFrac5SmallR20')
addDefaultTrimmedJets(DerivationFrameworkJob,"PhysCommon",dotruth=add_largeR_truth_jets, linkVRGhosts=True)

# Add large-R jet truth labeling
if (DerivationFrameworkIsMonteCarlo):
   addJetTruthLabel(jetalg="AntiKt10LCTopoTrimmedPtFrac5SmallR20",sequence=DerivationFrameworkJob,algname="JetTruthLabelingAlg",labelname="R10TruthLabel_R21Consolidated")

addQGTaggerTool(jetalg="AntiKt4EMTopo",sequence=DerivationFrameworkJob,algname="QGTaggerToolAlg")
addQGTaggerTool(jetalg="AntiKt4EMPFlow",sequence=DerivationFrameworkJob,algname="QGTaggerToolPFAlg")

# fJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=DerivationFrameworkJob, algname='PHYSJetForwardPFlowJvtToolAlg')

# Event cleaning flags
addEventCleanFlags(sequence=DerivationFrameworkJob)

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
FtagJetCollections(['AntiKt4EMPFlowJets','AntiKtVR30Rmax4Rmin02TrackJets'],DerivationFrameworkJob)

