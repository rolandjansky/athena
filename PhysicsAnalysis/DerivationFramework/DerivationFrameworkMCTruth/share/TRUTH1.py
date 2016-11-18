#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkMCTruth.MCTruthCommon import * 
from DerivationFrameworkTau.TauTruthCommon import *
augmentationTools = []
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
  #Large R jets
  akt10 = jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth",ptmin= 100000)
  akt10alg = JetAlgorithm("jetalgAntiKt10TruthJets", Tools = [akt10] )
  DerivationFrameworkJob += akt10alg
  akt10trim = jtm.addJetTrimmer("TrimmedAntiKt10TruthJets", rclus=0.2, ptfrac=0.05, input='AntiKt10TruthJets', modifiersin=[jtm.nsubjettiness, jtm.removeconstit])
  akt10trimalg = JetAlgorithm("jetalgTrimmedAntiKt10TruthJets", Tools = [akt10trim] )
  DerivationFrameworkJob += akt10trimalg

# Add truth-based MET algorithm here
import METReconstruction.METConfig_Truth
from METReconstruction.METRecoFlags import metFlags # not sure if you even need this line
from METReconstruction.METRecoConfig import getMETRecoAlg
metAlg = getMETRecoAlg('METReconstruction')
DerivationFrameworkJob += metAlg

#==============================================================================
# HEAVY FLAVOR DECORATIONS (ttbar)
#==============================================================================
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginClassifier.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginDecorator.cxx
# list of ttbar samples by mc_channel_number
TRUTH1DSIDList=[
  410000,
  410001,
  410002,
  410003,
  410004,
  410007,
  410008,
  410009,
  301528,
  301529,
  301530,
  301531,
  301532,
  303722,
  303723,
  303724,
  303725,
  303726,
  407009,
  407010,
  407011,
  407012,
  410120,
  410121,
  426090,
  426091,
  426092,
  426093,
  426094,
  426095,
  426096,
  426097,
  429007,
]

import PyUtils.AthFile as af
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
f = af.fopen(athenaCommonFlags.FilesInput()[0])
if len(f.infos['run_number']) > 0:
  if(int((f.infos['run_number'])[0]) in TRUTH1DSIDList):
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginClassifier
    TRUTH1hadronorigintool = DerivationFramework__HadronOriginClassifier("TRUTH1HadronOriginClassifier",DSID=int((f.infos['run_number'])[0]))
    ToolSvc += TRUTH1hadronorigintool
    print "TRUTH1hadronorigintool: ", TRUTH1hadronorigintool
    from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HadronOriginDecorator
    TRUTH1hadronorigindecorator = DerivationFramework__HadronOriginDecorator(name = "TRUTH1HadronOriginDecorator")
    TRUTH1hadronorigindecorator.ToolName = TRUTH1hadronorigintool
    ToolSvc += TRUTH1hadronorigindecorator
    print "TRUTH1hadronorigindecorator: ", TRUTH1hadronorigindecorator
    augmentationTools.append(TRUTH1hadronorigindecorator)

#==============================================================================
# Thinning the master truth collection 
#==============================================================================
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__MenuTruthThinning
TRUTH1TruthThinning = DerivationFramework__MenuTruthThinning(name                      = "TRUTH1TruthThinning",
                                                            ThinningService            = "TRUTH1ThinningSvc",
                                                            WritePartons               = False,
                                                            WriteHadrons               = False,
                                                            WriteBHadrons              = True,
                                                            WritettHFHadrons           = True,
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
                                                            WriteAllLeptons            = True,
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

#Let's save the post-shower HT and MET filter values that will make combining filtered samples easier (adds to the EventInfo) 
SUSYGenFilt_MCTruthClassifier = MCTruthClassifier(name = "SUSYGenFilt_MCTruthClassifier",
                                                  ParticleCaloExtensionTool="")
ToolSvc += SUSYGenFilt_MCTruthClassifier
GenFilter = CfgMgr.DerivationFramework__SUSYGenFilterTool(
  "MTandHTGenFilt",
  )
ToolSvc += GenFilter

#==============================================================================
# Create the derivation kernel algorithm
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
augmentationTools += [DFCommonTruthClassificationTool,GenFilter,
                      DFCommonTruthMuonTool,DFCommonTruthElectronTool,DFCommonTruthPhotonTool,DFCommonTruthNeutrinoTool,
                      DFCommonTruthElectronDressingTool, DFCommonTruthMuonDressingTool,
                      DFCommonTruthElectronIsolationTool1, DFCommonTruthElectronIsolationTool2,
                      DFCommonTruthMuonIsolationTool1, DFCommonTruthMuonIsolationTool2,
                      DFCommonTruthPhotonIsolationTool1, DFCommonTruthPhotonIsolationTool2]
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH1Kernel",
                                                                        AugmentationTools = augmentationTools,
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
TRUTH1Stream.AddItem("xAOD::JetContainer#TrimmedAntiKt10TruthJets")
TRUTH1Stream.AddItem("xAOD::JetAuxContainer#TrimmedAntiKt10TruthJetsAux.")
TRUTH1Stream.AddItem("xAOD::JetContainer#AntiKt10TruthJets")
TRUTH1Stream.AddItem("xAOD::JetAuxContainer#AntiKt10TruthJetsAux.")
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
TRUTH1Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )
