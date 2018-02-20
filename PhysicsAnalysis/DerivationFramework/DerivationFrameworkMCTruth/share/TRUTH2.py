#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
# Author: James Catmore
# Author: Frank Paige for hard truth, xAOD input support
#
# Paige: Added TRUTH2 to
# /usatlas/workarea/paige/thinTruth19/PhysicsAnalysis/DerivationFramework/DerivationFrameworkCore/python/DerivationFrameworkProdFlags.py
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# Add translator from EVGEN input to xAOD-like truth
# Add all the particle derivation tools
# This sets up its own common kernel and adds the common tools to it
from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
addStandardTruthContents()
# Extra classifiers for the Higgs group
import DerivationFrameworkHiggs.TruthCategories import

#==============================================================================
# Build hard truth HepMC
#==============================================================================

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__CompactHardTruth
thinHard = DerivationFramework__CompactHardTruth(
                                McEvent = "GEN_EVENT",
                                McEventOut = "GEN_HARD",
                                DanglePtCut = 1000.,
                                MaxCount = 5,
                                OutputLevel = Lvl.INFO)

DerivationFrameworkJob += thinHard

# Convert GEN_HARD to TruthEvent format with new name(s):
# Need to set TruthLinks to avoid conflict with standard name
# Need to set xAODTruthPileupEventContainerName
# No need for truth metadata here; it comes from the other converter
DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg(
                          "GEN_HARD2xAOD",
                          AODContainerName="GEN_HARD",
                          xAODTruthEventContainerName="TruthHard",
                          xAODTruthPileupEventContainerName="TruthHardPileup",
                          xAODTruthParticleContainerName="TruthHardParticles",
                          xAODTruthVertexContainerName="TruthHardVertices",
                          TruthLinks="TruthHardLinks",
                          WriteTruthMetaData=False,
                          OutputLevel = Lvl.INFO)

#==============================================================================
# Thin standard TruthEvent with HardTruthThinning
#==============================================================================

# HardTruthThinning is designed to work with CompactHardTruth. It uses
# ThinningSvc to keep truth particles related to hard objects:
#      - Stable particles matching those from CompactHardTruth
#      - Stable particles from truth jets (with some selection)
#      - Selected particles and their descendant particles/vertices
# The stable descendants of selected particles will be those linked to
# reconstructed objects.

# Particles to keep
cHadrons = [411,421,431, 4122,4132,4232,4332]
bHadrons = [511,521,531, 5122,5132,5232,5332]
tau      = [15]
keepers  = cHadrons + bHadrons + tau

from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__HardTruthThinning
TRUTH2TruthThinningTool = DerivationFramework__HardTruthThinning(
                         name = "HardThinTool",
                         ThinningService = "TRUTH2ThinningSvc",
                         EventInfo = "McEventInfo",
                         TruthParticles = "TruthParticles",
                         TruthVertices = "TruthVertices",
                         HardParticles = "TruthHardParticles",
                         KeepIds = keepers,
                         JetName = "AntiKt4TruthJets",
                         JetPtCut = 20000,
                         JetEtaCut = 2.5,
                         JetConstPtCut = 1000,
                         JetPhotonPtCut = 5000,
                         IsolRadius = 0.2,
                         IsolPtCut = 1000,
                         MaxCount = 0,
                         OutputLevel = Lvl.INFO)

thinningTools=[]
ToolSvc += TRUTH2TruthThinningTool
thinningTools.append(TRUTH2TruthThinningTool)

from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel(
                          "TRUTH2Kernel",
                          ThinningTools = thinningTools)

#==============================================================================
# Set up stream
#==============================================================================

streamName = derivationFlags.WriteDAOD_TRUTH2Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_TRUTH2Stream )
TRUTH2Stream = MSMgr.NewPoolRootStream( streamName, fileName )
# Only events that pass the filters listed are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRUTH2Stream.AcceptAlgs(['TRUTH2Kernel'])

# SPECIAL LINES FOR THINNING
# Thinning service name must match the one passed to the thinning tools
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
augStream = MSMgr.GetStream( streamName )
evtStream = augStream.GetEventStream()
svcMgr += createThinningSvc( svcName="TRUTH2ThinningSvc", outStreams=[evtStream] )

#==============================================================================
# Set up slimming content list here
#==============================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRUTH2SlimmingHelper = SlimmingHelper("TRUTH2SlimmingHelper")
TRUTH2SlimmingHelper.AppendToDictionary = {'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                           'MET_TruthRegions':'xAOD::MissingETContainer','MET_TruthRegionsAux':'xAOD::MissingETAuxContainer',
                                           'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                           'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer','AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer'
                                          }
TRUTH2SlimmingHelper.AllVariables = ["MET_Truth","MET_TruthRegions","TruthElectrons","TruthMuons","TruthPhotons","TruthTaus","TruthNeutrinos","TruthBSM","TruthTop","TruthBoson"]
TRUTH2SlimmingHelper.ExtraVariables = ["AntiKt4TruthDressedWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T",
                                       "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta"]
TRUTH2SlimmingHelper.AppendContentToStream(TRUTH2Stream)
# The entire event info, truth event, etc
TRUTH2Stream.AddItem("xAOD::EventInfo#*")
TRUTH2Stream.AddItem("xAOD::EventAuxInfo#*")
TRUTH2Stream.AddItem( "xAOD::TruthEventContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthEventAuxContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthVertexContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthVertexAuxContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthParticleContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthParticleAuxContainer#*" )
# Keep the metadata of course!
TRUTH2Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )
