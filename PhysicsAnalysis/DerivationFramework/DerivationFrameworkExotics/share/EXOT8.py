#********************************************************************
# EXOT8.py
# reductionConf flag EXOT8 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon            import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon    import *
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import (
    addCSSKSoftDropJets)
from DerivationFrameworkJetEtMiss.METCommon            import *
from DerivationFrameworkEGamma.EGammaCommon            import *
from DerivationFrameworkMuons.MuonsCommon              import *
from DerivationFrameworkFlavourTag.HbbCommon           import *
from DerivationFrameworkCore.WeightMetadata            import *
from DerivationFrameworkFlavourTag.FlavourTagCommon    import FlavorTagInit
from DerivationFrameworkFlavourTag.FlavourTagCommon    import applyBTagging_xAODColl
from JetRec.JetRecFlags import jetFlags
from JetRec.JetRecConf import JetAlgorithm
from DerivationFrameworkJetEtMiss.DFJetMetFlags        import *
from DerivationFrameworkJetEtMiss.DerivationFrameworkJetEtMissConf import DerivationFramework__JetDecorAlg

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__TriggerSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationAND
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__FilterCombinationOR
from DerivationFrameworkCore.DerivationFrameworkCoreConf   import DerivationFramework__DerivationKernel

#========================================================================================================================================
# Set up Stream
#========================================================================================================================================
streamName  = derivationFlags.WriteDAOD_EXOT8Stream.StreamName
fileName    = buildFileName( derivationFlags.WriteDAOD_EXOT8Stream )
EXOT8Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT8Stream.AcceptAlgs(["EXOT8Kernel"])

#
#  Trigger Nav thinning
#
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT8ThinningHelper = ThinningHelper( "EXOT8ThinningHelper" )
if globalflags.DataSource() is not "geant4":
    EXOT8ThinningHelper.TriggerChains = 'HLT_.*gsc.*|HLT_.*bmv2c.*|HLT_e.*|HLT_mu.*|HLT_j.*a10.*'
EXOT8ThinningHelper.AppendToStream( EXOT8Stream )

#========================================================================================================================================
# Thinning Tools
#========================================================================================================================================
thinningTools=[]

#########################################
# Tracks associated with Muons
#########################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__MuonTrackParticleThinning
EXOT8MuonTPThinningTool = DerivationFramework__MuonTrackParticleThinning(name                   = "EXOT8MuonTPThinningTool",
                                                                         ThinningService        = EXOT8ThinningHelper.ThinningSvc(),
                                                                         MuonKey                = "Muons",
                                                                         InDetTrackParticlesKey = "InDetTrackParticles",
                                                                         SelectionString        = "Muons.pt > 5*GeV",
                                                                         ConeSize               = 0)
ToolSvc += EXOT8MuonTPThinningTool
thinningTools.append(EXOT8MuonTPThinningTool)

#########################################
# Tracks associated with Electrons
#########################################
from DerivationFrameworkInDet.DerivationFrameworkInDetConf import DerivationFramework__EgammaTrackParticleThinning
EXOT8ElectronTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                   = "EXOT8ElectronTPThinningTool",
                                                                                ThinningService        = EXOT8ThinningHelper.ThinningSvc(),
                                                                                SGKey                  = "Electrons",
                                                                                InDetTrackParticlesKey = "InDetTrackParticles",
                                                                                SelectionString        = "Electrons.pt > 25*GeV", # Remove tracks with high pt cut
                                                                                ConeSize               = 0)
ToolSvc += EXOT8ElectronTPThinningTool
thinningTools.append(EXOT8ElectronTPThinningTool)

########################################## 
#Tracks associated with Photons
########################################## 
EXOT8PhotonTPThinningTool = DerivationFramework__EgammaTrackParticleThinning(name                    = "EXOT8PhotonTPThinningTool",
                                                                             ThinningService         = EXOT8ThinningHelper.ThinningSvc(),
                                                                             SGKey                   = "Photons",
                                                                             InDetTrackParticlesKey  = "InDetTrackParticles")

ToolSvc += EXOT8PhotonTPThinningTool
thinningTools.append(EXOT8PhotonTPThinningTool)


# Thin ak4 jets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT8ak4ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak4ThinningTool",
                                                                  ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                  ContainerName   = "AntiKt4EMTopoJets",
                                                                  ApplyAnd        = False,
                                                                  SelectionString = "AntiKt4EMTopoJets.DFCommonJets_Calib_pt > 20*GeV")
