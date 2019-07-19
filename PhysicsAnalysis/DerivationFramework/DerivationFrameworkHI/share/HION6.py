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
  from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
  addStandardTruthContents()

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

print "hion6expression_lep: ", hion6expression_lep
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
thinningTools = DerivationFrameworkTop.TOPQCommonThinning.setup('HION6',HION6ThinningHelper.ThinningSvc(), ToolSvc)

#====================================================================
# CREATE THE KERNEL(S)
#====================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel

# Create the private sequence
HION6Sequence = CfgMgr.AthSequencer("HION6Sequence")

# First skim on leptons
HION6Sequence += CfgMgr.DerivationFramework__DerivationKernel("HION6SkimmingKernel_lep", SkimmingTools = [HION6StringSkimmingTool_lep])

# add fat/trimmed jets
from DerivationFrameworkTop.TOPQCommonJets import addStandardJetsForTop
addStandardJetsForTop(HION6Sequence,'HION6')

# apply jet calibration
from DerivationFrameworkTop.TOPQCommonJets import applyTOPQJetCalibration
applyTOPQJetCalibration("AntiKt4EMTopo",DerivationFrameworkJob)
applyTOPQJetCalibration("AntiKt10LCTopoTrimmedPtFrac5SmallR20",HION6Sequence)

# Then skim on the newly created fat jets and calibrated jets
HION6Sequence += CfgMgr.DerivationFramework__DerivationKernel("HION6SkimmingKernel_jet", SkimmingTools = skimmingTools_jet)

# Retagging to get BTagging_AntiKt4EMPFlow Collection (not present in primary AOD)
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += [ "AntiKt4EMPFlow->AntiKt4EMTopo" ]

TaggerList = BTaggingFlags.StandardTaggers
from DerivationFrameworkFlavourTag.FlavourTagCommon import ReTag
ReTag(TaggerList,['AntiKt4EMPFlowJets'],HION6Sequence)

# Then apply truth tools in the form of augmentation
if DFisMC:
  from DerivationFrameworkTop.TOPQCommonTruthTools import *
  HION6Sequence += TOPQCommonTruthKernel

# add MSV variables
from DerivationFrameworkTop.TOPQCommonJets import addMSVVariables
addMSVVariables("AntiKt4EMTopoJets", HION6Sequence, ToolSvc)

# Then apply thinning
HION6Sequence += CfgMgr.DerivationFramework__DerivationKernel("HION6Kernel", ThinningTools = thinningTools)

#====================================================================
# JetTagNonPromptLepton decorations
#====================================================================
import JetTagNonPromptLepton.JetTagNonPromptLeptonConfig as JetTagConfig

# Build AntiKt4PV0TrackJets and run b-tagging
JetTagConfig.ConfigureAntiKt4PV0TrackJets(HION6Sequence, 'HION6')

# Add BDT decoration algs
HION6Sequence += JetTagConfig.GetDecoratePromptLeptonAlgs()
HION6Sequence += JetTagConfig.GetDecoratePromptTauAlgs()

# Finally, add the private sequence to the main job
DerivationFrameworkJob += HION6Sequence

#====================================================================
# SLIMMING
#====================================================================
import DerivationFrameworkTop.TOPQCommonSlimming
DerivationFrameworkTop.TOPQCommonSlimming.setup('HION6', HION6Stream)

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
