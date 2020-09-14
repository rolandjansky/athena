#====================================================================
# Author: Marcin Guzik (Marcin.Guzik _at_ cern.ch)
# Co-author: Iwona Grabowska-Bold (Iwona.Grabowska _at_ cern.ch)
# Co-author: Prabhakar Palni (Prabhakar.Palni@cern.ch)
# HION6 
#  TOP ANTI-TOP SELECTION
#   >=2 electrons(pT>15GeV) OR
#   >=2 muons(pT>15GeV)     OR 
#   >=1 electrons(pT>15GeV) AND >=1 muon(pT>15GeV) OR
#   >=2 leptons(pT>10GeV) AND >=1 leptom(pT>20GeV)
#  
# Lepton + jets
#   >=1  leptons(pT>13GeV)   AND   >= 4 akt4calibjet(pT>15GeV)
# 
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

# no truth info for data xAODs
if DerivationFrameworkHasTruth:
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
if DerivationFrameworkHasTruth:
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

#================
# ELECTRONS
#================
EL10 = "(Electrons.pt > 10*GeV && abs(Electrons.eta) < 2.5 && Electrons.DFCommonElectronsLHLoose)"
EL13 = "(Electrons.pt > 13*GeV && abs(Electrons.eta) < 2.5 && Electrons.DFCommonElectronsLHLoose)"
EL15 = "(Electrons.pt > 15*GeV && abs(Electrons.eta) < 2.5 && Electrons.DFCommonElectronsLHLoose)"
EL20 = "(Electrons.pt > 20*GeV && abs(Electrons.eta) < 2.5 && Electrons.DFCommonElectronsLHLoose)"

