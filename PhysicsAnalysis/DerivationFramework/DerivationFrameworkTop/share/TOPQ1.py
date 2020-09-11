# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

#====================================================================
# TOPQ1
# SINGLE TOP SELECTION
#   >=1 electron(pT>20GeV) OR
#   >=1 muon(pT>20GeV)
# reductionConf flag TOPQ1 in Reco_tf.py
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
streamName = derivationFlags.WriteDAOD_TOPQ1Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TOPQ1Stream )
TOPQ1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Accept the most selective kernel (last one in sequence; later in derivation)
TOPQ1Stream.AcceptAlgs(["TOPQ1Kernel"])

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
TOPQ1ThinningHelper = ThinningHelper("TOPQ1ThinningHelper")
TOPQ1ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains('leptonicTriggers' if globalflags.DataSource()!='geant4' else 'allTriggers')
TOPQ1ThinningHelper.AppendToStream(TOPQ1Stream)

#====================================================================
# SKIMMING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonSelection
skimmingTools_lep = DerivationFrameworkTop.TOPQCommonSelection.setup_lep('TOPQ1', ToolSvc)
skimmingTools_jet = DerivationFrameworkTop.TOPQCommonSelection.setup_jet('TOPQ1', ToolSvc)

#====================================================================
# THINNING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonThinning
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQ1',TOPQ1ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# Add K_S0->pi+pi- reconstruction (TOPQDERIV-69)
#====================================================================
doSimpleV0Finder = False
if doSimpleV0Finder:
    include("DerivationFrameworkBPhys/configureSimpleV0Finder.py")
else:
    include("DerivationFrameworkBPhys/configureV0Finder.py")

TOPQ1_V0FinderTools = BPHYV0FinderTools("TOPQ1")
print TOPQ1_V0FinderTools

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_V0Finder
TOPQ1_Reco_V0Finder   = DerivationFramework__Reco_V0Finder(
    name                   = "TOPQ1_Reco_V0Finder",
    V0FinderTool           = TOPQ1_V0FinderTools.V0FinderTool,
    OutputLevel            = WARNING,
    V0ContainerName        = "TOPQ1RecoV0Candidates",
    KshortContainerName    = "TOPQ1RecoKshortCandidates",
    LambdaContainerName    = "TOPQ1RecoLambdaCandidates",
    LambdabarContainerName = "TOPQ1RecoLambdabarCandidates",
    CheckVertexContainers  = ["PrimaryVertices"]
)

ToolSvc += TOPQ1_Reco_V0Finder
print TOPQ1_Reco_V0Finder

#====================================================================
# CREATE THE KERNEL(S)
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
TOPQ1Sequence = CfgMgr.AthSequencer("TOPQ1Sequence")

# First skim on leptons
TOPQ1Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ1SkimmingKernel_lep", SkimmingTools = skimmingTools_lep)

#====================================================================
# Special jets
#====================================================================
# Create TCC objects (see JETM1.py)
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(TOPQ1Sequence, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")

# Before any custom jet reconstruction, it's good to set up the output list
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
OutputJets["TOPQ1"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
# Only include those ones that you use. The order in the list is not significant
reducedJetList = ["AntiKt2PV0TrackJets", # This collection will be flavour-tagged automatically
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets",
                  "AntiKt10TrackCaloClusterJets"]
replaceAODReducedJets(reducedJetList, TOPQ1Sequence, "TOPQ1")

# If you use AntiKt10*PtFrac5SmallR20Jets, these must be scheduled
# *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets,addDefaultUFOSoftDropJets
addDefaultTrimmedJets(TOPQ1Sequence,"TOPQ1")
addDefaultUFOSoftDropJets(TOPQ1Sequence,"TOPQ1",dotruth=True)


# add SoftDrop jets
from DerivationFrameworkTop.TOPQCommonJets import addSoftDropJetsForTop
addSoftDropJetsForTop(TOPQ1Sequence, "TOPQ1")

# add TTC jets
from DerivationFrameworkTop.TOPQCommonJets import addTCCTrimmedJetsForTop
addTCCTrimmedJetsForTop(TOPQ1Sequence, "TOPQ1")

# add VR jets
from DerivationFrameworkTop.TOPQCommonJets import addVRJetsForTop
addVRJetsForTop(TOPQ1Sequence,'201810')
addVRJetsForTop(TOPQ1Sequence,'201903')

# add Xbb tagger information
from DerivationFrameworkTop.TOPQCommonJets import addXbbTaggerInformation
addXbbTaggerInformation(TOPQ1Sequence, ToolSvc)

# apply jet calibration
from DerivationFrameworkTop.TOPQCommonJets import applyTOPQJetCalibration
applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ1Sequence)

# Then skim on the newly created fat jets and calibrated jets
TOPQ1Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ1SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

TaggerList = BTaggingFlags.StandardTaggers
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = TOPQ1Sequence)

# Quark-gluon tagging
truthjetalg='AntiKt4TruthJets'
if not DFisMC:
    truthjetalg=None
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addQGTaggerTool
addQGTaggerTool(jetalg="AntiKt4EMTopo", sequence=TOPQ1Sequence, algname="QGTaggerToolAlg", truthjetalg=truthjetalg)
addQGTaggerTool(jetalg="AntiKt4EMPFlow", sequence=TOPQ1Sequence, algname="QGTaggerToolAlg", truthjetalg=truthjetalg)

# Decorate PFlow jets with FJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import getPFlowfJVT
getPFlowfJVT(jetalg='AntiKt4EMPFlow',sequence=TOPQ1Sequence, algname='JetForwardPFlowJvtToolAlg')

# Decorate PFlow jets with MVfJVT
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import applyMVfJvtAugmentation
applyMVfJvtAugmentation(jetalg='AntiKt4EMTopo',sequence=TOPQ1Sequence, algname='JetForwardJvtToolBDTAlg')

# Then apply truth tools in the form of aumentation
if DFisMC:
    from DerivationFrameworkTop.TOPQCommonTruthTools import *
    TOPQ1Sequence += TOPQCommonTruthKernel

# add MSV variables
from DerivationFrameworkTop.TOPQCommonJets import addMSVVariables
addMSVVariables("AntiKt4EMTopoJets", TOPQ1Sequence, ToolSvc)

# add ExKtDoubleTagVariables (TOPQDERIV-62)
from DerivationFrameworkTop.TOPQCommonJets import addExKtDoubleTagVariables
addExKtDoubleTagVariables(TOPQ1Sequence, ToolSvc)

# Then apply thinning
#AugmentationTool for TOPQDERIV-69
TOPQ1Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ1Kernel", ThinningTools = thinningTools, AugmentationTools = [TOPQ1_Reco_V0Finder])

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig
import LeptonTaggers.LeptonTaggersConfig as LepTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ1Sequence, 'TOPQ1')

# Add BDT decoration algs
TOPQ1Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ1Sequence += JetTagConfig.GetDecoratePromptTauAlgs()
TOPQ1Sequence += LepTagConfig.GetDecorateImprovedPromptLeptonAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ1Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ1', TOPQ1Stream)
