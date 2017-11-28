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
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning

exot2Seq = CfgMgr.AthSequencer("EXOT2Sequence")

#====================================================================
# SET UP STREAM   
#====================================================================
from AthenaServices.Configurables import ThinningSvc, createThinningSvc

streamName = derivationFlags.WriteDAOD_EXOT2Stream.StreamName
fileName   = buildFileName( derivationFlags.WriteDAOD_EXOT2Stream )
EXOT2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
EXOT2Stream.AcceptAlgs(["EXOT2Kernel"])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
#augStream = MSMgr.GetStream( streamName )
#evtStream = augStream.GetEventStream()
#svcMgr += createThinningSvc( svcName="EXOT2ThinningSvc", outStreams=[evtStream] )

#=====================
# TRIGGER NAV THINNING
#=====================

#Establish the thinning helper
from DerivationFrameworkCore.ThinningHelper import ThinningHelper
EXOT2ThinningHelper = ThinningHelper( "EXOT2ThinningHelper" )
#trigger navigation content
EXOT2ThinningHelper.TriggerChains = 'HLT_j.*|HLT_noalg_L1J.*'
EXOT2ThinningHelper.AppendToStream( EXOT2Stream )

#=======================================
# SKIMMING TOOL   
#=======================================
l1triggers = 'L1_J75 || L1_J100'
singlejettriggers = 'HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j100 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460 || HLT_noalg_L1J400'
bjettriggers2015 = 'HLT_j100_2j55_bmedium || HLT_j150_bmedium_j50_bmedium || HLT_j175_bmedium_j60_bmedium || HLT_2j45_bmedium_2j45_L14J15.0ETA25 || HLT_2j35_btight_2j35_L14J15.0ETA25 || HLT_j70_bmedium_3j70_L14J15.0ETA25 || HLT_2j75_bmedium_j75 || HLT_2j70_btight_j70'
#bjettriggers2016 = 'HLT_j150_bmv2c2077_split_j50_bmv2c2077_split || HLT_j175_bmv2c2077_split_j60_bmv2c2077_split || HLT_j225_bmv2c2085_split'
multijettriggers = 'L1_3J40 || L1_3J50 || L1_3J25_0ETA23 || HLT_4j85 || HLT_4j100 || HLT_3j175'
httriggers = 'HLT_ht550 || HLT_ht700 || HLT_ht850 || HLT_ht850_L1J75 || HLT_jt850_L1J100 || HLT_ht1000_L1J100'
expression = '(EventInfo.eventTypeBitmask==1) || ('+l1triggers+' || '+singlejettriggers+' || '+bjettriggers2015+' || '+multijettriggers+' || '+httriggers+')'

from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
EXOT2SkimmingTool = DerivationFramework__xAODStringSkimmingTool(name = "EXOT2SkimmingTool", expression = expression)
ToolSvc += EXOT2SkimmingTool
print EXOT2SkimmingTool

#=======================================
# THINNING
#=======================================

#Truth Thinning: want to keep some parton info so we can
#study BSM decays
thinningTools = []
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

# Further truth slimming to ensure useful parton info
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
EXOT2SlimmingHelper.SmartCollections = ["PrimaryVertices",
                                    "AntiKt4EMTopoJets",
                                    "AntiKt4LCTopoJets",
                                    "BTagging_AntiKt4EMTopo",
                                    "BTagging_AntiKt4LCTopo",
                                       ]
TrigJetCleaningVars = '.ECPSFraction.N90Constituents.LeadingClusterPt.LeadingClusterSecondLambda.LeadingClusterCenterLambda.LeadingClusterSecondR.CentroidR.OotFracClusters5.OotFracClusters10.Timing.GhostTruthAssociationFraction'
TruthAssociationVars = '.GhostTruth.GhostTruthAssociationLink.GhostPartons.GhostPartonsPt.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.GhostTruthCount'
EXOT2SlimmingHelper.ExtraVariables = ["AntiKt4TruthJets.pt.eta.phi.m",
                                      "AntiKt4EMTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m"+TruthAssociationVars+TrigJetCleaningVars,
                                      "AntiKt4LCTopoJets.JetEMScaleMomentum_pt.JetEMScaleMomentum_eta.JetEMScaleMomentum_phi.JetEMScaleMomentum_m"
                                     ]
EXOT2SlimmingHelper.AllVariables = ["TruthEvents",
                                    #"AntiKt4TruthJets",
                                    #"AntiKt4EMTopoJets",
                                    #"AntiKt4LCTopoJets",
                                    #"BTagging_AntiKt4EMTopo",
                                    #"BTagging_AntiKt4LCTopo",
                                    #"MuonSegments",
                                    "TruthParticles",
                                    "LVL1JetRoIs",
                                    "HLT_xAOD__JetContainer_a4tcemsubFS",
                                    "HLT_xAOD__JetContainer_a4tcemsubjesISFS",
                                    "MSDisplacedVertex",
                                    "MSonlyTracklets",
                                    "LVL1MuonRoIs",
                                   ]
EXOT2SlimmingHelper.IncludeJetTriggerContent = True
EXOT2SlimmingHelper.IncludeBJetTriggerContent = True
addMETOutputs(EXOT2SlimmingHelper, ["EXOT2"])
EXOT2SlimmingHelper.AppendContentToStream(EXOT2Stream)