#================
# MUONS
#================
MU10 = "(Muons.pt > 10*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU13 = "(Muons.pt > 13*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU15 = "(Muons.pt > 15*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"
MU20 = "(Muons.pt > 20*GeV && abs(Muons.eta) < 2.7 && Muons.muonType == 0 && Muons.DFCommonGoodMuon)"


#================
# JETS
#================

akt4EMcalib_15 = "(AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 15*GeV && abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta) < 2.5)"
akt10LCtrimmedcalib_200 = "(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV && abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.5)"

#================
#LargeR jets
#================

largeR_200="("+akt10LCtrimmedcalib_200+")"

#=============================================
# Single LEPTON EVENT SELECTION
#=============================================
skimmingTools_lep=[]

HION6_Selection_lep = "( (count("+MU13+") >= 1) || (count("+EL13+") >= 1) )"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION6StringSkimmingTool_lep = DerivationFramework__xAODStringSkimmingTool(
  name       = "HION6StringSkimmingTool_lep",
  expression = HION6_Selection_lep)

ToolSvc += HION6StringSkimmingTool_lep
skimmingTools_lep.append(HION6StringSkimmingTool_lep)

print "HION6_Selection_lep: ", HION6_Selection_lep
print "HION6StringSkimmingTool_lep: ", HION6StringSkimmingTool_lep

#=============================================
# Di-LEPTON EVENT SELECTION
#=============================================

HION6_LEP10 = "( (count("+EL10+") >= 2) || (count("+MU10+") >= 2) || (count("+EL10+")>= 1 && count("+MU10+") >= 1) )"
HION6_LEP15 = "( (count("+EL15+") >= 2) || (count("+MU15+") >= 2) || (count("+EL15+")>= 1 && count("+MU15+") >= 1) )"
HION6_LEP20 = "( (count("+EL20+") >= 1) || (count("+MU20+") >= 1) )"

HION6_Selection_dilep = "( ("+HION6_LEP15+") || (("+HION6_LEP10+") && ("+HION6_LEP20+")) )"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION6StringSkimmingTool_dilep = DerivationFramework__xAODStringSkimmingTool(
  name       = "HION6StringSkimmingTool_dilep",
  expression = HION6_Selection_dilep)

ToolSvc += HION6StringSkimmingTool_dilep
skimmingTools_lep.append(HION6StringSkimmingTool_dilep)

print "HION6_Selection_dilep: ", HION6_Selection_dilep
print "StringSkimmingTool_lep: ", HION6StringSkimmingTool_dilep

#=============================================
# JET EVENT SELECTION
#=============================================
skimmingTools_jet=[]

HION6_Selection_jet = "( (count("+akt4EMcalib_15+") >= 1) )"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION6StringSkimmingTool_jet = DerivationFramework__xAODStringSkimmingTool(
  name       = "HION6StringSkimmingTool_jet",
  expression = HION6_Selection_jet)

ToolSvc += HION6StringSkimmingTool_jet

skimmingTools_jet.append(HION6StringSkimmingTool_jet)

print "HION6_Selection_jet: ", HION6_Selection_jet
print "StringSkimmingTool_jet: ", HION6StringSkimmingTool_jet

#=============================================
# 4 JET EVENT SELECTION
#=============================================

#HION6_Selection_4jet = "( (count("+akt4EMcalib_15+") >= 4) || (count("+largeR_200+") >= 1) )"
HION6_Selection_4jet = "( (count("+akt4EMcalib_15+") >= 4) )"

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
HION6StringSkimmingTool_4jet = DerivationFramework__xAODStringSkimmingTool(
  name       = "HION6StringSkimmingTool_4jet",
  expression = HION6_Selection_4jet)

ToolSvc += HION6StringSkimmingTool_4jet

skimmingTools_jet.append(HION6StringSkimmingTool_4jet)

print "HION6_Selection_4jet: ", HION6_Selection_4jet
print "StringSkimmingTool_4jet: ", HION6StringSkimmingTool_4jet

#====== Di-Leptons AND Jet selection  ============
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
DiLeptonsANDJets=DerivationFramework__FilterCombinationAND(name="DiLeptonsANDJets",FilterList=[HION6StringSkimmingTool_dilep,HION6StringSkimmingTool_jet])

ToolSvc+=DiLeptonsANDJets

#====== Single-Leptons AND 4 Jets selection  ============
SingleLeptonsANDJets=DerivationFramework__FilterCombinationAND(name = "SingleLeptonsANDJets",FilterList=[HION6StringSkimmingTool_lep,HION6StringSkimmingTool_4jet])

ToolSvc+=SingleLeptonsANDJets

#====== An OR of Both Channels (Single-Leptons AND 4 Jets selection) OR (Di-leptons AND Jet) ============
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
SingleORDileptons=DerivationFramework__FilterCombinationOR(name = "SingleORDileptons",FilterList=[DiLeptonsANDJets,SingleLeptonsANDJets])

ToolSvc+=SingleORDileptons
print "SingleORDileptons : ", SingleORDileptons

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

#  Skim on Single Leptons and 4 jets, DiLeptons and jets, or both the channels by selecting different kernels below
HION6Sequence +=CfgMgr.DerivationFramework__DerivationKernel("SingleORDileptonsKernel", SkimmingTools=[SingleORDileptons])
#HION6Sequence +=CfgMgr.DerivationFramework__DerivationKernel("SingleLeptonsANDJetsKernel", SkimmingTools=[SingleLeptonsANDJets])
#HION6Sequence +=CfgMgr.DerivationFramework__DerivationKernel("DiLeptonsANDJetsKernel", SkimmingTools=[DiLeptonsANDJets])

# Before any custom jet reconstruction, it's good to set up the output list
from DerivationFrameworkJetEtMiss.JetCommon import OutputJets
OutputJets["HION6"] = []

#=======================================
# RESTORE AOD-REDUCED JET COLLECTIONS
#=======================================
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
# Only include those ones that you use. The order in the list is not significant
reducedJetList = ["AntiKt2PV0TrackJets", # This collection will be flavour-tagged automatically
                  "AntiKt4PV0TrackJets",
                  "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList, HION6Sequence, "HION6")

# If you use AntiKt10*PtFrac5SmallR20Jets, these must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(HION6Sequence, "HION6")

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
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit
FlavorTagInit(JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = HION6Sequence)


# Then apply truth tools in the form of aumentation
if DerivationFrameworkHasTruth:
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
