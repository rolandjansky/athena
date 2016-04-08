#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkMCTruth.MCTruthCommon import * 

#====================================================================
# JET/MET
#====================================================================
# Set jet flags
from JetRec.JetRecFlags import jetFlags
jetFlags.useTruth = True
jetFlags.useTracks = False
# Add jet algorithms
from JetRec.JetAlgorithm import addJetRecoToAlgSequence
addJetRecoToAlgSequence(DerivationFrameworkJob,eventShapeTools=None)
from JetRec.JetRecStandard import jtm
from JetRec.JetRecConf import JetAlgorithm
jetFlags.truthFlavorTags = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                            "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                            "TausFinal",
                            "Partons",
                            ]
# Standard truth jets
# To recover jet constituents remove the last modifier.
akt4 = jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit], ptmin= 20000)
akt4alg = JetAlgorithm("jetalgAntiKt4TruthJets", Tools = [akt4] )
DerivationFrameworkJob += akt4alg

#Large R jets
akt10 = jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth", ptmin= 100000)
akt10alg = JetAlgorithm("jetalgAntiKt10TruthJets", Tools = [akt10] )
DerivationFrameworkJob += akt10alg
akt10trim = jtm.addJetTrimmer("TrimmedAntiKt10TruthJets", rclus=0.3, ptfrac=0.05, input='AntiKt10TruthJets')
akt10trimalg = JetAlgorithm("jetalgTrimmedAntiKt10TruthJets", Tools = [akt10trim] )
DerivationFrameworkJob += akt10trimalg

# Add truth-based MET algorithm here
import METReconstruction.METConfig_Truth
from METReconstruction.METRecoFlags import metFlags # not sure if you even need this line
from METReconstruction.METRecoConfig import getMETRecoAlg
metAlg = getMETRecoAlg('METReconstruction')
DerivationFrameworkJob += metAlg

#==============================================================================
# Set-up of the tols for writing special collections and decorators
#==============================================================================

from DerivationFrameworkMCTruth.TruthObjectTools import *
from DerivationFrameworkMCTruth.TruthDecoratorTools import *

#==============================================================================
# Truth Collections
#==============================================================================

from DerivationFrameworkSUSY.DerivationFrameworkSUSYConf import DerivationFramework__Truth3CollectionMaker

TRUTH3MuonTool = DerivationFramework__Truth3CollectionMaker(name                   = "TRUTH3MuonTool",
                                                         NewCollectionName       = "TruthMuons",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 13 && TruthParticles.status==1")
ToolSvc += TRUTH3MuonTool
TRUTH3ElectronTool = DerivationFramework__Truth3CollectionMaker(name               = "TRUTH3ElectronTool",
                                                         NewCollectionName       = "TruthElectrons",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 11 && TruthParticles.status==1")
ToolSvc += TRUTH3ElectronTool

TRUTH3PhotonTool = DerivationFramework__Truth3CollectionMaker(name                 = "TRUTH3PhotonTool",
                                                         NewCollectionName       = "TruthPhotons",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 22 && TruthParticles.pt > 50.*GeV")
ToolSvc += TRUTH3PhotonTool
TRUTH3TauTool = DerivationFramework__Truth3CollectionMaker(name                    = "TRUTH3TauTool",
                                                         NewCollectionName       = "TruthTaus",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 15")
ToolSvc += TRUTH3TauTool

TRUTH3BosonTool = DerivationFramework__Truth3CollectionMaker(name                    = "TRUTH3BosonTool",
                                                         NewCollectionName       = "TruthBoson",
                                                         ParticleSelectionString = "(abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) == 25 || abs(TruthParticles.pdgId) == 23) && TruthParticles.m > 20.*GeV")
ToolSvc += TRUTH3BosonTool

TRUTH3TopTool = DerivationFramework__Truth3CollectionMaker(name                    = "TRUTH3TopTool",
                                                         NewCollectionName       = "TruthTop",
                                                         ParticleSelectionString = "abs(TruthParticles.pdgId) == 6");
ToolSvc += TRUTH3TopTool

TRUTH3BSMTool = DerivationFramework__Truth3CollectionMaker(name                    = "TRUTH3BSMTool",
                                                         NewCollectionName       = "TruthBSM",
                                                         ParticleSelectionString = "((abs(TruthParticles.pdgId) >= 1000001) && (abs(TruthParticles.pdgId) <= 1000039))");
ToolSvc += TRUTH3BSMTool

#TRUTH3bhadronTool = DerivationFramework__Truth3CollectionMaker(name                    = "TRUTH3bhadronTool",
#                                                                  NewCollectionName       = "Truthbhadron",
#                                                                  ParticleSelectionString = "TruthParticles.isBottomHadron")
#ToolSvc += TRUTH3bhadronTool

#==============================================================================
# Create the derivation kernel algorithm
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH3Kernel",
                                                                        AugmentationTools = [TRUTH3PhotonTool,TRUTH3ElectronTool,TRUTH3MuonTool,TRUTH3BSMTool,TRUTH3TopTool,TRUTH3BosonTool,TRUTH3TauTool],
                                                                        ThinningTools = [])

#==============================================================================
# Set up stream
#==============================================================================
streamName = derivationFlags.WriteDAOD_TRUTH3Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_TRUTH3Stream )
TRUTH3Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="TRUTH3ThinningSvc", outStreams=[evtStream] )

# Only events that pass the filters listed are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRUTH3Stream.AcceptAlgs(['TRUTH3Kernel'])

#==============================================================================
# Set up slimming content list here
#==============================================================================

from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper

TRUTH3SlimmingHelper = SlimmingHelper("TRUTH3SlimmingHelper")

TRUTH3SlimmingHelper.ExtraVariables = ["AntiKt4TruthJets.pt,AntiKt4TruthJets.eta,AntiKt4TruthJets.phi,AntiKt4TruthJets.m,AntiKt4TruthJets.GhostBHadronsFinalCount"]
TRUTH3SlimmingHelper.AppendContentToStream(TRUTH3Stream)

TRUTH3Stream.AddItem("xAOD::EventInfo#*")
TRUTH3Stream.AddItem("xAOD::EventAuxInfo#*")
TRUTH3Stream.AddItem("xAOD::JetContainer#TrimmedAntiKt10TruthJets*")
TRUTH3Stream.AddItem("xAOD::JetAuxContainer#TrimmedAntiKt10TruthJets*")
TRUTH3Stream.AddItem("xAOD::MissingETContainer#*")
TRUTH3Stream.AddItem("xAOD::MissingETAuxContainer#*")

TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#TruthPhotons*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#TruthElectrons*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#TruthMuons*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#TruthTop*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#TruthBoson*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#TruthBSM*")
#TRUTH3Stream.AddItem( "xAOD::TruthParticleContainer#Truthbhadron*")

TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthPhotons*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthElectrons*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthMuons*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTaus*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTop*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthBoson*")
TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthBSM*")
#TRUTH3Stream.AddItem( "xAOD::TruthParticleAuxContainer#Truthbhadron*")