ToolSvc += EXOT8ak4ThinningTool
thinningTools.append(EXOT8ak4ThinningTool)

# Thin ak10 jets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT8ak10ThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak10ThinningTool",
                                                                   ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                   ContainerName   = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                                                   ApplyAnd        = False,
                                                                   SelectionString = "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV")
ToolSvc += EXOT8ak10ThinningTool
thinningTools.append(EXOT8ak10ThinningTool)


# Thin ak10 untrimmed jets
from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__GenericObjectThinning
EXOT8ak10UntrimmedThinningTool = DerivationFramework__GenericObjectThinning(name            = "EXOT8ak10UntrimmedThinningTool",
                                                                             ThinningService = EXOT8ThinningHelper.ThinningSvc(),
                                                                             ContainerName   = "AntiKt10LCTopoJets",
                                                                             ApplyAnd        = False,
                                                                             SelectionString = "AntiKt10LCTopoJets.pt > 150*GeV")
ToolSvc += EXOT8ak10UntrimmedThinningTool
thinningTools.append(EXOT8ak10UntrimmedThinningTool)


#########################################
# truth thinning
#########################################
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
EXOT8TruthTool = DerivationFramework__MenuTruthThinning(name                         = "EXOT8TruthTool",
                                                        ThinningService              = EXOT8ThinningHelper.ThinningSvc(),
                                                        WritePartons                 = False,
                                                        WriteHadrons                 = False,
                                                        WriteBHadrons                = True,
                                                        WriteGeant                   = False,
                                                        GeantPhotonPtThresh          = -1.0,
                                                        WriteTauHad                  = False,
                                                        PartonPtThresh               = -1.0,
                                                        WriteBSM                     = True,
                                                        WriteBosons                  = True,
                                                        WriteBSMProducts             = True,
                                                        WriteBosonProducts           = False,
                                                        WriteTopAndDecays            = True,
                                                        WriteEverything              = False,
                                                        WriteAllLeptons              = True,
                                                        WriteStatus3                 = True,
                                                        PreserveGeneratorDescendants = True,
                                                        WriteFirstN                  = -1)

if globalflags.DataSource()=="geant4":
    ToolSvc += EXOT8TruthTool
    thinningTools.append(EXOT8TruthTool)

#====================================================================
# AUGMENTATION TOOLS
#====================================================================
augmentationTools = []
from DerivationFrameworkExotics.DerivationFrameworkExoticsConf import DerivationFramework__BJetRegressionVariables
EXOT8BJetRegressionVariables_EMTopo = DerivationFramework__BJetRegressionVariables(name = "EXOT8BJetRegressionVariables_EMTopo",
                                                                                   ContainerName = "AntiKt4EMTopoJets",
                                                                                   AssociatedTracks = "GhostTrack",
                                                                                   MinTrackPtCuts = [0])
EXOT8BJetRegressionVariables_EMPFlow = DerivationFramework__BJetRegressionVariables(name = "EXOT8BJetRegressionVariables_EMPFlow",
                                                                                    ContainerName = "AntiKt4EMPFlowJets",
                                                                                    AssociatedTracks = "GhostTrack",
                                                                                    MinTrackPtCuts = [0])

ToolSvc += EXOT8BJetRegressionVariables_EMTopo
augmentationTools.append(EXOT8BJetRegressionVariables_EMTopo)
ToolSvc += EXOT8BJetRegressionVariables_EMPFlow
augmentationTools.append(EXOT8BJetRegressionVariables_EMPFlow)
#print EXOT8BJetRegressionVariables

triggers_for_matching = ['HLT_g60_loose',
                         'HLT_g140_loose',
                         'HLT_g160_loose'
                         ]

from DerivationFrameworkCore.TriggerMatchingAugmentation import applyTriggerMatching
TrigMatchAug, NewTrigVars = applyTriggerMatching(ToolNamePrefix="EXOT6",
                                                 PhotonTriggers=triggers_for_matching)

augmentationTools.append(TrigMatchAug)
   
