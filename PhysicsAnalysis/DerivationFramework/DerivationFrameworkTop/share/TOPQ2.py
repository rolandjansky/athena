#====================================================================
# TOPQ2 
# DILEPTON SELECTION
#  >=2 leptons(pT>15GeV) OR
#  >=2 leptons(pT>10GeV) && >=1 lepton(pT>20GeV)
# reductionConf flag TOPQ2 in Reco_tf.py
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

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_TOPQ2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_TOPQ2Stream )
TOPQ2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Accept the most selective kernel (last one in sequence; later in derivation)
TOPQ2Stream.AcceptAlgs(["TOPQ2Kernel"])

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
TOPQ2ThinningHelper = ThinningHelper("TOPQ2ThinningHelper")
TOPQ2ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains()
TOPQ2ThinningHelper.AppendToStream(TOPQ2Stream)

#====================================================================
# SKIMMING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonSelection
skimmingTools_lep = DerivationFrameworkTop.TOPQCommonSelection.setup_lep('TOPQ2', ToolSvc)
skimmingTools_jet = DerivationFrameworkTop.TOPQCommonSelection.setup_jet('TOPQ2', ToolSvc)

#====================================================================
# THINNING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonThinning
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQ2',TOPQ2ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# CREATE THE KERNEL(S) 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
TOPQ2Sequence = CfgMgr.AthSequencer("TOPQ2Sequence")

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]
ReTag(['IP2D', 'IP3D', 'SV0',  'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'MV2c00', 'MV2c10', 'MV2c20', 'MV2c100', 'MV2m'],['AntiKt4EMPFlowJets'], DerivationFrameworkJob)

# First skim on leptons
TOPQ2Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ2SkimmingKernel_lep", SkimmingTools = skimmingTools_lep)

# Then build fat/trimmed jets
import DerivationFrameworkTop.TOPQCommonJets
addDefaultTrimmedJets(TOPQ2Sequence,'TOPQ2')

#Then apply jet calibration
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",TOPQ2Sequence)

# Then skim on the newly created fat jets and calibrated jets
TOPQ2Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ2SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Then apply the TruthWZ fix
if DFisMC:
  replaceBuggyAntiKt4TruthWZJets(TOPQ2Sequence,'TOPQ2')

# Then apply truth tools in the form of aumentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  TOPQ2Sequence += TOPQCommonTruthKernel

# Then apply thinning
TOPQ2Sequence += CfgMgr.DerivationFramework__DerivationKernel("TOPQ2Kernel", ThinningTools = thinningTools)

# Finally, add the private sequence to the main job
DerivationFrameworkJob += TOPQ2Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ2', TOPQ2Stream)
