#====================================================================
# SKIMTEST.py
# This an example job options script showing how to set up a 
# skim of DAOD_PHYS using the derivation framework.  
# It requires the reductionConf flag SKIMTEST in Reco_tf.py
# and inputDAOD_PHYSFile
# and outputD2AODFile   
#====================================================================

# Set up common services and job object. 
# This should appear in ALL derivation job options
from DerivationFrameworkCore.DerivationFrameworkMaster import *

#====================================================================
# SKIMMING TOOLS 
#====================================================================

# Set up your skimming tools (you can have as many as you need). 
# The tools must use the DerivationFrameworkInterfaces as in this example.
from DerivationFrameworkExamples.DerivationFrameworkExamplesConf import DerivationFramework__SkimmingToolExample
SKIMTESTSkimmingTool = DerivationFramework__SkimmingToolExample(   name                    = "SKIMTESTSkimmingTool",
                                                                MuonContainerKey        = "Muons",
                                                                NumberOfMuons           = 3,
                                                                MuonPtCut               = 1000.0)
ToolSvc += SKIMTESTSkimmingTool

#====================================================================
# CREATE THE DERIVATION KERNEL ALGORITHM AND PASS THE ABOVE TOOLS  
#====================================================================

# The name of the kernel (LooseSkimKernel in this case) must be unique to this derivation
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("SKIMTESTKernel",
                                                                        SkimmingTools = [SKIMTESTSkimmingTool]
                                                                      )

#====================================================================
# SET UP STREAM   
#====================================================================

# SKIMMING
# The base name (DAOD_SKIMTEST here) must match the string in 
# DerivationFrameworkProdFlags (in DerivationFrameworkCore) 
streamName = derivationFlags.WriteD2AOD_SKIMTESTStream.StreamName
fileName   = buildFileName( derivationFlags.WriteD2AOD_SKIMTESTStream )
SKIMTESTStream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed below are written out.
# Name must match that of the kernel above
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
SKIMTESTStream.AcceptAlgs(["SKIMTESTKernel"])

#====================================================================
# CONTENTS   
#====================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
SKIMTESTSlimmingHelper = SlimmingHelper("SKIMTESTSlimmingHelper")

SKIMTESTSlimmingHelper.SmartCollections = ["Electrons",
                                       "Photons",
                                       "Muons",
                                       "PrimaryVertices",
                                       "InDetTrackParticles",
                                       "AntiKt4EMTopoJets",
                                       "AntiKt4EMPFlowJets",
                                       "BTagging_AntiKt4EMTopo_201810",
                                       "BTagging_AntiKt4EMPFlow_201810",
                                       "BTagging_AntiKt4EMPFlow_201903",
                                       "MET_Reference_AntiKt4EMTopo",
                                       "MET_Reference_AntiKt4EMPFlow",
                                       "TauJets",
                                       "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
                                       "AntiKt4TruthWZJets",
                                       "AntiKt4TruthJets"
                                      ]

# Truth containers
if DerivationFrameworkHasTruth:
   SKIMTESTSlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                            'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                            'MET_TruthRegions':'xAOD::MissingETContainer','MET_TruthRegionsAux':'xAOD::MissingETAuxContainer',
                                            'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthWbosonWithDecayParticles':'xAOD::TruthParticleContainer','TruthWbosonWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                            'TruthWbosonWithDecayVertices':'xAOD::TruthVertexContainer','TruthWbosonWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                           }

   from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
   addTruth3ContentToSlimmerTool(SKIMTESTSlimmingHelper)


SKIMTESTSlimmingHelper.ExtraVariables = ["AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.D2",
                                     "Electrons.TruthLink",
                                     "Muons.TruthLink",
                                     "Photons.TruthLink",
                                     "AntiKt2PV0TrackJets.pt.eta.phi.m",
                                     "AntiKt4EMTopoJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID",
                                     "AntiKt4EMPFlowJets.DFCommonJets_QGTagger_truthjet_nCharged.DFCommonJets_QGTagger_truthjet_pt.DFCommonJets_QGTagger_truthjet_eta.NumTrkPt500PV.PartonTruthLabelID"]

# Final construction of output stream
SKIMTESTSlimmingHelper.AppendContentToStream(SKIMTESTStream)