#========================================================================================================================================
# Triggers (https://indico.cern.ch/event/403233/contribution/4/material/slides/0.pdf)
#
# Not yet known: 25ns menu, L1_HT menu
#========================================================================================================================================
triggers = ["L1_J50",
            "L1_J75",
            "L1_J100",
            "L1_J120",
            "L1_2J50_XE40",

            "L1_3J40",
            "L1_3J50",
            "L1_3J75",

            "L1_4J15",
            "L1_4J20",
            "L1_3J25.0ETA23",
            "L1_4J15.0ETA25",
            "L1_J75_3J20",
            "L1_J85_3J30",
            "L1_3J35.0ETA23",
            "L1_HT190-J15s5.ETA21",
            # Single Leptons
            "L1_EM22VHI",
            "L1_EM24VHI",
            "L1_EM26VHI",
            "L1_MU20",            
            ]

trigger_photon = "HLT_g140_loose"

#========================================================================================================================================
# Event Skimming
#
# https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BTaggingBenchmarks
#========================================================================================================================================

# $equire 2 b-tags from the same algorithm
b2tag_EMTopo_MV2c10_Fixed    = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_MV2c10) >= 2"
b2tag_EMTopo_MV2r_Fixed   = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_MV2r)   >= 2"
b2tag_EMTopo_MV2rmu_Fixed = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_MV2rmu) >= 2"
b2tag_EMTopo_MV2          = "(%s || %s || %s)" % (b2tag_EMTopo_MV2c10_Fixed,b2tag_EMTopo_MV2r_Fixed,b2tag_EMTopo_MV2rmu_Fixed)

b2tag_EMTopo_DL1_Fixed       = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_DL1)    >= 2"
b2tag_EMTopo_DL1r_Fixed      = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_DL1r)   >= 2"
b2tag_EMTopo_DL1rmu_Fixed    = "count(AntiKt4EMTopoJets.DFCommonJets_FixedCutBEff_77_DL1rmu) >= 2"
b2tag_EMTopo_DL1             = "(%s || %s || %s)" % (b2tag_EMTopo_DL1_Fixed,b2tag_EMTopo_DL1r_Fixed,b2tag_EMTopo_DL1rmu_Fixed)

b2tag77_EMTopo         = "(%s || %s)" % (b2tag_EMTopo_MV2, b2tag_EMTopo_DL1)


b2tag_EMPFlow_MV2c10_Fixed = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_MV2c10)    >= 2"
b2tag_EMPFlow_MV2r_Fixed   = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_MV2r)   >= 2"
b2tag_EMPFlow_MV2rmu_Fixed = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_MV2rmu) >= 2"
b2tag_EMPFlow_MV2          = "(%s || %s || %s)" % (b2tag_EMPFlow_MV2c10_Fixed,b2tag_EMPFlow_MV2r_Fixed,b2tag_EMPFlow_MV2rmu_Fixed)

b2tag_EMPFlow_DL1_Fixed       = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_DL1)    >= 2"
b2tag_EMPFlow_DL1r_Fixed      = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_DL1r)   >= 2"
b2tag_EMPFlow_DL1rmu_Fixed    = "count(AntiKt4EMPFlowJets.DFCommonJets_FixedCutBEff_77_DL1rmu) >= 2"
b2tag_EMPFlow_DL1             = "(%s || %s || %s)" % (b2tag_EMPFlow_DL1_Fixed,b2tag_EMPFlow_DL1r_Fixed,b2tag_EMPFlow_DL1rmu_Fixed)

b2tag77_EMPFlow         = "(%s || %s)" % (b2tag_EMPFlow_MV2, b2tag_EMPFlow_DL1)

# jet skimming
resolved_4jetsEMTopo  = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt  > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)  < 2.8)) >= 4 && %s" % b2tag77_EMTopo
resolved_4jetsEMPFlow = "count((AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8)) >= 4 && %s" % b2tag77_EMPFlow
resolved_4jet         = "(%s || %s)" % (resolved_4jetsEMTopo, resolved_4jetsEMPFlow)

resolved_2jetsEMTopo  = "count((AntiKt4EMTopoJets.DFCommonJets_Calib_pt  > 25*GeV) && (abs(AntiKt4EMTopoJets.DFCommonJets_Calib_eta)  < 2.8)) >= 2 && %s" % b2tag77_EMTopo
resolved_2jetsEMPFlow = "count((AntiKt4EMPFlowJets.DFCommonJets_Calib_pt > 25*GeV) && (abs(AntiKt4EMPFlowJets.DFCommonJets_Calib_eta) < 2.8)) >= 2 && %s" % b2tag77_EMPFlow
resolved_2jet         = "(%s || %s)" % (resolved_2jetsEMTopo, resolved_2jetsEMPFlow)

