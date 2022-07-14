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

#==============================================================================
# Set up stream
#==============================================================================
streamName = derivationFlags.WriteDAOD_TRUTH3Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_TRUTH3Stream )
TRUTH3Stream = MSMgr.NewPoolRootStream( streamName, fileName )

#==============================================================================
# Set up slimming content list here
#==============================================================================
from DerivationFrameworkCore.SlimmingHelper import SlimmingHelper
TRUTH3SlimmingHelper = SlimmingHelper("TRUTH3SlimmingHelper")
TRUTH3SlimmingHelper.AppendToDictionary = {'TruthEvents':'xAOD::TruthEventContainer','TruthEventsAux':'xAOD::TruthEventAuxContainer',
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
                                           'AntiKt4TruthDressedWZJets':'xAOD::JetContainer','AntiKt4TruthDressedWZJetsAux':'xAOD::JetAuxContainer',
                                           'AntiKt10TruthSoftDropBeta100Zcut10Jets':'xAOD::JetContainer','AntiKt10TruthSoftDropBeta100Zcut10JetsAux':'xAOD::JetAuxContainer'
                                          }

from DerivationFrameworkMCTruth.MCTruthCommon import addTruth3ContentToSlimmerTool
addTruth3ContentToSlimmerTool(TRUTH3SlimmingHelper)
TRUTH3SlimmingHelper.AppendContentToStream(TRUTH3Stream)

# Keep the metadata of course!
TRUTH3Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )

# Remove the old-style event info. It just duplicates information, and it's quite large in the case of multiple event weights.
# Note that in 21.2, this means we can't read the file in athena. A bit too dangerous given truth analyses that might be of interest.
# In master, this is safe and saves us another 0.75kB/event or so
TRUTH3Stream.RemoveItem('EventInfo#*')

# New-style event info, though, we do need to keep!
TRUTH3Stream.AddItem("xAOD::EventInfo#*")
TRUTH3Stream.AddItem("xAOD::EventAuxInfo#*")

# If we don't have a conditions tag set by now, then assume this job isn't going to have one and kill the conditions service
if len(globalflags.ConditionsTag())==0:
    svcMgr.PoolSvc.ReadCatalog = []
