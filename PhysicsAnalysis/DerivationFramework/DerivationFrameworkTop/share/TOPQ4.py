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
TOPQ4ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains('hadronicTriggers' if globalflags.DataSource()!='geant4' else 'jetTriggers')
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

# First skim on leptons
TOPQ4Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ4SkimmingKernel_lep", SkimmingTools = skimmingTools_lep)

# Then build fat/trimmed jets
from DerivationFrameworkTop.TOPQCommonJets import addStandardJetsForTop
addStandardJetsForTop(TOPQ4Sequence,'TOPQ4')

#Then apply jet calibration
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ4Sequence)

# Tag jets
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import updateJVT_xAODColl
updateJVT_xAODColl('AntiKt4EMTopo', DerivationFrameworkJob)
from DerivationFrameworkFlavourTag.FlavourTagCommon import applyBTagging_xAODColl
applyBTagging_xAODColl('AntiKt4EMTopo', DerivationFrameworkJob)

# Then skim on the newly created fat jets and calibrated jets
TOPQ4Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ4SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]
ReTag(['IP2D', 'IP3D', 'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'SoftMu', 'MV2c10', 'MV2c10mu', 'MV2c10rnn', 'JetVertexCharge', 'MV2c100', 'MV2cl100' , 'DL1', 'DL1rnn', 'DL1mu', 'RNNIP'],
      ['AntiKt4EMPFlowJets'],
      TOPQ4Sequence)

# Removing manual scheduling of ELReset, see https://its.cern.ch/jira/browse/ATLASRECTS-3988
# if not hasattr(TOPQ4Sequence,"ELReset"):
#   TOPQ4Sequence += CfgMgr.xAODMaker__ElementLinkResetAlg( "ELReset" )


# THIS IS NO LONGER NEEDED IN REL 21, REMOVE IN FUTURE (May-17)
# # Then apply the TruthWZ fix
# if DFisMC:
#   replaceBuggyAntiKt4TruthWZJets(TOPQ4Sequence,'TOPQ4')

# Then apply truth tools in the form of aumentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  TOPQ4Sequence += TOPQCommonTruthKernel

DerivationFrameworkTop.TOPQCommonJets.addMSVVariables("AntiKt4EMTopoJets", TOPQ4Sequence, ToolSvc)

# Then apply thinning
TOPQ4Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ4Kernel", ThinningTools = thinningTools)

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(TOPQ4Sequence, 'TOPQ4')

# Add BDT decoration algs
TOPQ4Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
TOPQ4Sequence += JetTagConfig.GetDecoratePromptTauAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ4Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ4', TOPQ4Stream)
