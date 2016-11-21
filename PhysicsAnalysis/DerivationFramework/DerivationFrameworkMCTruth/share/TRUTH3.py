#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *
# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkMCTruth.MCTruthCommon import * 
from DerivationFrameworkTau.TauTruthCommon import *
# Flag to distinguish EVNT/xAOD input
# isEVNT = False
# This is now set in the MCTruthCommon

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
#jetFlags.truthFlavorTags = []

jetFlags.truthFlavorTags = ["BHadronsInitial", "BHadronsFinal", "BQuarksFinal",
                            "CHadronsInitial", "CHadronsFinal", "CQuarksFinal",
                            "TausFinal",
                            "Partons",
                            ]

if dfInputIsEVNT:
  # Standard truth jets
  # To recover jet constituents remove the last modifier.
  akt4 = jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit, jtm.jetdrlabeler, jtm.trackjetdrlabeler], ptmin= 20000)
  #akt4 = jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit, jtm.jetdrlabeler, jtm.trackjetdrlabeler], ptmin= 20000)
  akt4alg = JetAlgorithm("jetalgAntiKt4TruthJets", Tools = [akt4] )
  DerivationFrameworkJob += akt4alg

  # WZ Truth Jets
  akt4wz = jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin= 20000, modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit])
  akt4wzalg = JetAlgorithm("jetalgAntiKt4TruthWZJets", Tools = [akt4wz] )
  DerivationFrameworkJob += akt4wzalg
  #jtm.addJetFinder("AntiKt6TruthWZJets",  "AntiKt", 0.6,  "truthwz", ptmin= 5000)
  # Other jets
  #akt6  = jtm.addJetFinder("AntiKt6TruthJets", "AntiKt", 0.6, "truth", ptmin= 5000)
  #akt10 = jtm.addJetFinder("AntiKt10TruthJets", "AntiKt", 1.0, "truth", ptmin= 5000)
  #akt10trim = jtm.addJetTrimmer("TrimmedAntiKt10TruthJets", rclus=0.3, ptfrac=0.05, input='AntiKt10TruthJets')

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
# Create the derivation kernel algorithm
#==============================================================================

#Photons with a pT cut
TRUTH3PhotonTool = DerivationFramework__TruthCollectionMaker(name                 = "TRUTH3PhotonTool",
                                                             NewCollectionName       = "Truth3Photons",
                                                             ParticleSelectionString = "(abs(TruthParticles.pdgId) == 22) && (TruthParticles.status == 1) && (TruthParticles.pt > 20.0*GeV)")
# ((TruthParticles.classifierParticleOrigin != 42) || (TruthParticles.pt > 20.0*GeV)))")
ToolSvc += TRUTH3PhotonTool

'''
#from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__TruthIsolationTool
TRUTH3PhotonIsolationTool1 = DerivationFramework__TruthIsolationTool(name = "TRUTH3PhotonIsolationTool1",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.2],
                                                                  IsolationVarNamePrefix = 'etcone',
                                                                  ChargedParticlesOnly = False
                                                                  )
ToolSvc += TRUTH3PhotonIsolationTool1
TRUTH3PhotonIsolationTool2 = DerivationFramework__TruthIsolationTool(name = "TRUTH3PhotonIsolationTool2",
                                                                  isoParticlesKey = "TruthPhotons",
                                                                  allParticlesKey = "TruthParticles",
                                                                  particleIDsToCalculate = [22],
                                                                  IsolationConeSizes = [0.3],
                                                                  IsolationVarNamePrefix = 'ptcone',
                                                                  ChargedParticlesOnly = True
                                                                  )
ToolSvc += TRUTH3PhotonIsolationTool2
'''

#Now, for partons.
TRUTH3TopTool = DerivationFramework__TruthCollectionMaker(name                   = "TRUTH3TopTool",
                                                          NewCollectionName       = "TruthTop",
                                                          ParticleSelectionString = "(abs(TruthParticles.pdgId) == 6)",
                                                          Do_Compress = True,
                                                          )
ToolSvc += TRUTH3TopTool

TRUTH3BosonTool = DerivationFramework__TruthCollectionMaker(name                   = "TRUTH3BosonTool",
                                                            NewCollectionName       = "TruthBoson",
                                                            ParticleSelectionString = "(abs(TruthParticles.pdgId) == 23 || abs(TruthParticles.pdgId) == 24 || abs(TruthParticles.pdgId) == 25)",
                                                            Do_Compress = True,
                                                            Do_Sherpa= True)
ToolSvc += TRUTH3BosonTool

TRUTH3BSMTool = DerivationFramework__TruthCollectionMaker(name                   = "TRUTH3BSMTool",
                                                          NewCollectionName       = "TruthBSM",
                                                          ParticleSelectionString = "( (31<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<38) || abs(TruthParticles.pdgId)==39 || abs(TruthParticles.pdgId)==41 || abs(TruthParticles.pdgId)==42 || abs(TruthParticles.pdgId)== 7 || abs(TruthParticles.pdgId)== 8 || (1000000<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<1000040) || (2000000<abs(TruthParticles.pdgId) && abs(TruthParticles.pdgId)<2000040) )",
                                                          Do_Compress = True)
ToolSvc += TRUTH3BSMTool
#Taken from the code in DerivationFramework::MenuTruthThinning::isBSM

#Let's save the post-shower HT and MET filter values that will make combining filtered samples easier (adds to the EventInfo)
SUSYGenFilt_MCTruthClassifier = MCTruthClassifier(name = "SUSYGenFilt_MCTruthClassifier",
                                                  ParticleCaloExtensionTool="")
ToolSvc += SUSYGenFilt_MCTruthClassifier
GenFilter = CfgMgr.DerivationFramework__SUSYGenFilterTool(
  "MTandHTGenFilt",
  )
ToolSvc += GenFilter

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH3Kernel",
                                                                       AugmentationTools = [DFCommonTruthClassificationTool,GenFilter,
                                                                                            DFCommonTruthMuonTool,DFCommonTruthElectronTool,TRUTH3PhotonTool,DFCommonTruthNeutrinoTool,
                                                                                            TRUTH3TopTool,TRUTH3BosonTool,TRUTH3BSMTool,
                                                                                            DFCommonTruthElectronDressingTool, DFCommonTruthMuonDressingTool,
                                                                                            DFCommonTruthElectronIsolationTool1, DFCommonTruthElectronIsolationTool2,
                                                                                            DFCommonTruthMuonIsolationTool1, DFCommonTruthMuonIsolationTool2],
                                                                       ThinningTools = [])
#                                                                       ThinningTools = [TRUTH3PhotonIsolationTool1, TRUTH3PhotonIsolationTool2])

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
TRUTH3SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                           'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                           'MET_TruthRegions':'xAOD::MissingETContainer','MET_TruthRegionsAux':'xAOD::MissingETAuxContainer',
                                           'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                           'Truth3Photons':'xAOD::TruthParticleContainer','Truth3PhotonsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                           'AntiKt4TruthJets':'xAOD::JetContainer','AntiKt4TruthJetsAux':'xAOD::JetAuxContainer',
                                           'TrimmedAntiKt10TruthJets':'xAOD::JetContainer','TrimmedAntiKt10TruthJetsAux':'xAOD::JetAuxContainer'     
                                          }
TRUTH3SlimmingHelper.AllVariables = ["MET_Truth","MET_TruthRegions","TruthElectrons","TruthMuons","Truth3Photons","TruthTaus","TruthNeutrinos","TruthBSM","TruthTop","TruthBoson"]
TRUTH3SlimmingHelper.ExtraVariables = ["AntiKt4TruthJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T",
                                       "TrimmedAntiKt10TruthJets.pt.Tau1_wta.Tau2_wta.Tau3_wta",
                                       "TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.weights.crossSection"]
TRUTH3SlimmingHelper.AppendContentToStream(TRUTH3Stream)
TRUTH3Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )

