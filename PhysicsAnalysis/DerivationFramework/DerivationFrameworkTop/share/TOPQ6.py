#====================================================================
# TOPQ6
# SINGLE TOP SELECTION
#   >=1 electron(pT>20GeV) OR
#   >=1 muon(pT>20GeV)
# same as TOPQ1 but keep all tracks (minimal thinning)
# reductionConf flag TOPQ6 in Reco_tf.py
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
streamName = derivationFlags.WriteDAOD_TOPQ6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TOPQ6Stream )
TOPQ6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Accept the most selective kernel (last one in sequence; later in derivation)
TOPQ6Stream.AcceptAlgs(["TOPQ6Kernel"])

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
TOPQ6ThinningHelper = ThinningHelper("TOPQ6ThinningHelper")
TOPQ6ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains('leptonicTriggers' if globalflags.DataSource()!='geant4' else 'allTriggers')
TOPQ6ThinningHelper.AppendToStream(TOPQ6Stream)

#====================================================================
# SKIMMING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonSelection
skimmingTools_lep = DerivationFrameworkTop.TOPQCommonSelection.setup_lep('TOPQ6', ToolSvc)
skimmingTools_jet = DerivationFrameworkTop.TOPQCommonSelection.setup_jet('TOPQ6', ToolSvc)

#====================================================================
# THINNING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonThinning
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQ6',TOPQ6ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# Add K_S0->pi+pi- reconstruction (TOPQDERIV-69)
#====================================================================
doSimpleV0Finder = False
if doSimpleV0Finder:
  include("DerivationFrameworkBPhys/configureSimpleV0Finder.py")
else:
  include("DerivationFrameworkBPhys/configureV0Finder.py")

TOPQ6_V0FinderTools = BPHYV0FinderTools("TOPQ6")
print TOPQ6_V0FinderTools

from DerivationFrameworkBPhys.DerivationFrameworkBPhysConf import DerivationFramework__Reco_V0Finder
TOPQ6_Reco_V0Finder   = DerivationFramework__Reco_V0Finder(
    name                   = "TOPQ6_Reco_V0Finder",
    V0FinderTool           = TOPQ6_V0FinderTools.V0FinderTool,
    OutputLevel            = WARNING,
    V0ContainerName        = "TOPQ6RecoV0Candidates",
    KshortContainerName    = "TOPQ6RecoKshortCandidates",
    LambdaContainerName    = "TOPQ6RecoLambdaCandidates",
    LambdabarContainerName = "TOPQ6RecoLambdabarCandidates",
    CheckVertexContainers  = ["PrimaryVertices"]
)

ToolSvc += TOPQ6_Reco_V0Finder
print TOPQ6_Reco_V0Finder

#====================================================================
# CREATE THE KERNEL(S)
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
TOPQ6Sequence = CfgMgr.AthSequencer("TOPQ6Sequence")

# First skim on leptons
TOPQ6Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ6SkimmingKernel_lep", SkimmingTools = skimmingTools_lep)

#====================================================================
# Special jets
#====================================================================
# Create TCC objects (see JETM1.py)
from TrackCaloClusterRecTools.TrackCaloClusterConfig import runTCCReconstruction
# Set up geometry and BField
import AthenaCommon.AtlasUnixStandardJob
include("RecExCond/AllDet_detDescr.py")
runTCCReconstruction(TOPQ6Sequence, ToolSvc, "LCOriginTopoClusters", "InDetTrackParticles",outputTCCName="TrackCaloClustersCombinedAndNeutral")

# add fat/trimmed jets
from DerivationFrameworkTop.TOPQCommonJets import addNonLargeRJetsForTop
addNonLargeRJetsForTop(TOPQ6Sequence,'TOPQ6')

# apply jet calibration
from DerivationFrameworkTop.TOPQCommonJets import applyTOPQJetCalibration
applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)

# Then skim on the newly created fat jets and calibrated jets
TOPQ6Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ6SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]

TaggerList = BTaggingFlags.StandardTaggers
from DerivationFrameworkFlavourTag.FlavourTagCommon import ReTag
ReTag(TaggerList,['AntiKt4EMPFlowJets'],TOPQ6Sequence)

# Then apply truth tools in the form of aumentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  TOPQ6Sequence += TOPQCommonTruthKernel

# add MSV variables
from DerivationFrameworkTop.TOPQCommonJets import addMSVVariables
addMSVVariables("AntiKt4EMTopoJets", TOPQ6Sequence, ToolSvc)

# add ExKtDoubleTagVariables (TOPQDERIV-62)
from DerivationFrameworkTop.TOPQCommonJets import addExKtDoubleTagVariables
addExKtDoubleTagVariables(TOPQ6Sequence, ToolSvc)

# Then apply thinning
#AugmentationTool for TOPQDERIV-69
TOPQ6Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ6Kernel", ThinningTools = thinningTools, AugmentationTools = [TOPQ6_Reco_V0Finder])

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ6Sequence, 'TOPQ6')

# Add BDT decoration algs
TOPQ6Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ6Sequence += JetTagConfig.GetDecoratePromptTauAlgs()


# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ6Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ6', TOPQ6Stream)
