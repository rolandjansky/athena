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
TRUTH1SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
                                           'MET_Truth':'xAOD::MissingETContainer','MET_TruthAux':'xAOD::MissingETAuxContainer',
                                           'TruthElectrons':'xAOD::TruthParticleContainer','TruthElectronsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthMuons':'xAOD::TruthParticleContainer','TruthMuonsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthPhotons':'xAOD::TruthParticleContainer','TruthPhotonsAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTaus':'xAOD::TruthParticleContainer','TruthTausAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthNeutrinos':'xAOD::TruthParticleContainer','TruthNeutrinosAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBSM':'xAOD::TruthParticleContainer','TruthBSMAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBoson':'xAOD::TruthParticleContainer','TruthBosonAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBottom':'xAOD::TruthParticleContainer','TruthBottomAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthTop':'xAOD::TruthParticleContainer','TruthTopAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthForwardProtons':'xAOD::TruthParticleContainer','TruthForwardProtonsAux':'xAOD::TruthParticleAuxContainer',
                                           'BornLeptons':'xAOD::TruthParticleContainer','BornLeptonsAux':'xAOD::TruthParticleAuxContainer',
                                           'HardScatterParticles':'xAOD::TruthParticleContainer','HardScatterParticlesAux':'xAOD::TruthParticleAuxContainer',
                                           'HardScatterVertices':'xAOD::TruthVertexContainer','HardScatterVerticesAux':'xAOD::TruthVertexAuxContainer',
                                           'TruthBosonsWithDecayParticles':'xAOD::TruthParticleContainer','TruthBosonsWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBosonsWithDecayVertices':'xAOD::TruthVertexContainer','TruthBosonsWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                           'TruthBSMWithDecayParticles':'xAOD::TruthParticleContainer','TruthBSMWithDecayParticlesAux':'xAOD::TruthParticleAuxContainer',
                                           'TruthBSMWithDecayVertices':'xAOD::TruthVertexContainer','TruthBSMWithDecayVerticesAux':'xAOD::TruthVertexAuxContainer',
                                           'AntiKt2TruthChargedJets':'xAOD::JetContainer','AntiKt2TruthChargedJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt10TruthJets':'xAOD::JetContainer','AntiKt10TruthJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt10TruthTrimmedPtFrac5SmallR20Jets':'xAOD::JetContainer','AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux':'xAOD::JetAuxContainer',
                                           'AntiKtVR30Rmax4Rmin02TruthChargedJets':'xAOD::JetContainer','AntiKtVR30Rmax4Rmin02TruthChargedJetsAux':'xAOD::JetAuxContainer'
                                          }
# Custom extra variables -- won't be overridden by the call below, just added to. Full lists for completeness.
TRUTH1SlimmingHelper.ExtraVariables = ["AntiKt2TruthChargedJets.GhostHBosonsCount.GhostHBosonsPt.GhostBHadronsFinalCount.GhostBHadronsFinalPt.GhostCHadronsFinalCount.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.constituentLinks",
                                       "AntiKtVR30Rmax4Rmin02TruthChargedJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TrueFlavor",
                                       "AntiKt4TruthDressedWZJets.GhostCHadronsFinalCount.GhostBHadronsFinalCount.pt.HadronConeExclTruthLabelID.ConeTruthLabelID.PartonTruthLabelID.TruthLabelDeltaR_B.TruthLabelDeltaR_C.TruthLabelDeltaR_T.ConeExclBHadronsFinal.ConeExclCHadronsFinal.ConeExclTausFinal.constituentLinks",
                                       "AntiKt10TruthJets.pt.GhostAntiKt2TruthChargedJets.GhostAntiKtVR30Rmax4Rmin02TruthChargedJets",
                                       "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.Parent.Tau1_wta.Tau2_wta.Tau3_wta.constituentLinks.D2"]

# Add standard content
from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
addTruth3ContentToSlimmerTool(TRUTH1SlimmingHelper)

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

# If we don't have a conditions tag set by now, then assume this job isn't going to have one and kill the conditions service
if len(globalflags.ConditionsTag())==0:
    for a in svcMgr.PoolSvc.ReadCatalog:
        svcMgr.PoolSvc.ReadCatalog.remove(a)
