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

# Then build fat/trimmed jets
from DerivationFrameworkTop.TOPQCommonJets import addStandardJetsForTop
addStandardJetsForTop(TOPQ3Sequence,'TOPQ3')

#Then apply jet calibration
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ3Sequence)

# Then skim on the newly created fat jets and calibrated jets
TOPQ3Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ3SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]
ReTag(['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP'],
      ['AntiKt4EMPFlowJets'],
      TOPQ3Sequence)

# Removing manual scheduling of ELReset, see https://its.cern.ch/jira/browse/ATLASRECTS-3988
# if not hasattr(TOPQ3Sequence,"ELReset"):
#   TOPQ3Sequence += CfgMgr.xAODMaker__ElementLinkResetAlg( "ELReset" )


# THIS IS NO LONGER NEEDED IN REL 21, REMOVE IN FUTURE (May-17)
# # Then apply the TruthWZ fix
# if DFisMC:
#   replaceBuggyAntiKt4TruthWZJets(TOPQ3Sequence,'TOPQ3')

# Then apply truth tools in the form of aumentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  TOPQ3Sequence += TOPQCommonTruthKernel

DerivationFrameworkTop.TOPQCommonJets.addMSVVariables("AntiKt4EMTopoJets", TOPQ3Sequence, ToolSvc)

# Then apply thinning
TOPQ3Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ3Kernel", ThinningTools = thinningTools)

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ3Sequence, 'TOPQ3')

# Add BDT decoration algs
TOPQ3Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ3Sequence += JetTagConfig.GetDecoratePromptTauAlgs()


# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ3Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ3', TOPQ3Stream)
