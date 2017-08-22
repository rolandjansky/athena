#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkMCTruth.MCTruthCommon import * 
from DerivationFrameworkTau.TauTruthCommon import *

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
  # To remove jet constituents add the modifier jtm.removeconstit
  truth_modifiers = [jtm.truthpartondr, jtm.partontruthlabel, jtm.jetdrlabeler, jtm.trackjetdrlabeler]
  akt4 = jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", ptmin=15000, modifiersin=truth_modifiers)
  akt4alg = JetAlgorithm("jetalgAntiKt4TruthJets", Tools = [akt4] )
  DerivationFrameworkJob += akt4alg

  # WZ Truth Jets
  akt4wz = jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin=15000, modifiersin=truth_modifiers)
  akt4wzalg = JetAlgorithm("jetalgAntiKt4TruthWZJets", Tools = [akt4wz] )
  DerivationFrameworkJob += akt4wzalg

  # Some examples of other truth jet collections
  #akt6wz    = jtm.addJetFinder("AntiKt6TruthWZJets",  "AntiKt", 0.6,  "truthwz", ptmin= 5000)
  #akt6      = jtm.addJetFinder("AntiKt6TruthJets", "AntiKt", 0.6, "truth", ptmin= 5000)
  #akt10     = jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth", ptmin= 5000)
  #akt10trim = jtm.addJetTrimmer("TrimmedAntiKt10TruthJets", rclus=0.3, ptfrac=0.05, input='AntiKt10TruthJets')

  #Large R jets
  akt10 = jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth",ptmin= 100000)
  akt10alg = JetAlgorithm("jetalgAntiKt10TruthJets", Tools = [akt10] )
  DerivationFrameworkJob += akt10alg
  akt10trim = jtm.addJetTrimmer("TrimmedAntiKt10TruthJets", rclus=0.2, ptfrac=0.05, input='AntiKt10TruthJets', modifiersin=[jtm.nsubjettiness, jtm.removeconstit], doArea=False)
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
import DerivationFrameworkMCTruth.HFHadronsCommon
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginClassifier.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginDecorator.cxx

#Photons with a pT cut
TruthPhotonTool = DerivationFramework__TruthCollectionMaker(name                 = "TruthPhotonTool",
                                                             NewCollectionName       = "TruthPhotons",
                                                             ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && (TruthParticles.pt > 20.0*GeV)")
# ((TruthParticles.classifierParticleOrigin != 42) || (TruthParticles.pt > 20.0*GeV)))")
ToolSvc += TruthPhotonTool

'''
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
TruthPhotonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "TruthPhotonIsolationTool1",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += TruthPhotonIsolationTool1
TruthPhotonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "TruthPhotonIsolationTool2",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += TruthPhotonIsolationTool2
'''

#Now, for partons.
TruthTopTool = DerivationFramework__TruthCollectionMaker(name                   = "TruthTopTool",
                                                          NewCollectionName       = "TruthTop",
                                                          ParticleSelectionString = "(abs(TruthParticles.pdgId) == 6)",
                                                          Do_Compress = True,
                                                          )
ToolSvc += TruthTopTool

TruthBosonTool = DerivationFramework__TruthCollectionMaker(name                   = "TruthBosonTool",
                                                            NewCollectionName       = "TruthBoson",
                                                            ParticleSelectionString = "(abs(TruthParticles.pdgId) == 23 || abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) == 25)",
                                                            Do_Compress = True,
                                                            Do_Sherpa= True)
ToolSvc += TruthBosonTool

TruthBSMTool = DerivationFramework__TruthCollectionMaker(name                   = "TruthBSMTool",
                                                          NewCollectionName       = "TruthBSM",
                                                          ParticleSelectionString = "( (31<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<38) || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==41 || abs(TruthParticles.pdgId)==42 || abs(TruthParticles.pdgId)== 7 || abs(TruthParticles.pdgId)== 8 || (1000000<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<1000040) || (2000000<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<2000040) )",
                                                          Do_Compress = True)
ToolSvc += TruthBSMTool
#Taken from the code in DerivationFramework::MenuTruthThinning::isBSM

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
augmentationTools = [DFCommonTruthClassificationTool,GenFilter,
                     DFCommonTruthMuonTool,DFCommonTruthElectronTool,TruthPhotonTool,DFCommonTruthNeutrinoTool,
                     TruthTopTool,TruthBosonTool,TruthBSMTool,
                     DFCommonTruthElectronDressingTool, DFCommonTruthMuonDressingTool,
                     DFCommonTruthElectronIsolationTool1, DFCommonTruthElectronIsolationTool2,
                     DFCommonTruthMuonIsolationTool1, DFCommonTruthMuonIsolationTool2,
                     DFCommonTruthPhotonIsolationTool1, DFCommonTruthPhotonIsolationTool2]
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH1Kernel",
                                                                       AugmentationTools = augmentationTools,
                                                                       ThinningTools = [])
# Note: to add thinning of photons according to isolation, use the DFCommonTruthPhotonTool and the following line:
#                                                                       ThinningTools = [TruthPhotonIsolationTool1, TruthPhotonIsolationTool2])

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
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRUTH1SlimmingHelper = SlimmingHelper("TRUTH1SlimmingHelper")
TRUTH1SlimmingHelper.AppendToDictionary = {'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                           'MET_TruthRegions':'xAOD::MissingETContainer','MET_TruthRegionsAux':'xAOD::MissingETAuxContainer',
                                           'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                           'AntiKt4TruthWZJets':'xAOD::JetContainer','AntiKt4TruthWZJetsAux':'xAOD::JetAuxContainer',
                                           'TrimmedAntiKt10TruthJets':'xAOD::JetContainer','TrimmedAntiKt10TruthJetsAux':'xAOD::JetAuxContainer'
                                          }
TRUTH1SlimmingHelper.AllVariables = ["MET_Truth","MET_TruthRegions","TruthElectrons","TruthMuons","TruthPhotons","TruthTaus","TruthNeutrinos","TruthBSM","TruthTop","TruthBoson"]
TRUTH1SlimmingHelper.ExtraVariables = ["AntiKt4TruthWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T",
                                       "TrimmedAntiKt10TruthJets.pt.Tau1_wta.Tau2_wta.Tau3_wta"]
TRUTH1SlimmingHelper.AppendContentToStream(TRUTH1Stream)
# Add the event info, in its entirety
TRUTH1Stream.AddItem("xAOD::EventInfo#McEventInfo")
TRUTH1Stream.AddItem("xAOD::EventInfo#EventInfo")
TRUTH1Stream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
# Add the entirety of the truth event
TRUTH1Stream.AddItem( "xAOD::TruthEventContainer#TruthEvents" )
TRUTH1Stream.AddItem( "xAOD::TruthEventAuxContainer#TruthEventsAux." )
TRUTH1Stream.AddItem( "xAOD::TruthVertexContainer#TruthVertices" )
TRUTH1Stream.AddItem( "xAOD::TruthVertexAuxContainer#TruthVerticesAux." )
TRUTH1Stream.AddItem( "xAOD::TruthParticleContainer#TruthParticles" )
TRUTH1Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthParticlesAux." )
# Not clear to me if these are needed?
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
# Keep the metadata of course!
TRUTH1Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )
