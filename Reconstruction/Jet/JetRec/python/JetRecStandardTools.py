# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# JetRecStandardTools.py
#
# David Adams
# March 2014
#
# Define the low-level tools used in jet reconstruction.
#
# Tools are configured and put in the global jet tool manager so
# they can be accessed when configuring JetRec tools.
#
# Execute this file to add the definitions to
# JetRecStandardToolManager.jtm, e.g.
#   import JetRec.JetRecStandardTools

# Import the jet flags.
from JetRec.JetRecFlags import jetFlags

if not "UseTriggerStore " in locals():
  UseTriggerStore = False

# get levels defined VERBOSE=1 etc.
from GaudiKernel.Constants import *

from eflowRec.eflowRecFlags import jobproperties

from JetRec.JetRecStandardToolManager import jtm
from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
from MCTruthClassifier.MCTruthClassifierConf import MCTruthClassifier

from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool as RetrievePFOTool
from PFlowUtils.PFlowUtilsConf import CP__WeightPFOTool as WeightPFOTool
from JetRecTools.JetRecToolsConf import TrackPseudoJetGetter
from JetRecTools.JetRecToolsConf import JetTrackSelectionTool
from JetRecTools.JetRecToolsConf import SimpleJetTrackSelectionTool
from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
try:
  from JetRecCalo.JetRecCaloConf import MissingCellListTool
  jtm.haveJetRecCalo = True
except ImportError:
  jtm.haveJetRecCalo = False
from JetRecTools.JetRecToolsConf import PFlowPseudoJetGetter
from JetRec.JetRecConf import JetPseudojetRetriever
from JetRec.JetRecConf import JetConstituentsRetriever
from JetRec.JetRecConf import JetRecTool
from JetRec.JetRecConf import PseudoJetGetter
from JetRec.JetRecConf import MuonSegmentPseudoJetGetter
from JetRec.JetRecConf import JetFromPseudojet
from JetRec.JetRecConf import JetConstitRemover
from JetRec.JetRecConf import JetSorter
from JetMomentTools.JetMomentToolsConf import JetCaloQualityTool
try:
  from JetMomentTools.JetMomentToolsConf import JetCaloCellQualityTool
  jtm.haveJetCaloCellQualityTool = True
except ImportError:
  jtm.haveJetCaloCellQualityTool = False
from JetMomentTools.JetMomentToolsConf import JetWidthTool
from JetMomentTools.JetMomentToolsConf import JetCaloEnergies
try:
  from JetMomentTools.JetMomentToolsConf import JetJetBadChanCorrTool
  jtm.haveJetBadChanCorrTool = True
except ImportError:
  jtm.haveJetBadChanCorrTool = False
from JetMomentTools.JetMomentToolsConf import JetECPSFractionTool
from JetMomentTools.JetMomentToolsConf import JetVertexFractionTool
from JetMomentTools.JetMomentToolsConf import JetVertexTaggerTool
from JetMomentTools.JetMomentToolsConf import JetTrackMomentsTool
from JetMomentTools.JetMomentToolsConf import JetTrackSumMomentsTool
from JetMomentTools.JetMomentToolsConf import JetClusterMomentsTool
from JetMomentTools.JetMomentToolsConf import JetVoronoiMomentsTool
from JetMomentTools.JetMomentToolsConf import JetIsolationTool
from JetMomentTools.JetMomentToolsConf import JetLArHVTool
from JetMomentTools.JetMomentToolsConf import JetOriginCorrectionTool
from JetMomentTools.JetMomentToolsConf import JetConstitFourMomTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtDeltaRTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import NSubjettinessTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KTSplittingScaleTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import AngularityTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import DipolarityTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import PlanarFlowTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import KtMassDropTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import EnergyCorrelatorTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import CenterOfMassShapesTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetPullTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import JetChargeTool
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import QwTool
try:
  from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import ShowerDeconstructionTool
  jtm.haveShowerDeconstructionTool = True
except ImportError:
  jtm.haveShowerDeconstructionTool = False
try:
  from ParticleJetTools.ParticleJetToolsConf import Analysis__JetQuarkLabel
  jtm.haveParticleJetTools = True
except:
  jtm.haveParticleJetTools = False
if jtm.haveParticleJetTools:
  from ParticleJetTools.ParticleJetToolsConf import Analysis__JetConeLabeling
  from ParticleJetTools.ParticleJetToolsConf import Analysis__JetPartonTruthLabel
  from ParticleJetTools.ParticleJetToolsConf import CopyTruthJetParticles
  from ParticleJetTools.ParticleJetToolsConf import ParticleJetDeltaRLabelTool


ghostScaleFactor = 1e-40


#--------------------------------------------------------------
# Track selection.
#--------------------------------------------------------------

# This is the InDet loose selection from
# https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InDetTrackingPerformanceGuidelines
# October 28, 2014
#jtm += InDet__InDetDetailedTrackSelectionTool(
jtm += InDet__InDetTrackSelectionTool(
  "trk_trackselloose",
  CutLevel = "Loose",
  minPt    = 500
)

jtm += JetTrackSelectionTool(
  "trackselloose",
  InputContainer  = jtm.trackContainer,
  OutputContainer = "JetSelectedTracks",
  Selector        = jtm.trk_trackselloose
)

jtm += InDet__InDetTrackSelectionTool(
  "trk_trackselloose_trackjets",
  CutLevel = "Loose",
  minPt    = 500
)

jtm += JetTrackSelectionTool(
   "trackselloose_trackjets",
  InputContainer  = jtm.trackContainer,
  OutputContainer = "JetSelectedTracks_LooseTrackJets",
  Selector        = jtm.trk_trackselloose_trackjets
)

if jetFlags.useInDetTrackSelection():
  jtm += JetTrackSelectionTool(
    "tracksel",
    InputContainer  = jtm.trackContainer,
    OutputContainer = "JetSelectedTracks",
    Selector        = jtm.trk_trackselloose
  )
else:
  jtm += SimpleJetTrackSelectionTool(
    "tracksel",
    PtMin = 500.0,
    InputContainer  = jtm.trackContainer,
    OutputContainer = "JetSelectedTracks",
  )

#--------------------------------------------------------------
# Track-vertex association.
#--------------------------------------------------------------
from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool
jtm += CP__TightTrackVertexAssociationTool("jetTightTVAtool", dzSinTheta_cut=3, doPV=True)

from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__LooseTrackVertexAssociationTool
jtm += CP__LooseTrackVertexAssociationTool("jetLooseTVAtool")

jtm += TrackVertexAssociationTool(
  "tvassoc",
  TrackParticleContainer  = jtm.trackContainer,
  TrackVertexAssociation  = "JetTrackVtxAssoc",
  VertexContainer         = jtm.vertexContainer,
  TrackVertexAssoTool     = jtm.jetTightTVAtool,
)

jtm += TrackVertexAssociationTool(
  "tvassoc_old",
  TrackParticleContainer  = jtm.trackContainer,
  TrackVertexAssociation  = "JetTrackVtxAssoc_old",
  VertexContainer         = jtm.vertexContainer,
  MaxTransverseDistance   = 1.5,
  MaxLongitudinalDistance = 1.0e7,
  MaxZ0SinTheta = 1.5
)

#--------------------------------------------------------------
# Truth selection.
#--------------------------------------------------------------

if jetFlags.useTruth:
    truthClassifier = MCTruthClassifier(name = "JetMCTruthClassifier",
                                       ParticleCaloExtensionTool="")
    jtm += truthClassifier

    # Extra config: make sure if we are using EVNT that we don't try to check sim metadata 
    from RecExConfig.ObjKeyStore import objKeyStore
    barCodeFromMetadata=2
    # Input file is EVNT
    if objKeyStore.isInInput( "McEventCollection", "GEN_EVENT" ):
        barCodeFromMetadata=0

    jtm += CopyTruthJetParticles("truthpartcopy", OutputName="JetInputTruthParticles",
                                 MCTruthClassifier=truthClassifier,BarCodeFromMetadata=barCodeFromMetadata)
    jtm += CopyTruthJetParticles("truthpartcopywz", OutputName="JetInputTruthParticlesNoWZ",
                                 MCTruthClassifier=truthClassifier,
                                 IncludePromptLeptons=False,IncludePromptPhotons=False,
                                 IncludeMuons=True,IncludeNeutrinos=True,BarCodeFromMetadata=barCodeFromMetadata)


#--------------------------------------------------------------
# Jet reco infrastructure.
#--------------------------------------------------------------

# Jet pseudojet retriever.
jtm += JetPseudojetRetriever("jpjretriever")

# Jet constituent retriever.
labs = []
if jetFlags.useTracks():
  labs += ["Track"]
  labs += ["AntiKt2TrackJet", "AntiKt2TrackJet"]
if jetFlags.useMuonSegments():
  labs += ["MuonSegment",]
if jetFlags.useTruth():
  labs += ["Truth"]
  for lab in jetFlags.truthFlavorTags():
    labs += [lab]
jtm += JetConstituentsRetriever(
  "jconretriever",
  UsePseudojet = True,
  UseJetConstituents = True,
  PseudojetRetriever = jtm.jpjretriever,
  GhostLabels = labs,
  GhostScale = ghostScaleFactor
)

#--------------------------------------------------------------
# Pseudojet builders.
#--------------------------------------------------------------

# Prepare a sequence of input constituent modifiers
from JetRecTools.JetRecToolsConfig import ctm
jtm += ctm.buildConstitModifSequence( "JetConstitSeq_LCOrigin",
    OutputContainer='LCOriginTopoClusters',
    InputContainer= 'CaloCalTopoClusters',
    modList = [  'clus_origin' ] )

jtm += ctm.buildConstitModifSequence( "JetConstitSeq_EMOrigin",
    OutputContainer='EMOriginTopoClusters',
    InputContainer= 'CaloCalTopoClusters',                                      
    modList = [ 'clus_emscale', 'clus_origin' ] )