singleElectron       = "count((Electrons.Tight)        && (Electrons.pt > 25*GeV) && (abs(Electrons.eta) < 2.5)) >= 1"
singleMuon           = "count((Muons.DFCommonGoodMuon) && (Muons.pt     > 25*GeV) && (abs(Muons.eta)     < 2.5)) >= 1"
singleLepton         = "(%s) || (%s)" % (singleElectron, singleMuon)

boosted_1LargeR      = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 200*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"
boosted_1LargeR_photon = "count((AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_pt > 150*GeV) && (abs(AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.DFCommonJets_Calib_eta) < 2.4)) >= 1"

photon               = "count(Photons.pt > 130*GeV) >= 1"

eventSkim_zeroLepton   = "((%s) || (%s))" % (resolved_4jet, boosted_1LargeR)
eventSkim_singleLepton = "((%s) && (%s))" % (singleLepton, resolved_2jet)
eventSkim_photonSel = "((%s) && (%s))" % (photon, boosted_1LargeR_photon)

preSkim_photon = "((%s) && (%s))" % (trigger_photon, photon) 

#------------------------------------------
#pre-skimming tools

#trigger
EXOT8TriggerPreSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT8TriggerPreSkimmingTool",
                                                                       TriggerListAND = [],
                                                                       TriggerListOR  = triggers)
ToolSvc += EXOT8TriggerPreSkimmingTool

#1 lepton
EXOT8PreSkimmingTool_sl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8PreSkimmingTool_sl", expression = singleLepton)
ToolSvc += EXOT8PreSkimmingTool_sl

#photon trigger + photon
EXOT8PreSkimmingTool_photon = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8PreSkimmingTool_photon", expression = preSkim_photon)
ToolSvc += EXOT8PreSkimmingTool_photon

#------------------------------------------
#skimming tools

#trigger
EXOT8TriggerSkimmingTool = DerivationFramework__TriggerSkimmingTool(name = "EXOT8TriggerSkimmingTool",
                                                                    TriggerListAND = [],
                                                                    TriggerListOR  = triggers)
ToolSvc += EXOT8TriggerSkimmingTool

#1 lepton
EXOT8SkimmingTool_sl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_sl", expression = eventSkim_singleLepton)
ToolSvc += EXOT8SkimmingTool_sl

#0 lepton
EXOT8SkimmingTool_zl = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_zl", expression = eventSkim_zeroLepton)
ToolSvc += EXOT8SkimmingTool_zl

#photon trigger
EXOT8SkimmingTool_ph_trig = DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_ph_trig", expression = trigger_photon)
ToolSvc += EXOT8SkimmingTool_ph_trig

#photon + large-R
EXOT8SkimmingTool_ph =  DerivationFramework__xAODStringSkimmingTool(name = "EXOT8SkimmingTool_ph", expression = eventSkim_photonSel)
ToolSvc += EXOT8SkimmingTool_ph

#------------------------------------------
#pre-skimming tools combinations

#trigger || 1 lepton || (photon trigger && photon)
EXOT8PreSkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT8PreSkimmingTool", FilterList = [EXOT8TriggerPreSkimmingTool, EXOT8PreSkimmingTool_sl, EXOT8PreSkimmingTool_photon])
ToolSvc += EXOT8PreSkimmingTool

#------------------------------------------
#skimming tools combinations

#trigger && 0 lepton
EXOT8ANDSkimmingTool_zl = DerivationFramework__FilterCombinationAND(name = "EXOT8ANDSkimmingTool_zl", FilterList = [EXOT8TriggerSkimmingTool, EXOT8SkimmingTool_zl])
ToolSvc += EXOT8ANDSkimmingTool_zl

#photon trigger && 1 photon
EXOT8ANDSkimmingTool_photon = DerivationFramework__FilterCombinationAND(name = "EXOT8ANDSkimmingTool_photon", FilterList = [EXOT8SkimmingTool_ph_trig, EXOT8SkimmingTool_ph])
ToolSvc += EXOT8ANDSkimmingTool_photon

#(trigger && 0 lepton) || 1 lepton || (trigger photon + photon + large-R)
EXOT8SkimmingTool = DerivationFramework__FilterCombinationOR(name = "EXOT8SkimmingTool", FilterList = [EXOT8ANDSkimmingTool_zl, EXOT8SkimmingTool_sl, EXOT8ANDSkimmingTool_photon])
ToolSvc += EXOT8SkimmingTool


#=======================================
# Create private sequences
# Create the derivation kernel algorithm and pass the above skimming, thinning and augmentation tools
#=======================================
#this pre-sequence skims events based on what is available from the input xAOD, thus reducing the use of CPU resources
exot8PreSeq = CfgMgr.AthSequencer("EXOT8PreSequence")
exot8PreSeq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8PreKernel_skim", SkimmingTools = [EXOT8PreSkimmingTool])
DerivationFrameworkJob += exot8PreSeq


#the main sequence tools will be passed after jets have been reconstructed
exot8Seq = CfgMgr.AthSequencer("EXOT8Sequence")
exot8PreSeq += exot8Seq

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT8"] = []
reducedJetList = [
    "AntiKt2PV0TrackJets", #flavour-tagged automatically
    "AntiKt4PV0TrackJets",
    "AntiKt4TruthJets",
    "AntiKt4TruthWZJets",
    "AntiKt10TruthJets",
    "AntiKt10LCTopoJets"]
replaceAODReducedJets(reducedJetList,exot8Seq,"EXOT8")

#AntiKt10*PtFrac5SmallR20Jets must be scheduled *AFTER* the other collections are replaced
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addDefaultTrimmedJets
addDefaultTrimmedJets(exot8Seq,"EXOT8")

# Adds a new jet collection for SoftDrop with constituent modifiers CS and SK applied
if globalflags.DataSource()=="geant4":
  addSoftDropJets('AntiKt', 1.0, 'Truth', beta=1.0, zcut=0.1, mods="truth_groomed", algseq=exot8Seq, outputGroup="EXOT8", writeUngroomed=True)

addCSSKSoftDropJets(exot8Seq, "EXOT8")

# Create variable-R trackjets and dress AntiKt10LCTopo with ghost VR-trkjet 
addVRJets(exot8Seq)
# Also add Hbb Tagger
addHbbTagger(exot8Seq, ToolSvc)
addHbbTagger(exot8Seq, ToolSvc,nn_file_name="BoostedJetTaggers/HbbTagger/Summer2018/MulticlassNetwork.json",nn_config_file="BoostedJetTaggers/HbbTaggerDNN/MulticlassConfigJune2018.json")


#b-tagging

# use alias for VR jets
from BTagging.BTaggingFlags import BTaggingFlags
BTaggingFlags.CalibrationChannelAliases += ["AntiKtVR30Rmax4Rmin02Track->AntiKtVR30Rmax4Rmin02Track,AntiKt4EMTopo"]

#tag pFlow jets
FlavorTagInit(scheduleFlipped = False, JetCollections  = ['AntiKt4EMPFlowJets'], Sequencer = exot8Seq)

#====================================================================
# Apply jet calibration
#====================================================================
applyJetCalibration_xAODColl("AntiKt4EMTopo", exot8Seq)
updateJVT_xAODColl("AntiKt4EMTopo", exot8Seq)
applyBTagging_xAODColl("AntiKt4EMTopo",exot8Seq)

applyJetCalibration_xAODColl("AntiKt4EMPFlow", exot8Seq)
updateJVT_xAODColl("AntiKt4EMPFlow", exot8Seq)
applyBTagging_xAODColl("AntiKt4EMPFlow", exot8Seq)

applyJetCalibration_CustomColl("AntiKt10LCTopoTrimmedPtFrac5SmallR20", exot8Seq)


#######################
# Get the jet pull
#######################
jtm.modifiersMap["jetmomcopy"] = [jtm.pull] # Add jet pull to the jtm modifiers map.
jetmomcopy_EMTopo = jtm.addJetCopier("DFJetMomentsCopy_AntiKt4EMTopoJets", "AntiKt4EMTopoJets", "jetmomcopy") # Output, Input, modifier map index. Makes jetRecTool.g
exot8Seq += JetAlgorithm("jetalgDFJetMomentsCopy_AntiKt4EMTopoJets", Tools = [jetmomcopy_EMTopo] ) # Run this tool

## Schedule the JetDecorAlg which copies jet moments back to the original collection
exot8Seq += DerivationFramework__JetDecorAlg("DecorJet_EMTopo")
exot8Seq.DecorJet_EMTopo.InputCollection="DFJetMomentsCopy_AntiKt4EMTopoJets" # Which collection to copy moments FROM
exot8Seq.DecorJet_EMTopo.MomentsToCopy=[
    "float#PullMag@PullMag",   "float#PullPhi@PullPhi",   "float#Pull_C00@Pull_C00",
    "float#Pull_C01@Pull_C01", "float#Pull_C10@Pull_C10", "float#Pull_C11@Pull_C11"]
jtm.modifiersMap["jetmomcopy"] = [jtm.pull] # Add jet pull to the jtm modifiers map.
jetmomcopy_EMPFlow = jtm.addJetCopier("DFJetMomentsCopy_AntiKt4EMPFlowJets", "AntiKt4EMPFlowJets", "jetmomcopy") # Output, Input, modifier map index. Makes jetRecTool.g
exot8Seq += JetAlgorithm("jetalgDFJetMomentsCopy_AntiKt4EMPFlowJets", Tools = [jetmomcopy_EMPFlow] ) # Run this tool

### Schedule the JetDecorAlg which copies jet moments back to the original collection
exot8Seq += DerivationFramework__JetDecorAlg("DecorJet_EMPFlow")
exot8Seq.DecorJet_EMPFlow.InputCollection="DFJetMomentsCopy_AntiKt4EMPFlowJets" # Which collection to copy moments FROM
exot8Seq.DecorJet_EMPFlow.MomentsToCopy=[
    "float#PullMag@PullMag",   "float#PullPhi@PullPhi",   "float#Pull_C00@Pull_C00",
    "float#Pull_C01@Pull_C01", "float#Pull_C10@Pull_C10", "float#Pull_C11@Pull_C11"]



exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel_skim",SkimmingTools = [EXOT8SkimmingTool])
exot8Seq += CfgMgr.DerivationFramework__DerivationKernel("EXOT8Kernel", ThinningTools = thinningTools,
                                                                        AugmentationTools = augmentationTools)




# Add the containers to the output stream - slimming done here
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT8SlimmingHelper = SlimmingHelper("EXOT8SlimmingHelper")

EXOT8SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "BTagging_AntiKt2Track",
                                        "BTagging_AntiKtVR30Rmax4Rmin02TrackGhostTag",
                                        "PrimaryVertices",
                                        "Electrons",
                                        "Muons",
                                        "Photons",
                                        "MET_Reference_AntiKt4EMTopo",
                                        "AntiKt4TruthJets",
                                        "AntiKt4TruthWZJets",
                                        "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                        "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
                                        "HLT_xAOD__BTaggingContainer_HLTBjetFex",
                                        "InDetTrackParticles",
                                        "AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMPFlow",
                                        "MET_Reference_AntiKt4EMPFlow",
                                        ]

EXOT8SlimmingHelper.ExtraVariables = ["Electrons.charge", 
                                      "Muons.charge", 
                                      "AntiKt4EMTopoJets.DFCommonJets_TrackSumMass",
                                      "AntiKt4EMTopoJets.DFCommonJets_TrackSumPt",
                                      "AntiKt4EMTopoJets.TrackSumPt",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkPt0",
                                      "AntiKt4EMTopoJets.VecSumPtTrkPt0",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkCleanPt0",
                                      "AntiKt4EMTopoJets.ScalSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMTopoJets.VecSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMTopoJets.PullMag",
                                      "AntiKt4EMTopoJets.PullPhi",
                                      "AntiKt4EMTopoJets.Pull_C00",
                                      "AntiKt4EMTopoJets.Pull_C01",
                                      "AntiKt4EMTopoJets.Pull_C10",
                                      "AntiKt4EMTopoJets.Pull_C11",
                                      "AntiKt4EMPFlowJets.ScalSumPtTrkPt0",
                                      "AntiKt4EMPFlowJets.VecSumPtTrkPt0",
                                      "AntiKt4EMPFlowJets.ScalSumPtTrkCleanPt0",
                                      "AntiKt4EMPFlowJets.ScalSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMPFlowJets.VecSumPtTrkCleanPt0PV0",
                                      "AntiKt4EMPFlowJets.PullMag",
                                      "AntiKt4EMPFlowJets.PullPhi",
                                      "AntiKt4EMPFlowJets.Pull_C00",
                                      "AntiKt4EMPFlowJets.Pull_C01",
                                      "AntiKt4EMPFlowJets.Pull_C10",
                                      "AntiKt4EMPFlowJets.Pull_C11",
                                      "BTagging_AntiKt4EMTopo.JetVertexCharge_discriminant",
                                      "BTagging_AntiKt4EMTopo.SV1_normdist",
                                      "BTagging_AntiKt4EMTopo.SV1_masssvx",
                                      "BTagging_AntiKt2Track.JetVertexCharge_discriminant",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.GhostTrackCount",
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.Tau1",                                      
                                      "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.Tau2",                                      
                                      "Photons."+NewTrigVars["Photons"]
                                      ]

