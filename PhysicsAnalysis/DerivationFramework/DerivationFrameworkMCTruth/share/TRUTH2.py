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

#====================================================================
# ATTACH THE RECONSTRUCTION TO THE SEQUENCER  
#====================================================================

# Flag to distinguish EVNT/xAOD input
isEVNT = True

# Add translator from EVGEN input to xAOD-like truth here
from RecExConfig.ObjKeyStore import objKeyStore
from xAODTruthCnv.xAODTruthCnvConf import xAODMaker__xAODTruthCnvAlg
from DerivationFrameworkMCTruth.DerivationFrameworkMCTruthConf import DerivationFramework__xAODtoHepMCCnvAlg

if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
    DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg("GEN_EVNT2xAOD",AODContainerName="GEN_EVENT")
    isEVNT = True
else:
    # Add back-translator from xAOD::TruthEvent to HepMC for xAOD input
    # Needed to run hard truth on xAOD input files
    # Use GEN_EVENT name for output
    DerivationFrameworkJob += DerivationFramework__xAODtoHepMCCnvAlg("xAOD2GEN_EVENT",
                                  xAODTruthEventKey = "TruthEvents",
                                  HepMCTruthEventKey = "GEN_EVENT",
                                  MaxCount = 5,
                                  OutputLevel = Lvl.INFO)
    isEVNT = False
    
#==============================================================================
# Make truth jets and MET
#==============================================================================

# Only do this for EVNT input, not xAOD
# For Rel.20 use JetRec-02-04-82-08 to avoid CaloCluster ERROR for truth

if( isEVNT ):

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
    #jtm.addJetFinder("AntiKt4TruthWZJets",  "AntiKt", 0.4,  "truthwz", ptmin= 5000)
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

DerivationFrameworkJob += xAODMaker__xAODTruthCnvAlg(
                          "GEN_HARD2xAOD",
                          AODContainerName="GEN_HARD",
                          xAODTruthEventContainerName="TruthHard",
                          xAODTruthPileupEventContainerName="TruthHardPileup",
                          xAODTruthParticleContainerName="TruthHardParticles",
                          xAODTruthVertexContainerName="TruthHardVertices",
                          TruthLinks="TruthHardLinks",
                          OutputLevel = Lvl.INFO)


#####################################
# Skip new truth collections in TRUTH1
#####################################

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

TRUTH2Stream.AddItem("xAOD::EventInfo#*")
TRUTH2Stream.AddItem("xAOD::EventAuxInfo#*")
TRUTH2Stream.AddItem("xAOD::JetContainer#*")
TRUTH2Stream.AddItem("xAOD::JetAuxContainer#*")
TRUTH2Stream.AddItem("xAOD::MissingETContainer#*")
TRUTH2Stream.AddItem("xAOD::MissingETAuxContainer#*")

TRUTH2Stream.AddItem( "xAOD::TruthEventContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthEventAuxContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthVertexContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthVertexAuxContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthParticleContainer#*" )
TRUTH2Stream.AddItem( "xAOD::TruthParticleAuxContainer#*" )
TRUTH2Stream.AddMetaDataItem( [ "xAOD::TruthMetaDataContainer#TruthMetaData", "xAOD::TruthMetaDataAuxContainer#TruthMetaDataAux." ] )

