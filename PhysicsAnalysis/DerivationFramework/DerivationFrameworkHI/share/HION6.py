#====================================================================
# Author: Marcin Guzik (Marcin.Guzik _at_ cern.ch)
# Co-author: Iwona Grabowska-Bold (Iwona.Grabowska _at_ cern.ch)
# HION6 
#  TOP ANTI-TOP SELECTION
#   >=2 electrons(pT>15GeV) OR
#   >=2 muons(pT>15GeV)     OR 
#   >=2 electrons(pT>15GeV) AND >=1 muon(pT>15GeV) 
# reductionConf flag HION6 in Reco_tf.py
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
  from DerivationFrameworkMCTruth.MCTruthCommon import *

#====================================================================
# SET UP STREAM   
#====================================================================
streamName = derivationFlags.WriteDAOD_HION6Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_HION6Stream )
HION6Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Accept the most selective kernel (last one in sequence; later in derivation)
HION6Stream.AcceptAlgs(["HION6Kernel"])

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
HION6ThinningHelper = ThinningHelper("HION6ThinningHelper")
# HION6ThinningHelper.TriggerChains =  DerivationFrameworkTop.TOPQCommonThinning.TOPQTriggerChains('leptonicTriggers' if globalflags.DataSource()!='geant4' else 'allTriggers')
HION6ThinningHelper.AppendToStream(HION6Stream)

#====================================================================
# SKIMMING TOOLS
#====================================================================
muonsRequirements = '( (Muons.DFCommonMuonsPreselection && Muons.DFCommonGoodMuon) && (Muons.pt > 15*GeV) && (abs(Muons.eta) < 2.6) )'

electronsRequirements = '( (Electrons.pt > 15*GeV) && (abs(Electrons.eta) < 2.6) && (Electrons.DFCommonElectronsLHLoose) )'

jetRequirements = '( (AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5) )'

muonOnlySelection = '( count('+muonsRequirements+') >= 2 )'
electronOnlySelection = '( count('+electronsRequirements+') >= 2 )'
muonElectronSelection = '( ( count('+electronsRequirements+') >= 1 ) && ( count('+muonsRequirements+') >=1 ) )'
#muonElectronSelection = '( count('+electronsRequirements+') +  count('+muonsRequirements+') >=2 )'

jetOnlySelection = 'count('+jetRequirements+') >= 1'

hion6expression_lep = " || ".join([muonOnlySelection,electronOnlySelection,muonElectronSelection])

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION6StringSkimmingTool_lep = DerivationFramework__xAODStringSkimmingTool(
  name       = "HION6StringSkimmingTool_lep",
  expression = hion6expression_lep)

ToolSvc += HION6StringSkimmingTool_lep

print "hion6expression: ", hion6expression_lep
print "StringSkimmingTool_lep: ", HION6StringSkimmingTool_lep


skimmingTools_jet=[]

hion6expression_jet = '('+jetOnlySelection+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION6StringSkimmingTool_jet = DerivationFramework__xAODStringSkimmingTool(
  name       = "HION6StringSkimmingTool_jet",
  expression = hion6expression_jet)

ToolSvc += HION6StringSkimmingTool_jet

skimmingTools_jet.append(HION6StringSkimmingTool_jet)

print "hion6expression_jet: ", hion6expression_jet
print "StringSkimmingTool_jet: ", HION6StringSkimmingTool_jet


#====================================================================
# THINNING TOOLS
#====================================================================
import DerivationFrameworkTop.TOPQCommonThinning
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('TOPQ2',HION6ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# CREATE THE KERNEL(S) 
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
HION6Sequence = CfgMgr.AthSequencer("HION6Sequence")

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from DerivationFrameworkFlavourTag.FlavourTagCommon import *
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]
ReTag(['IP2D', 'IP3D', 'SV0',  'MultiSVbb1',  'MultiSVbb2', 'SV1', 'JetFitterNN', 'MV2c00', 'MV2c10', 'MV2c20', 'MV2c100', 'MV2m'],['AntiKt4EMPFlowJets'], HION6Sequence)

if not hasattr(HION6Sequence,"ELReset"):
  HION6Sequence += CfgMgr.xAODMaker__ElementLinkResetAlg( "ELReset" )


# First skim on leptons
HION6Sequence += CfgMgr.DerivationFramework__DerivationKernel("HION6SkimmingKernel_lep", SkimmingTools = [HION6StringSkimmingTool_lep])

# Then build fat/trimmed jets
import DerivationFrameworkTop.TOPQCommonJets
addDefaultTrimmedJets(HION6Sequence,'HION6')

#Then apply jet calibration
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
DerivationFrameworkTop.TOPQCommonJets.applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",HION6Sequence)

# Then skim on the newly created fat jets and calibrated jets
# HION6Sequence += CfgMgr.DerivationFramework__DerivationKernel("HION6SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Then apply the TruthWZ fix
if DFisMC:
  replaceBuggyAntiKt4TruthWZJets(HION6Sequence,'HION6')

# Then apply truth tools in the form of aumentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  HION6Sequence += TOPQCommonTruthKernel

DerivationFrameworkTop.TOPQCommonJets.addMSVVariables("AntiKt4EMTopoJets", HION6Sequence, ToolSvc)

# Then apply thinning
HION6Sequence += CfgMgr.DerivationFramework__DerivationKernel("HION6Kernel", ThinningTools = thinningTools)

# JetTagNonPromptLepton decorations
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as Config
HION6Sequence += Config.GetDecoratePromptLeptonAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += HION6Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('TOPQ2', HION6Stream)



from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

HION6SlimmingHelper = SlimmingHelper("HION6SlimmingHelper")

HION6SlimmingHelper.AllVariables = ["AntiKt2HIJets","AntiKt3HIJets","AntiKt4HIJets","AntiKt4HITrackJets","CaloSums","HIEventShape","ZdcModules","ZdcTriggerTowers","ZdcSums","MBTSForwardEventInfo", "MBTSModules"]


HION6SlimmingHelper.AppendContentToStream(HION6Stream)

#This enables L1EnergySums
OtherContent = [
    'xAOD::EnergySumRoI#*',
    'xAOD::EnergySumRoIAuxInfo#*'
 ]

for item in OtherContent:
    HION6Stream.AddItem(item)