EXOT8SlimmingHelper.AllVariables   = ["TruthParticles",
                                      "TruthEvents",
                                      "TruthVertices",
                                      "AntiKt10LCTopoJets",
                                      "AntiKt2PV0TrackJets",
                                      "AntiKt10TruthJets",
                                      "CombinedMuonTrackParticles",
                                      "ExtrapolatedMuonTrackParticles",
                                      ]
                                      
EXOT8SlimmingHelper.StaticContent = [
                                     "xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
                                     "xAOD::JetContainer#AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsAux.",
                                     "xAOD::JetContainer#AntiKt10LCTopoCSSKJets",
                                     "xAOD::JetAuxContainer#AntiKt10LCTopoCSSKJetsAux.",
                                    ]

# Add VR track-jet collection and its b-tagging container to output stream
EXOT8SlimmingHelper.AppendToDictionary = {
    "AntiKtVR30Rmax4Rmin02TrackJets"                 :   "xAOD::JetContainer"        ,
    "AntiKtVR30Rmax4Rmin02TrackJetsAux"              :   "xAOD::JetAuxContainer"     ,
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets"    :   "xAOD::JetContainer"        ,
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10JetsAux" :   "xAOD::JetAuxContainer"     ,
    "BTagging_AntiKtVR30Rmax4Rmin02Track"            :   "xAOD::BTaggingContainer"   ,
    "BTagging_AntiKtVR30Rmax4Rmin02TrackAux"         :   "xAOD::BTaggingAuxContainer",
    "BTagging_AntiKt4EMPFlow"                        :   "xAOD::BTaggingContainer"   , 
    "BTagging_AntiKt4EMPFlowAux"                     :   "xAOD::BTaggingAuxContainer",
}
                               
# Add all variabless for VR track-jets
EXOT8SlimmingHelper.AllVariables  += ["AntiKtVR30Rmax4Rmin02TrackJets"]
EXOT8SlimmingHelper.SmartCollections  += ["BTagging_AntiKtVR30Rmax4Rmin02Track"]

# Save certain b-tagging variables for VR track-jet
EXOT8SlimmingHelper.ExtraVariables += [
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_pb.SV1_pu.IP3D_pb.IP3D_pu",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.MV2c100_discriminant",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.SV1_badTracksIP.SV1_vertices.BTagTrackToJetAssociator.MSV_vertices",
    "BTagging_AntiKtVR30Rmax4Rmin02Track.BTagTrackToJetAssociatorBB.JetFitter_JFvertices.JetFitter_tracksAtPVlinks.MSV_badTracksIP",
]
EXOT8SlimmingHelper.ExtraVariables += [
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.HbbScore",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.XbbScoreHiggs.XbbScoreTop.XbbScoreQCD"
]


if globalflags.DataSource()=="geant4":
    EXOT8SlimmingHelper.StaticContent += [
                                     "xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
                                     "xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.",
                                     ]
    EXOT8SlimmingHelper.AppendToDictionary = {
      "AntiKt10TruthSoftDropBeta100Zcut10Jets"   :   "xAOD::JetContainer"        ,
      "AntiKt10TruthSoftDropBeta100Zcut10JetsAux":   "xAOD::JetAuxContainer"        ,
    }
    EXOT8SlimmingHelper.AllVariables  += ["AntiKt10TruthSoftDropBeta100Zcut10Jets"]

EXOT8SlimmingHelper.IncludeJetTriggerContent = True
EXOT8SlimmingHelper.IncludeBJetTriggerContent = True
EXOT8SlimmingHelper.IncludeEGammaTriggerContent = True
EXOT8SlimmingHelper.IncludeMuonTriggerContent = True
EXOT8SlimmingHelper.AppendContentToStream(EXOT8Stream)
