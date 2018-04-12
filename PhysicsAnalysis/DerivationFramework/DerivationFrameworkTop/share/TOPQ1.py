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
# CREATE THE KERNEL(S)
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
TOPQ1Sequence = CfgMgr.AthSequencer("TOPQ1Sequence")

# First skim on leptons
TOPQ1Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ1SkimmingKernel_lep", SkimmingTools = skimmingTools_lep)

# Then build fat/trimmed jets
from DerivationFrameworkTop.TOPQCommonJets import addStandardJetsForTop
addStandardJetsForTop(TOPQ1Sequence,'TOPQ1')

#Then apply jet calibration
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ1Sequence)

# Then skim on the newly created fat jets and calibrated jets
TOPQ1Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ1SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]
ReTag(['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP'],
      ['AntiKt4EMPFlowJets'],
      TOPQ1Sequence)

# Removing manual scheduling of ELReset, see https://its.cern.ch/jira/browse/ATLASRECTS-3988
# if not hasattr(TOPQ1Sequence,"ELReset"):
#   TOPQ1Sequence += CfgMgr.xAODMaker__ElementLinkResetAlg( "ELReset" )


# THIS IS NO LONGER NEEDED IN REL 21, REMOVE IN FUTURE (May-17)
# # Then apply the TruthWZ fix
# if DFisMC:
#   replaceBuggyAntiKt4TruthWZJets(TOPQ1Sequence,'TOPQ1')

# Then apply truth tools in the form of aumentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  TOPQ1Sequence += TOPQCommonTruthKernel

DerivationFrameworkTop.TOPQCommonJets.addMSVVariables("AntiKt4EMTopoJets", TOPQ1Sequence, ToolSvc)

# Then apply thinning
TOPQ1Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ1Kernel", ThinningTools = thinningTools)

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ1Sequence, 'TOPQ1')

# Add BDT decoration algs
TOPQ1Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ1Sequence += JetTagConfig.GetDecoratePromptTauAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ1Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ1', TOPQ1Stream)
