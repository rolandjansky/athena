#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# Add translator from EVGEN input to xAOD-like truth
# Add all the particle derivation tools
# This sets up its own common kernel and adds the common tools to it
from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents,addBornLeptonCollection,addHardScatterCollection
addStandardTruthContents()
# Special collection for Born leptons
addBornLeptonCollection()
# Special collection for hard scatter (matrix element) - save TWO extra generations of particles
addHardScatterCollection(None,2)
# Extra classifiers for the Higgs group
import DerivationFrameworkHiggs.TruthCategories

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

#==============================================================================
# Set up slimming content list here
#==============================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRUTH5SlimmingHelper = SlimmingHelper("TRUTH5SlimmingHelper")
TRUTH5SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
                                           'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                           'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                           'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                           'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer','AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer'
                                          }
TRUTH5SlimmingHelper.AllVariables = ["MET_Truth","MET_TruthRegions","TruthElectrons","TruthMuons","TruthPhotons","TruthTaus","TruthNeutrinos","TruthBSM","TruthTop","TruthBoson","TruthBSMWithDecayParticles","TruthBSMWithDecayVertices"]
TRUTH5SlimmingHelper.ExtraVariables = ["AntiKt4TruthDressedWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TrueFlavor",
                                       "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta",
                                       "TruthEvents.Q.XF1.XF2.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.weights.crossSection"]
TRUTH5SlimmingHelper.AppendContentToStream(TRUTH5Stream)
# Keep the metadata of course!
TRUTH5Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )
