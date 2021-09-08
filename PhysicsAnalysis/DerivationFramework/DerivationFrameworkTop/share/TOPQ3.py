# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#====================================================================
# TOPQ3
# LEPTON + JETS SELECTION
#  >=1 lepton(pT>20GeV) AND
#  >=4 akt4calibjet(pT>15GeV) OR >=1 largeRjet(pT>200GeV)
# reductionConf flag TOPQ3 in Reco_tf.py
#====================================================================

#====================================================================
# IMPORTS - Order Matters
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkInDet.InDetCommon import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from AthenaCommon.GlobalFlags import globalflags
DFisMC = (globalflags.DataSource()=='geant4')

# no truth info for data xAODs
if DFisMC:
    from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
    addStandardTruthContents()

#====================================================================
# SET UP STREAM
#====================================================================
streamName = derivationFlags.WriteDAOD_TOPQ3Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TOPQ3Stream )
TOPQ3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Accept the most selective kernel (last one in sequence; later in derivation)
TOPQ3Stream.AcceptAlgs(["TOPQ3Kernel"])

#====================================================================
# PDF Weight Metadata
#====================================================================
if DFisMC:
    from DerivationFrameworkCore.WeightMetadata import *

#====================================================================
# TRIGGER NAVIGATION THINNING
#====================================================================
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
import DerivationFrameworkTop.TOPQCommonThinning
TOPQ3ThinningHelper = ThinningHelper("TOPQ3ThinningHelper")
TOPQ3ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains('leptonicTriggers' if globalflags.DataSource()!='geant4' else 'allTriggers')
TOPQ3ThinningHelper.AppendToStream(TOPQ3Stream)

#====================================================================
# SKIMMING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonSelection
skimmingTools_lep = DerivationFrameworkTop.TOPQCommonSelection.setup_lep('TOPQ3', ToolSvc)
skimmingTools_jet = DerivationFrameworkTop.TOPQCommonSelection.setup_jet('TOPQ3', ToolSvc)

#====================================================================
# THINNING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonThinning
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQ3',TOPQ3ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# CREATE THE KERNEL(S)
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
TOPQ3Sequence = CfgMgr.AthSequencer("TOPQ3Sequence")

# First skim on leptons
TOPQ3Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ3SkimmingKernel_lep", SkimmingTools = skimmingTools_lep)

# Before any custom jet reconstruction, it's good to set up the output list
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
OutputJets["TOPQ3"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
# Only include those ones that you use. The order in the list is not significant
reducedJetList = ["AntiKt2PV0TrackJets", # This collection will be flavour-tagged automatically
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList, TOPQ3Sequence, "TOPQ3")

# If you use AntiKt10*PtFrac5SmallR20Jets, these must be scheduled
# *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(TOPQ3Sequence, "TOPQ3")

# apply jet calibration
from DerivationFrameworkTop.TOPQCommonJets import applyTOPQJetCalibration
applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ3Sequence)

# Then skim on the newly created fat jets and calibrated jets
TOPQ3Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ3SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]

TaggerList = BTaggingFlags.StandardTaggers
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = TOPQ3Sequence)

# Decorate PFlow jets with FJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getPFlowfJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=TOPQ3Sequence, algname='JetForwardPFlowJvtToolAlg')

# Decorate PFlow jets with MVfJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=TOPQ3Sequence, algname='JetForwardJvtToolBDTAlg')


# Then apply truth tools in the form of aumentation
if DFisMC:
    from DerivationFrameworkTop.TOPQCommonTruthTools import *
    TOPQ3Sequence += TOPQCommonTruthKernel

# add MSV variables
from DerivationFrameworkTop.TOPQCommonJets import addMSVVariables
addMSVVariables("AntiKt4EMTopoJets", TOPQ3Sequence, ToolSvc)

# Then apply thinning
TOPQ3Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ3Kernel", ThinningTools = thinningTools)

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
#import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
#JetTagNonPromptLepton package is moved to LeptonTaggers package
import LeptonTaggers.LeptonTaggersConfig as JetTagConfig
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ3Sequence, 'TOPQ3')

# Add BDT decoration algs
TOPQ3Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ3Sequence += JetTagConfig.GetDecoratePromptTauAlgs()
TOPQ3Sequence += LepTagConfig.GetDecorateImprovedPromptLeptonAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ3Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ3', TOPQ3Stream)
