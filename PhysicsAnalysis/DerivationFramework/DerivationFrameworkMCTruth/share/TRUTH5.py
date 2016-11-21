#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
from DerivationFrameworkMCTruth.TruthDerivationTools import *

#====================================================================
# GEN_AOD and xAOD truth making
#====================================================================

from McParticleAlgs.JobOptCfg import McAodBuilder,createMcAodBuilder
builder =  createMcAodBuilder(inMcEvtCollection = "GEN_EVENT")
DerivationFrameworkJob += builder
DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg("GEN_AOD2xAOD",AODContainerName="GEN_AOD")

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
akt4 = jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit, jtm.jetdrlabeler, jtm.trackjetdrlabeler], ptmin= 5000)
akt4alg = JetAlgorithm("jetalgAntiKt4TruthJets", Tools = [akt4] )
DerivationFrameworkJob += akt4alg

# WZ Truth Jets
akt4wz = jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin= 5000, modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit])
akt4wzalg = JetAlgorithm("jetalgAntiKt4TruthWZJets", Tools = [akt4wz] )
DerivationFrameworkJob += akt4wzalg
#jtm.addJetFinder("AntiKt6TruthWZJets",  "AntiKt", 0.6,  "truthwz", ptmin= 5000)
# Other jets
#akt6  = jtm.addJetFinder("AntiKt6TruthJets", "AntiKt", 0.6, "truth", ptmin= 5000)
#akt10 = jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth", ptmin= 5000)
#akt10trim = jtm.addJetTrimmer("TrimmedAntiKt10TruthJets", rclus=0.3, ptfrac=0.05, input='AntiKt10TruthJets')

# Add truth-based MET algorithm here
import METReconstruction.METConfig_Truth
from METReconstruction.METRecoFlags import metFlags # not sure if you even need this line
from METReconstruction.METRecoConfig import getMETRecoAlg
metAlg = getMETRecoAlg('METReconstruction')
DerivationFrameworkJob += metAlg

#==============================================================================
# Thinning the master truth collection 
#==============================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
TRUTH5TruthThinning = DerivationFramework__MenuTruthThinning(name                      = "TRUTH5TruthThinning",
                                                            ThinningService            = "TRUTH5ThinningSvc",
                                                            WritePartons               = False,
                                                            WriteHadrons               = False,
                                                            WriteBHadrons              = True,
                                                            WriteGeant                 = False,
                                                            GeantPhotonPtThresh        = -1.0,
                                                            WriteTauHad                = True,
                                                            PartonPtThresh             = -1.0,
                                                            WriteBSM                   = True,
                                                            WriteBosons                = True,
                                                            WriteBSMProducts           = True,
                                                            WriteBosonProducts         = True,
                                                            WriteTopAndDecays          = True,
                                                            WriteEverything            = False,
                                                            WriteAllLeptons            = False,
                                                            WriteStatus3               = False,
                                                            PreserveDescendants        = False, 
                                                            PreserveGeneratorDescendants = False,
                                                            PreserveAncestors          = True,
                                                            WriteFirstN                = 10)
ToolSvc += TRUTH5TruthThinning

#==============================================================================
# Thinning the photon truth collection : no photons from pi0 (origin=42)
#==============================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
TRUTH5PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "TRUTH5PhotonThinning",
                                                                 ThinningService         = "TRUTH5ThinningSvc",
                                                                 ParticlesKey            = "TruthPhotons",  
                                                                 ParticleSelectionString = "(TruthPhotons.classifierParticleOrigin != 42) || (TruthPhotons.pt > 20.0*GeV)")
ToolSvc += TRUTH5PhotonThinning

#==============================================================================
# Create the derivation kernel algorithm
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH5Kernel",
                                                                        AugmentationTools = [DFCommonTruthClassificationTool,
                                                                          DFCommonTruthMuonTool,DFCommonTruthElectronTool,DFCommonTruthPhotonTool,DFCommonTruthTauTool,DFCommonTruthNeutrinoTool,
                                                                          DFCommonTruthElectronDressingTool, DFCommonTruthMuonDressingTool,
                                                                          DFCommonTruthElectronIsolationTool1, DFCommonTruthElectronIsolationTool2,
                                                                          DFCommonTruthMuonIsolationTool1, DFCommonTruthMuonIsolationTool2,
                                                                          DFCommonTruthPhotonIsolationTool1, DFCommonTruthPhotonIsolationTool2],
                                                                        ThinningTools = [TRUTH5TruthThinning,TRUTH5PhotonThinning])

#==============================================================================
# Set up stream
#==============================================================================
streamName = derivationFlags.WriteDAOD_TRUTH5Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_TRUTH5Stream )
TRUTH5Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="TRUTH5ThinningSvc", outStreams=[evtStream] )

# Only events that pass the filters listed are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRUTH5Stream.AcceptAlgs(['TRUTH5Kernel'])

#==============================================================================
# Set up slimming content list here
#==============================================================================
TRUTH5Stream.AddItem("xAOD::EventInfo#McEventInfo")
TRUTH5Stream.AddItem("xAOD::EventInfo#EventInfo")
TRUTH5Stream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
TRUTH5Stream.AddItem("xAOD::JetContainer#AntiKt4TruthWZJets")
TRUTH5Stream.AddItem("xAOD::JetContainer#AntiKt4TruthJets")
TRUTH5Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthJetsAux.")
TRUTH5Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthWZJetsAux.")
TRUTH5Stream.AddItem("xAOD::MissingETContainer#MET_Truth")
TRUTH5Stream.AddItem("xAOD::MissingETContainer#MET_TruthRegions")
TRUTH5Stream.AddItem("xAOD::MissingETAuxContainer#MET_TruthAux.")
TRUTH5Stream.AddItem("xAOD::MissingETAuxContainer#MET_TruthRegionsAux.")
TRUTH5Stream.AddItem( "xAOD::TruthEventContainer#TruthEvents" )
TRUTH5Stream.AddItem( "xAOD::TruthEventAuxContainer#TruthEventsAux." )
TRUTH5Stream.AddItem( "xAOD::TruthVertexContainer#TruthVertices" )
TRUTH5Stream.AddItem( "xAOD::TruthVertexAuxContainer#TruthVerticesAux." )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#JetInputTruthParticles" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#JetInputTruthParticlesNoWZ" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthElectrons" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBHadronsFinal" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBHadronsInitial" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBQuarksFinal" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCHadronsFinal" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCHadronsInitial" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCQuarksFinal" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelHBosons" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelPartons" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelTQuarksFinal" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelTausFinal" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelWBosons" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelZBosons" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthMuons" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthParticles" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthPhotons" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleContainer#TruthNeutrinos" )
TRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthElectronsAux." )
TRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthMuonsAux." )
TRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthParticlesAux." )
TRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthPhotonsAux." )
TRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTausAux." )
TRUTH5Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux." )
TRUTH5Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )
