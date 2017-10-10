#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
addStandardTruthContents()

# Flag to distinguish EVNT/xAOD input
# isEVNT = False
# This is now set in the MCTruthCommon

# Add translator from EVGEN input to xAOD-like truth here
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg

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
if dfInputIsEVNT:
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
# Set-up of the tols for writing special collections and decorators
#==============================================================================

from DerivationFrameworkMCTruth.TruthObjectTools import *
from DerivationFrameworkMCTruth.TruthDecoratorTools import *

#==============================================================================
# Thinning the master truth collection 
#==============================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
TRUTH1TruthThinning = DerivationFramework__MenuTruthThinning(name                      = "TRUTH1TruthThinning",
                                                            ThinningService            = "TRUTH1ThinningSvc",
                                                            WritePartons               = False,
                                                            WriteHadrons               = False,
                                                            WriteBHadrons              = False,
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
ToolSvc += TRUTH1TruthThinning

#==============================================================================
# Thinning the photon truth collection : no photons from pi0 (origin=42)
#==============================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__GenericTruthThinning
TRUTH1PhotonThinning = DerivationFramework__GenericTruthThinning(name                    = "TRUTH1PhotonThinning",
                                                                 ThinningService         = "TRUTH1ThinningSvc",
                                                                 ParticlesKey            = "TruthPhotons",  
                                                                 ParticleSelectionString = "(TruthPhotons.classifierParticleOrigin != 42) || (TruthPhotons.pt > 20.0*GeV)")
ToolSvc += TRUTH1PhotonThinning

#==============================================================================
# Create the derivation kernel algorithm
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH1Kernel",
                                                                        AugmentationTools = [TRUTH1ClassificationTool,
                                                                          TRUTH1MuonTool,TRUTH1ElectronTool,TRUTH1PhotonTool,TRUTH1TauTool,TRUTH1NeutrinoTool,
                                                                          TRUTH1ElectronDressingTool, TRUTH1MuonDressingTool,
                                                                          TRUTH1ElectronIsolationTool1, TRUTH1ElectronIsolationTool2,
                                                                          TRUTH1MuonIsolationTool1, TRUTH1MuonIsolationTool2,
                                                                          TRUTH1PhotonIsolationTool1, TRUTH1PhotonIsolationTool2],
                                                                        ThinningTools = [TRUTH1TruthThinning,TRUTH1PhotonThinning])

#==============================================================================
# Set up stream
#==============================================================================
streamName = derivationFlags.WriteDAOD_TRUTH1Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_TRUTH1Stream )
TRUTH1Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Thinning
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="TRUTH1ThinningSvc", outStreams=[evtStream] )

# Only events that pass the filters listed are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRUTH1Stream.AcceptAlgs(['TRUTH1Kernel'])

#==============================================================================
# Set up slimming content list here
#==============================================================================
TRUTH1Stream.AddItem("xAOD::EventInfo#McEventInfo")
TRUTH1Stream.AddItem("xAOD::EventInfo#EventInfo")
TRUTH1Stream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
TRUTH1Stream.AddItem("xAOD::JetContainer#AntiKt4TruthWZJets")
TRUTH1Stream.AddItem("xAOD::JetContainer#AntiKt4TruthJets")
TRUTH1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthJetsAux.")
TRUTH1Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthWZJetsAux.")
TRUTH1Stream.AddItem("xAOD::MissingETContainer#MET_Truth")
TRUTH1Stream.AddItem("xAOD::MissingETContainer#MET_TruthRegions")
TRUTH1Stream.AddItem("xAOD::MissingETAuxContainer#MET_TruthAux.")
TRUTH1Stream.AddItem("xAOD::MissingETAuxContainer#MET_TruthRegionsAux.")
TRUTH1Stream.AddItem( "xAOD::TruthEventContainer#TruthEvents" )
TRUTH1Stream.AddItem( "xAOD::TruthEventAuxContainer#TruthEventsAux." )
TRUTH1Stream.AddItem( "xAOD::TruthVertexContainer#TruthVertices" )
TRUTH1Stream.AddItem( "xAOD::TruthVertexAuxContainer#TruthVerticesAux." )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#JetInputTruthParticles" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#JetInputTruthParticlesNoWZ" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthElectrons" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBHadronsFinal" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBHadronsInitial" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBQuarksFinal" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCHadronsFinal" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCHadronsInitial" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCQuarksFinal" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelHBosons" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelPartons" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelTQuarksFinal" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelTausFinal" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelWBosons" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelZBosons" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthMuons" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthParticles" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthPhotons" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthTaus" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthNeutrinos" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthElectronsAux." )
TRUTH1Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthMuonsAux." )
TRUTH1Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthParticlesAux." )
TRUTH1Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthPhotonsAux." )
TRUTH1Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthTausAux." )
TRUTH1Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthNeutrinosAux." )