jtm += PseudoJetGetter(
  "lcoriginget",
  InputContainer = jtm.JetConstitSeq_LCOrigin.OutputContainer,
  Label = "LCTopoOrigin",
  OutputContainer = jtm.JetConstitSeq_LCOrigin.OutputContainer+"PseudoJet",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

jtm += PseudoJetGetter(
  "emoriginget",
  InputContainer = jtm.JetConstitSeq_EMOrigin.OutputContainer,
  Label = "EMTopoOrigin",
  OutputContainer = jtm.JetConstitSeq_EMOrigin.OutputContainer+"PseudoJet",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

# Clusters.
jtm += PseudoJetGetter(
  "lcget",
  InputContainer = "CaloCalTopoClusters",
  Label = "LCTopo",
  OutputContainer = "PseudoJetLCTopo",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

# EM clusters.
jtm += PseudoJetGetter(
  "emget",
  InputContainer = "CaloCalTopoClusters",
  Label = "EMTopo",
  OutputContainer = "PseudoJetEMTopo",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

# Tracks.
jtm += TrackPseudoJetGetter(
  "trackget",
  InputContainer = jtm.trackselloose_trackjets.OutputContainer,
  Label = "Track",
  OutputContainer = "PseudoJetTracks",
  TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

# Ghost tracks.
jtm += TrackPseudoJetGetter(
  "gtrackget",
  InputContainer = jtm.tracksel.OutputContainer,
  Label = "GhostTrack",
  OutputContainer = "PseudoJetGhostTracks",
  TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
  SkipNegativeEnergy = True,
  GhostScale = ghostScaleFactor
)

# Muon segments
jtm += MuonSegmentPseudoJetGetter(
  "gmusegget",
  InputContainer = "MuonSegments",
  Label = "GhostMuonSegment",
  OutputContainer = "PseudoJetGhostMuonSegment",
  Pt = 1.e-20
)

# TCCs.
jtm += PseudoJetGetter(
  "tccget",
  InputContainer = "TrackCaloClustersCombinedAndNeutral",
  Label = "TrackCaloCluster",
  OutputContainer = "PseudoJetTCC",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

# Retriever for pflow objects.
jtm += RetrievePFOTool("pflowretriever")

# Weight tool for charged pflow objects.
jtm += WeightPFOTool("pflowweighter")
jtm += WeightPFOTool("pflowweighter_LC",NeutralPFOScale="LC")

useVertices = True
if False == jetFlags.useVertices:
  useVertices = False

if True == jobproperties.eflowRecFlags.useUpdated2015ChargedShowerSubtraction:
  useChargedWeights = True
else:
  useChargedWeights = False

useTrackVertexTool = False
if True == jetFlags.useTrackVertexTool:
  useTrackVertexTool = True

# EM-scale pflow.
jtm += PFlowPseudoJetGetter(
  "empflowget",
  Label = "EMPFlow",
  OutputContainer = "PseudoJetEMPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  WeightPFOTool = jtm.pflowweighter,
  InputIsEM = True,
  CalibratePFO = False,
  SkipNegativeEnergy = True,
  UseChargedWeights = useChargedWeights,
  UseVertices = useVertices,
  UseTrackToVertexTool = useTrackVertexTool
)

# Calibrated EM-scale pflow.
jtm += PFlowPseudoJetGetter(
  "emcpflowget",
  Label = "EMCPFlow",
  OutputContainer = "PseudoJetEMCPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  WeightPFOTool = jtm.pflowweighter_LC,
  InputIsEM = True,
  CalibratePFO = True,
  SkipNegativeEnergy = True,
  UseChargedWeights = useChargedWeights,
  UseVertices = useVertices,
  UseTrackToVertexTool = useTrackVertexTool
)

# LC-scale pflow.
jtm += PFlowPseudoJetGetter(
  "lcpflowget",
  Label = "LCPFlow",
  OutputContainer = "PseudoJetLCPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  WeightPFOTool = jtm.pflowweighter_LC,
  InputIsEM = False,
  CalibratePFO = False,
  SkipNegativeEnergy = True,
  UseChargedWeights = useChargedWeights,
  UseVertices = useVertices,
  UseTrackToVertexTool = useTrackVertexTool
)

# AntiKt2 track jets.
jtm += PseudoJetGetter(
  "gakt2trackget", # give a unique name
  InputContainer = jetFlags.containerNamePrefix() + "AntiKt2PV0TrackJets", # SG key
  Label = "GhostAntiKt2TrackJet",   # this is the name you'll use to retrieve associated ghosts
  OutputContainer = "PseudoJetGhostAntiKt2TrackJet",
  SkipNegativeEnergy = True,
  GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
)

# AntiKt3 track jets.
# jtm += PseudoJetGetter(
#   "gakt3trackget", # give a unique name
#   InputContainer = jetFlags.containerNamePrefix() + "AntiKt3PV0TrackJets", # SG key
#   Label = "GhostAntiKt3TrackJet",   # this is the name you'll use to retrieve associated ghosts
#   OutputContainer = "PseudoJetGhostAntiKt3TrackJet",
#   SkipNegativeEnergy = True,
#   GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
# )

# AntiKt4 track jets.
jtm += PseudoJetGetter(
  "gakt4trackget", # give a unique name
  InputContainer = jetFlags.containerNamePrefix() + "AntiKt4PV0TrackJets", # SG key
  Label = "GhostAntiKt4TrackJet",   # this is the name you'll use to retrieve associated ghosts
  OutputContainer = "PseudoJetGhostAntiKt4TrackJet",
  SkipNegativeEnergy = True,
  GhostScale = ghostScaleFactor,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
)

# Truth.
if jetFlags.useTruth and jtm.haveParticleJetTools:
  jtm += PseudoJetGetter(
    "truthget",
    Label = "Truth",
    InputContainer = jtm.truthpartcopy.OutputName,
    OutputContainer = "PseudoJetTruth",
    GhostScale = 0.0,
    SkipNegativeEnergy = True,

  )
  jtm += PseudoJetGetter(
    "truthwzget",
    Label = "TruthWZ",
    InputContainer = jtm.truthpartcopywz.OutputName,
    OutputContainer = "PseudoJetTruthWZ",
    GhostScale = 0.0,
    SkipNegativeEnergy = True,
    
  )
  jtm += PseudoJetGetter(
    "gtruthget",
    Label = "GhostTruth",
    InputContainer = jtm.truthpartcopy.OutputName,
    OutputContainer = "PseudoJetGhostTruth",
    GhostScale = ghostScaleFactor,
    SkipNegativeEnergy = True,
  )

  # Truth flavor tags.
  for ptype in jetFlags.truthFlavorTags():
    jtm += PseudoJetGetter(
      "gtruthget_" + ptype,
      InputContainer = "TruthLabel" + ptype,
      Label = "Ghost" + ptype,
      OutputContainer = "PseudoJetGhost" + ptype,
      SkipNegativeEnergy = True,
      GhostScale = ghostScaleFactor,
    )

  # ParticleJetTools tools may be omitted in analysi releases.
  #ift jtm.haveParticleJetTools:
  # Delta-R truth parton label: truthpartondr.
  jtm += Analysis__JetQuarkLabel(
      "jetquarklabel",
      McEventCollection = "TruthEvents"
    )
  jtm += Analysis__JetConeLabeling(
      "truthpartondr",
      JetTruthMatchTool = jtm.jetquarklabel
      )
  
  # Parton truth label.
  jtm += Analysis__JetPartonTruthLabel("partontruthlabel")

  # Cone matching for B, C and tau truth for all but track jets.
  jtm += ParticleJetDeltaRLabelTool(
    "jetdrlabeler",
    LabelName = "HadronConeExclTruthLabelID",
    DoubleLabelName = "HadronConeExclExtendedTruthLabelID",
    BLabelName = "ConeExclBHadronsFinal",
    CLabelName = "ConeExclCHadronsFinal",
    TauLabelName = "ConeExclTausFinal",
    BParticleCollection = "TruthLabelBHadronsFinal",
    CParticleCollection = "TruthLabelCHadronsFinal",
    TauParticleCollection = "TruthLabelTausFinal",
    PartPtMin = 5000.0,
    JetPtMin =     0.0,
    DRMax = 0.3,
    MatchMode = "MinDR"
  )

  # Cone matching for B, C and tau truth for track jets.
  jtm += ParticleJetDeltaRLabelTool(
    "trackjetdrlabeler",
    LabelName = "HadronConeExclTruthLabelID",
    DoubleLabelName = "HadronConeExclExtendedTruthLabelID",
    BLabelName = "ConeExclBHadronsFinal",
    CLabelName = "ConeExclCHadronsFinal",
    TauLabelName = "ConeExclTausFinal",
    BParticleCollection = "TruthLabelBHadronsFinal",
    CParticleCollection = "TruthLabelCHadronsFinal",
    TauParticleCollection = "TruthLabelTausFinal",
    PartPtMin = 5000.0,
    JetPtMin = 4500.0,
    DRMax = 0.3,
    MatchMode = "MinDR"
  )

#--------------------------------------------------------------
# Jet builder.
# The tool manager must have one jet builder.
#--------------------------------------------------------------

jtm.addJetBuilderWithArea(JetFromPseudojet(
  "jblda",
  Attributes = ["ActiveArea", "ActiveArea4vec"]
))

jtm.addJetBuilderWithoutArea(JetFromPseudojet(
  "jbldna",
  Attributes = []
))

#--------------------------------------------------------------
# Non-substructure moment builders.
#--------------------------------------------------------------

# Quality from clusters.
jtm += JetCaloQualityTool(
  "caloqual_cluster",
  TimingCuts = [5, 10],
  Calculations = ["LArQuality", "N90Constituents", "FracSamplingMax",  "NegativeE", "Timing", "HECQuality", "Centroid", "AverageLArQF", "BchCorrCell"],
)

# Quality from cells.
if jtm.haveJetCaloCellQualityTool:
  jtm += JetCaloCellQualityTool(
    "caloqual_cell",
    LArQualityCut = 4000,
    TileQualityCut = 254,
    TimingCuts = [5, 10],
    Calculations = ["LArQuality", "N90Cells", "FracSamplingMax",  "NegativeE", "Timing", "HECQuality", "Centroid", "AverageLArQF"]
  )

# Jet width.
jtm += JetWidthTool("width", WeightPFOToolEM=jtm.pflowweighter, WeightPFOToolLC=jtm.pflowweighter_LC)

# Calo layer energies.
jtm += JetCaloEnergies("jetens")

# Read in missing cell map (needed for the following)
# commented out : incompatible with trigger : ATR-9696
## if jtm.haveJetRecCalo:
##     def missingCellFileReader(): 
##       import os
##       dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
##       dataPathList.insert(0, os.curdir)
##       from AthenaCommon.Utils.unixtools import FindFile
##       RefFileName = FindFile( "JetBadChanCorrTool.root" ,dataPathList, os.R_OK )
##       from AthenaCommon.AppMgr import ServiceMgr
##       if not hasattr(ServiceMgr, 'THistSvc'):
##         from GaudiSvc.GaudiSvcConf import THistSvc
##         ServiceMgr += THistSvc()
##       ServiceMgr.THistSvc.Input += ["JetBadChanCorrTool DATAFILE=\'%s\' OPT=\'READ\'" % RefFileName]
##       missingCellFileReader.called = True 

##     missingCellFileReader()

##     jtm += MissingCellListTool(
##       "missingcells",
##       AddCellList = [],
##       RemoveCellList = [],
##       AddBadCells = True,
##       DeltaRmax = 1.0,
##       AddCellFromTool = False,
##       LArMaskBit = 608517,
##       TileMaskBit = 1,
##       MissingCellMapName = "MissingCaloCellsMap"
## )

## # Bad channel corrections from cells
## if jtm.haveJetBadChanCorrTool:
##   jtm += JetBadChanCorrTool(
##     "bchcorrcell",
##     NBadCellLimit = 10000,
##     StreamName = "/JetBadChanCorrTool/",
##     ProfileName = "JetBadChanCorrTool.root",
##     ProfileTag = "",
##     UseCone = True,
##     UseCalibScale = False,
##     MissingCellMap = "MissingCaloCellsMap",
##     ForceMissingCellCheck = False,
##     UseClusters = False,
##   )
  
##   # Bad channel corrections from clusters
##   jtm += JetBadChanCorrTool(
##     "bchcorrclus",
##     NBadCellLimit = 0,
##     StreamName = "",
##     ProfileName = "",
##     ProfileTag = "",
##     UseCone = True,
##     UseCalibScale = False,
##     MissingCellMap = "",
##     ForceMissingCellCheck = False,
##     UseClusters = True
##   )

# Jet vertex fraction.
# jtm += JetVertexFractionTool(
#   "jvfold",
#   VertexContainer = jtm.vertexContainer,
#   AssociatedTracks = "GhostTrack",
#   TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
#   JVFName = "JVFOld"
# )

# Jet vertex fraction with selection.
jtm += JetVertexFractionTool(
  "jvf",
  VertexContainer = jtm.vertexContainer,
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
  TrackParticleContainer  = jtm.trackContainer,
  TrackSelector = jtm.trackselloose,
  JVFName = "JVF",
  K_JVFCorrScale = 0.01,
  #Z0Cut = 3.0,
  PUTrkPtCut = 30000.0
)

# Jet vertex tagger.
jtm += JetVertexTaggerTool(
  "jvt",
  VertexContainer = jtm.vertexContainer,
  # AssociatedTracks = "GhostTrack",
  # TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
  # TrackSelector = jtm.trackselloose,
  JVTName = "Jvt",
)

# Jet track info.
jtm += JetTrackMomentsTool(
  "trkmoms",
  VertexContainer = jtm.vertexContainer,
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
  TrackMinPtCuts = [500, 1000],
  TrackSelector = jtm.trackselloose
)

# Jet track vector sum info
jtm += JetTrackSumMomentsTool(
  "trksummoms",
  VertexContainer = jtm.vertexContainer,
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
  RequireTrackPV = True,
  TrackSelector = jtm.trackselloose
)

# Jet cluster info.
jtm += JetClusterMomentsTool(
  "clsmoms",
  DoClsPt = True,
  DoClsSecondLambda = True,
  DoClsCenterLambda = True,
  DoClsSecondR = True
)

jtm += JetVoronoiMomentsTool(
  "voromoms",
  AreaXmin= -5.,
  AreaXmax=  5.,
  AreaYmin= -3.141592,
  AreaYmax=  3.141592
)

# Number of associated muon segments.
#jtm += JetMuonSegmentMomentsTool("muonsegs")

# Isolations.
# Note absence of PseudoJetGetter property means the jet inputs
# are obtained according to the InputType property of the jet.
jtm += JetIsolationTool(
  "jetisol",
  IsolationCalculations = ["IsoDelta:2:SumPt", "IsoDelta:3:SumPt"],
)
jtm += JetIsolationTool(
  "run1jetisol",
  IsolationCalculations = ["IsoKR:11:Perp", "IsoKR:11:Par", "IsoFixedCone:6:SumPt",],
)

# Bad LAr fractions.
jtm += JetLArHVTool("larhvcorr")

# Bad LAr fractions.
jtm += JetECPSFractionTool(
  "ecpsfrac",
)

# Jet origin correction.
jtm += JetOriginCorrectionTool(
  "jetorigincorr",
  VertexContainer = jtm.vertexContainer,
  OriginCorrectedName = "JetOriginConstitScaleMomentum"
)

# Just set the PV without applying origin correction
jtm += JetOriginCorrectionTool(
  "jetorigin_setpv",
  VertexContainer = jtm.vertexContainer,
  OriginCorrectedName = "",
  OnlyAssignPV = True,
)

# Load the xAODCaloEvent dictionary for cluster scale enum
import cppyy
try: cppyy.loadDictionary('xAODCaloEventDict')
except: pass
from ROOT import xAOD
# Touch an unrelated class so the dictionary is loaded
# and therefore the CaloCluster version typedef is recognised
xAOD.CaloVertexedTopoCluster

### Workaround for inability of Gaudi to parse single-element tuple
import GaudiPython.Bindings as GPB
_old_setattr = GPB.iProperty.__setattr__
def _new_setattr(self, name, value):
   if type(value) == tuple:
       value = list(value)
   return _old_setattr(self, name, value)
GPB.iProperty.__setattr__ = _new_setattr
###

jtm += JetConstitFourMomTool(
  "constitfourmom_lctopo",
  JetScaleNames = ["DetectorEtaPhi"],
  AltConstitColls = ["CaloCalTopoClusters"],
  AltConstitScales = [xAOD.CaloCluster.CALIBRATED],
  AltJetScales = [""]
  )

jtm += JetConstitFourMomTool(
  "constitfourmom_emtopo",
  JetScaleNames = ["DetectorEtaPhi","JetLCScaleMomentum"],
  AltConstitColls = ["CaloCalTopoClusters","LCOriginTopoClusters" if jetFlags.useTracks() else "CaloCalTopoClusters"],
  AltConstitScales = [xAOD.CaloCluster.UNCALIBRATED,xAOD.CaloCluster.CALIBRATED],
  AltJetScales = ["",""]
  )

jtm += JetConstitFourMomTool(
  "constitfourmom_pflow",
  JetScaleNames = ["DetectorEtaPhi"],
  AltConstitColls = [""],
  AltConstitScales = [0],
  AltJetScales = ["JetConstitScaleMomentum"]
  )

#--------------------------------------------------------------
# Substructure moment builders.
#--------------------------------------------------------------

# Nsubjettiness
jtm += NSubjettinessTool(
  "nsubjettiness",
  Alpha = 1.0
)

# KtDR
jtm += KtDeltaRTool(
  "ktdr",
  JetRadius = 0.4
)

# Kt-splitter
jtm += KTSplittingScaleTool("ktsplitter")

# Angularity.
jtm += AngularityTool("angularity")

# Dipolarity.
jtm += DipolarityTool("dipolarity", SubJetRadius = 0.3)

# Planar flow.
jtm += PlanarFlowTool("planarflow")

# Kt mass drop.
jtm += KtMassDropTool("ktmassdrop")

# Energy correlations.
jtm += EnergyCorrelatorTool("encorr", Beta = 1.0)

# COM shapes.
jtm += CenterOfMassShapesTool("comshapes")

# Jet pull
jtm += JetPullTool(
  "pull",
  UseEtaInsteadOfY = False,
  IncludeTensorMoments = True
)

# Jet charge
jtm += JetChargeTool("charge", K=1.0)

# Shower deconstruction.
if jtm.haveShowerDeconstructionTool:
  jtm += ShowerDeconstructionTool("showerdec")

#Q jets
jtm += QwTool("qw")

# Remove constituents (useful for truth jets in evgen pile-up file)
jtm += JetConstitRemover("removeconstit")

# Sort jets by pT
# May be deisred after calibration or grooming.
jtm += JetSorter("jetsorter")
