#==============================================================================
# Set up common services and job object.
# This should appear in ALL derivation job options
#==============================================================================

# Add translator from EVGEN input to xAOD-like truth here
from DerivationFrameworkCore.DerivationFrameworkMaster import *
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
from DerivationFrameworkMCTruth.TruthDerivationTools import *

if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
        DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="GEN_EVENT")
elif objKeyStore.isInInput( "McEventCollection", "TruthEvent"):
        DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="TruthEvent")

from DerivationFrameworkHiggs.TruthCategories import *

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
akt4 = jtm.addJetFinder("AntiKt4TruthJets", "AntiKt", 0.4, "truth", modifiersin=[jtm.truthpartondr, jtm.partontruthlabel, jtm.removeconstit], ptmin= 5000)
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
# Create the derivation kernel algorithm
#==============================================================================
from DerivationFrameworkCore.DerivationFrameworkCoreConf import DerivationFramework__DerivationKernel
DerivationFrameworkJob += CfgMgr.DerivationFramework__DerivationKernel("TRUTH4Kernel")

#==============================================================================
# Set up stream
#==============================================================================
streamName = derivationFlags.WriteDAOD_TRUTH4Stream.StreamName
fileName = buildFileName( derivationFlags.WriteDAOD_TRUTH4Stream )
TRUTH4Stream = MSMgr.NewPoolRootStream( streamName, fileName )

# Only events that pass the filters listed are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
TRUTH4Stream.AcceptAlgs(['TRUTH4Kernel'])

#==============================================================================
# Set up slimming content list here
#==============================================================================
TRUTH4Stream.AddItem("xAOD::EventInfo#McEventInfo")
TRUTH4Stream.AddItem("xAOD::EventInfo#EventInfo")
TRUTH4Stream.AddItem("xAOD::EventAuxInfo#EventInfoAux.")
TRUTH4Stream.AddItem("xAOD::JetContainer#AntiKt4TruthWZJets")
TRUTH4Stream.AddItem("xAOD::JetContainer#AntiKt4TruthJets")
TRUTH4Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthJetsAux.")
TRUTH4Stream.AddItem("xAOD::JetAuxContainer#AntiKt4TruthWZJetsAux.")
TRUTH4Stream.AddItem("xAOD::MissingETContainer#MET_Truth")
TRUTH4Stream.AddItem("xAOD::MissingETContainer#MET_TruthRegions")
TRUTH4Stream.AddItem("xAOD::MissingETAuxContainer#MET_TruthAux.")
TRUTH4Stream.AddItem("xAOD::MissingETAuxContainer#MET_TruthRegionsAux.")
TRUTH4Stream.AddItem( "xAOD::TruthEventContainer#TruthEvents" )
TRUTH4Stream.AddItem( "xAOD::TruthEventAuxContainer#TruthEventsAux." )
TRUTH4Stream.AddItem( "xAOD::TruthVertexContainer#TruthVertices" )
TRUTH4Stream.AddItem( "xAOD::TruthVertexAuxContainer#TruthVerticesAux." )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthParticles" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleAuxContainer#TruthParticlesAux." )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#JetInputTruthParticles" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#JetInputTruthParticlesNoWZ" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBHadronsFinal" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBHadronsInitial" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelBQuarksFinal" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCHadronsFinal" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCHadronsInitial" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelCQuarksFinal" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelHBosons" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelPartons" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelTQuarksFinal" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelTausFinal" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelWBosons" )
TRUTH4Stream.AddItem( "xAOD::TruthParticleContainer#TruthLabelZBosons" )
