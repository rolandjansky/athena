#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkMaster import *

# Add translator from EVGEN input to xAOD-like truth
# Add all the particle derivation tools
# This sets up its own common kernel and adds the common tools to it
from DerivationFrameworkMCTruth.MCTruthCommon import addStandardTruthContents
addStandardTruthContents()
# Extra classifiers for the Higgs group
import DerivationFrameworkHiggs.TruthCategories

#==============================================================================
# HEAVY FLAVOR DECORATIONS (ttbar)
#==============================================================================
import DerivationFrameworkMCTruth.HFHadronsCommon
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginClassifier.cxx
# PhysicsAnalysis/DerivationFramework/DerivationFrameworkMCTruth/trunk/src/HadronOriginDecorator.cxx

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
                                           'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer','AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer'
                                          }
TRUTH1SlimmingHelper.AllVariables = ["MET_Truth","MET_TruthRegions","TruthElectrons","TruthMuons","TruthPhotons","TruthTaus","TruthNeutrinos","TruthBSM","TruthTop","TruthBoson"]
TRUTH1SlimmingHelper.ExtraVariables = ["AntiKt4TruthDressedWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.constituentLinks",
                                       "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Tau1_wta.Tau2_wta.Tau3_wta.constituentLinks"]

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
