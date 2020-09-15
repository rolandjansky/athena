# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#====================================================================
# TOPQ4
# ALL HADRONIC SELECTION
#  >=5 akt4calibjet(pT>20GeV) OR
#  >=1 largeRjet(pT>200GeV)
# reductionConf flag TOPQ4 in Reco_tf.py
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
streamName = derivationFlags.WriteDAOD_TOPQ4Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TOPQ4Stream )
TOPQ4Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Accept the most selective kernel (last one in sequence; later in derivation)
TOPQ4Stream.AcceptAlgs(["TOPQ4Kernel"])

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
TOPQ4ThinningHelper = ThinningHelper("TOPQ4ThinningHelper")
#TOPQ4ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains('hadronicTriggers' if globalflags.DataSource()!='geant4' else 'jetTriggers')
TOPQ4ThinningHelper.TriggerChains = "" # temporarily disable trigger thinning for TOPQ4; this will only affect data derivation
TOPQ4ThinningHelper.AppendToStream(TOPQ4Stream)

#====================================================================
# SKIMMING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonSelection
skimmingTools_lep = DerivationFrameworkTop.TOPQCommonSelection.setup_lep('TOPQ4', ToolSvc)
skimmingTools_jet = DerivationFrameworkTop.TOPQCommonSelection.setup_jet('TOPQ4', ToolSvc)

#====================================================================
# THINNING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonThinning
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQ4',TOPQ4ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# CREATE THE KERNEL(S)
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
TOPQ4Sequence = CfgMgr.AthSequencer("TOPQ4Sequence")

from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = TOPQ4Sequence)

# First skim on leptons
TOPQ4Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ4SkimmingKernel_lep", SkimmingTools = skimmingTools_lep)

#====================================================================
# Special jets
#====================================================================
# Create TCC objects (see JETM1.py)
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(TOPQ4Sequence, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")

# Before any custom jet reconstruction, it's good to set up the output list
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
OutputJets["TOPQ4"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
# Only include those ones that you use. The order in the list is not significant
reducedJetList = ["AntiKt2PV0TrackJets", # This collection will be flavour-tagged automatically
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt10TrackCaloClusterJets"]
replaceAODReducedJets(reducedJetList, TOPQ4Sequence, "TOPQ4")

# If you use AntiKt10*PtFrac5SmallR20Jets, these must be scheduled
# *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets,addDefaultUFOSoftDropJets
addDefaultTrimmedJets(TOPQ4Sequence,"TOPQ4")
addDefaultUFOSoftDropJets(TOPQ4Sequence,"TOPQ4",dotruth=True)

# add TTC jets
from DerivationFrameworkTop.TOPQCommonJets import addTCCTrimmedJetsForTop
addTCCTrimmedJetsForTop(TOPQ4Sequence, "TOPQ4")

# add VR jets
from DerivationFrameworkTop.TOPQCommonJets import addVRJetsForTop
addVRJetsForTop(TOPQ4Sequence,'201810')
addVRJetsForTop(TOPQ4Sequence,'201903')

# apply jet calibration
from DerivationFrameworkTop.TOPQCommonJets import applyTOPQJetCalibration
applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
applyTOPQJetCalibration("AntiKt4EMPFlow",DerivationFrameworkJob)
applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ4Sequence)

# Tag jets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import updateJVT_xAODColl
updateJVT_xAODColl('AntiKt4EMTopo', DerivationFrameworkJob)
updateJVT_xAODColl('AntiKt4EMPFlow', DerivationFrameworkJob)	
from DerivationFrameworkFlavourTag.FlavourTagCommon import applyBTagging_xAODColl
applyJetCalibration_xAODColl("AntiKt4EMTopo_BTagging201810", DerivationFrameworkJob)
applyJetCalibration_xAODColl("AntiKt4EMPFlow_BTagging201810", TOPQ4Sequence)
updateJVT_xAODColl('AntiKt4EMTopo_BTagging201810', DerivationFrameworkJob)
updateJVT_xAODColl('AntiKt4EMPFlow_BTagging201810', TOPQ4Sequence)
applyBTagging_xAODColl('AntiKt4EMTopo_BTagging201810', DerivationFrameworkJob)
applyBTagging_xAODColl('AntiKt4EMPFlow_BTagging201810', TOPQ4Sequence)

# Then skim on the newly created fat jets and calibrated jets
TOPQ4Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ4SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

TaggerList = BTaggingFlags.StandardTaggers

# Quark-gluon tagging
truthjetalg='AntiKt4TruthJets'
if not DFisMC:
    truthjetalg=None
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=TOPQ4Sequence, algname="QGTaggerToolAlg", truthjetalg=truthjetalg)
addQGTaggerTool(jetalg="AntiKt4EMPFlow", sequence=TOPQ4Sequence, algname="QGTaggerToolAlg", truthjetalg=truthjetalg)

# Decorate PFlow jets with FJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getPFlowfJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=TOPQ4Sequence, algname='JetForwardPFlowJvtToolAlg')

# Decorate PFlow jets with MVfJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=TOPQ4Sequence, algname='JetForwardJvtToolBDTAlg')


# Then apply truth tools in the form of aumentation
if DFisMC:
    from DerivationFrameworkTop.TOPQCommonTruthTools import *
    TOPQ4Sequence += TOPQCommonTruthKernel

# add MSV variables
from DerivationFrameworkTop.TOPQCommonJets import addMSVVariables
addMSVVariables("AntiKt4EMTopoJets", TOPQ4Sequence, ToolSvc)

# Then apply thinning
TOPQ4Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ4Kernel", ThinningTools = thinningTools)

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ4Sequence, 'TOPQ4')

# Add BDT decoration algs
TOPQ4Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ4Sequence += JetTagConfig.GetDecoratePromptTauAlgs()
TOPQ4Sequence += LepTagConfig.GetDecorateImprovedPromptLeptonAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ4Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ4', TOPQ4Stream)
