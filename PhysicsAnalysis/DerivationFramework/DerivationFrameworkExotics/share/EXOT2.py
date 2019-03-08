#********************************************************************
# EXOT2.py 
# reductionConf flag EXOT2 in Reco_tf.py   
#********************************************************************
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkJetEtMiss.JetCommon import *
from DerivationFrameworkJetEtMiss.METCommon import *
from DerivationFrameworkEGamma.EGammaCommon import *
from DerivationFrameworkMuons.MuonsCommon import *
from DerivationFrameworkCore.WeightMetadata import *

exot2Seq = CfgMgr.AthSequencer("EXOT2Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
from AthenaServices.Configurables import ThinningSvc, createThinningSvc

streamName = derivationFlags.WriteDAOD_EXOT2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT2Stream )
EXOT2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT2Stream.AcceptAlgs(["EXOT2Kernel"])

#=====================
# TRIGGER NAV THINNING
#=====================

#establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT2ThinningHelper = ThinningHelper( "EXOT2ThinningHelper" )
#trigger navigation content
EXOT2ThinningHelper.AppendToStream( EXOT2Stream )

#=======================================
# SKIMMING TOOL   
#=======================================
l1triggers = 'L1_J75 || L1_J100'
gsctriggers = 'HLT_j15_gsc35_boffperf_split || HLT_j35_gsc55_boffperf_split || HLT_j60_gsc110_boffperf_split || HLT_j175_gsc225_boffperf_split || HLT_j225_gsc275_boffperf_split || HLT_j225_gsc300_boffperf_split || HLT_j225_gsc360_boffperf_split || HLT_j225_gsc380_boffperf_split || HLT_j225_gsc400_boffperf_split || HLT_j225_gsc420_boffperf_split || HLT_j225_gsc440_boffperf_split || HLT_j225_gsc450_boffperf_split'
singlejettriggers = 'HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j100 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460 || HLT_noalg_L1J400 || HLT_j0_perf || HLT_j45 || HLT_j45_L1RD0_FILLED || HLT_j450'
bjettriggers2015 = 'HLT_j100_2j55_bmedium || HLT_j150_bmedium_j50_bmedium || HLT_j175_bmedium_j60_bmedium || HLT_2j45_bmedium_2j45_L14J15.0ETA25 || HLT_2j35_btight_2j35_L14J15.0ETA25 || HLT_j70_bmedium_3j70_L14J15.0ETA25 || HLT_2j75_bmedium_j75 || HLT_2j70_btight_j70'
#bjettriggers2016 = 'HLT_j150_bmv2c2077_split_j50_bmv2c2077_split || HLT_j175_bmv2c2077_split_j60_bmv2c2077_split || HLT_j225_bmv2c2085_split'
multijettriggers = 'L1_3J40 || L1_3J50 || L1_3J25_0ETA23 || HLT_4j85 || HLT_4j100 || HLT_3j175'
httriggers = 'HLT_ht550 || HLT_ht700 || HLT_ht850 || HLT_ht850_L1J75 || HLT_jt850_L1J100 || HLT_ht1000_L1J100'
expression = '(EventInfo.eventTypeBitmask==1) || ('+l1triggers+' || '+gsctriggers+' || '+singlejettriggers+' || '+bjettriggers2015+' || '+multijettriggers+' || '+httriggers+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT2SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT2SkimmingTool", expression = expression)
ToolSvc += EXOT2SkimmingTool
print EXOT2SkimmingTool

#=======================================
# THINNING
#=======================================

thinningTools = []

#truth thinning: want to keep some parton info so we can study BSM decays
if DerivationFrameworkIsMonteCarlo:
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
    EXOT2MCThinningTool = DerivationFramework__MenuTruthThinning(name                = "EXOT2MCThinningTool",
                                                                 ThinningService     = EXOT2ThinningHelper.ThinningSvc(),
                                                                 WriteEverything     = False,
                                                                 WritePartons        = True,
                                                                 WriteHadrons        = True,
                                                                 WriteBHadrons       = True,
                                                                 WriteGeant          = False,
                                                                 WriteTauHad         = False,
                                                                 WriteBSM            = True,
                                                                 WriteBosons         = True,
                                                                 WriteBosonProducts  = True,
                                                                 WriteBSMProducts    = True,
                                                                 WriteTopAndDecays   = False,
                                                                 WriteAllLeptons     = False,
                                                                 WriteStatus3        = False,
                                                                 WriteFirstN         = -1)

    ToolSvc += EXOT2MCThinningTool
    thinningTools += EXOT2MCThinningTool

#further truth thinning to ensure useful parton info
if DerivationFrameworkIsMonteCarlo:
    truth_cond_jets = "(((abs(TruthParticles.pdgId) > 0) && (abs(TruthParticles.pdgId) <= 7) || (abs(TruthParticles.pdgId) == 21)) && (TruthParticles.pt > 1*GeV) && ((TruthParticles.status ==1) || (TruthParticles.status ==2) || (TruthParticles.status ==3) || (TruthParticles.status ==23)) && (TruthParticles.barcode<200000))"
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
    EXOT2TruthTool2 = DerivationFramework__GenericTruthThinning(name                         = "EXOT2TruthTool2",
                                                                ThinningService              = EXOT2ThinningHelper.ThinningSvc(),
                                                                ParticleSelectionString      = truth_cond_jets,
                                                                PreserveDescendants          = False,
                                                                PreserveGeneratorDescendants = True,
                                                                PreserveAncestors            = True)

    ToolSvc += EXOT2TruthTool2
    thinningTools.append(EXOT2TruthTool2)

#=======================================
# JETS
#=======================================

#restore AOD-reduced jet collections
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import replaceAODReducedJets
OutputJets["EXOT2"] = []
reducedJetList = [
    "AntiKt4TruthJets"]
replaceAODReducedJets(reducedJetList,exot2Seq,"EXOT2")

# redo ghost association
from DerivationFrameworkJetEtMiss.ExtendedJetCommon import addJetPtAssociation
if DerivationFrameworkIsMonteCarlo :
    addJetPtAssociation(jetalg="AntiKt4EMTopo",  truthjetalg="AntiKt4TruthJets", sequence=exot2Seq, algname="JetPtAssociationAlg")
    addJetPtAssociation(jetalg="AntiKt4EMPFlow",  truthjetalg="AntiKt4TruthJets", sequence=exot2Seq, algname="JetPtAssociationAlg")

#b-tagging for pflow jets
from DerivationFrameworkFlavourTag.FlavourTagCommon import FlavorTagInit 
FlavorTagInit(JetCollections = ['AntiKt4EMPFlowJets'], Sequencer = exot2Seq) 

#=======================================
# CREATE THE DERIVATION KERNEL ALGORITHM   
#=======================================

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += exot2Seq
exot2Seq += CfgMgr.DerivationFramework__DerivationKernel(name = "EXOT2Kernel",
                                                         SkimmingTools = [EXOT2SkimmingTool],
                                                         ThinningTools = thinningTools)

#====================================================================
# Add the containers to the output stream - slimming done here
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
EXOT2SlimmingHelper = SlimmingHelper("EXOT2SlimmingHelper")
EXOT2SlimmingHelper.SmartCollections = ["AntiKt4EMTopoJets",
                                        "BTagging_AntiKt4EMTopo",
                                        "AntiKt4EMPFlowJets",
                                        "BTagging_AntiKt4EMPFlow"
                                       ]
TrigJetCleaningVars = '.ECPSFraction.N90Constituents.LeadingClusterPt.LeadingClusterSecondLambda.LeadingClusterCenterLambda.LeadingClusterSecondR.CentroidR.OotFracClusters5.OotFracClusters10.Timing.GhostTruthAssociationFraction'
TruthAssociationVars = '.GhostTruth.GhostTruthAssociationLink.GhostPartons.GhostPartonsPt.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruthCount'

EXOT2SlimmingHelper.ExtraVariables = ["AntiKt4TruthJets.pt.eta.phi.m","AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m"+TruthAssociationVars+TrigJetCleaningVars,"PrimaryVertices.trackParticleLinks.x.y.z.vertexType.neutralParticleLinks"
                                      ]

EXOT2SlimmingHelper.AllVariables = ["TruthEvents",
                                    "TruthParticles",
                                    "LVL1JetRoIs",
                                    "HLT_xAOD__JetContainer_a4tcemsubFS",
                                    "HLT_xAOD__JetContainer_a4tcemsubjesISFS",
                                    "HLT_xAOD__JetContainer_GSCJet",
                                   ]
#PFlow b-tagging
EXOT2SlimmingHelper.AppendToDictionary = {'BTagging_AntiKt4EMPFlow':'xAOD::BTaggingContainer','BTagging_AntiKt4EMPFlowAux':'xAOD::BTaggingAuxContainer'}

EXOT2SlimmingHelper.AppendContentToStream(EXOT2Stream)

EXOT2Stream.RemoveItem("xAOD::TrigNavigation#*")
EXOT2Stream.RemoveItem("xAOD::TrigNavigationAuxInfo#*")

EXOT2Stream.RemoveItem("HLT_xAOD__JetContainer_a10ttclcwjesFS")
EXOT2Stream.RemoveItem("HLT_xAOD__JetContainer_a10r_tcemsubjesFS")
EXOT2Stream.RemoveItem("HLT_xAOD__JetContainer_a10tclcwsubFS")
EXOT2Stream.RemoveItem("HLT_xAOD__JetContainer_a10r_tcemsubjesISFS")
EXOT2Stream.RemoveItem("HLT_xAOD__JetContainer_a10tclcwsubjesFS")
EXOT2Stream.RemoveItem("HLT_xAOD__JetContainer_EFJet")
EXOT2Stream.RemoveItem("HLT_xAOD__JetContainer_a4tcemsubFS")

