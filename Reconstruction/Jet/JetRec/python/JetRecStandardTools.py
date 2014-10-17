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
# Execute this file to add the definitions to JetRecStandard.jtm, e.g.
# python> import JetRec.JetRecStandardTools

# Import the jet flags.
from JetRec.JetRecFlags import jetFlags

if not "UseTriggerStore " in locals():
  UseTriggerStore = False

# get levels defined VERBOSE=1 etc.
from GaudiKernel.Constants import *

from JetRec.JetRecStandard import jtm
from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
from PFlowUtils.PFlowUtilsConf import CP__RetrievePFOTool as RetrievePFOTool
from JetRecTools.JetRecToolsConf import TrackPseudoJetGetter
from JetRecTools.JetRecToolsConf import JetDetailedTrackSelectorTool
from JetRecTools.JetRecToolsConf import JetTrackSelectionTool
from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
from JetRecTools.JetRecToolsConf import MissingCellListTool
from JetRecTools.JetRecToolsConf import PFlowPseudoJetGetter
from JetRec.JetRecConf import JetRecTool
from JetRec.JetRecConf import PseudoJetGetter
from JetRec.JetRecConf import MuonSegmentPseudoJetGetter
from JetRec.JetRecConf import JetFromPseudojet
from JetSimTools.JetSimToolsConf import JetTruthParticleSelectorTool
from JetSimTools.JetSimToolsConf import TruthPseudoJetGetter
from JetMomentTools.JetMomentToolsConf import JetCaloQualityTool
from JetMomentTools.JetMomentToolsConf import JetWidthTool
from JetMomentTools.JetMomentToolsConf import JetCaloEnergies
from JetMomentTools.JetMomentToolsConf import JetBadChanCorrTool
from JetMomentTools.JetMomentToolsConf import JetVertexFractionTool
from JetMomentTools.JetMomentToolsConf import JetTrackMomentsTool
#from JetMomentTools.JetMomentToolsConf import JetMuonSegmentMomentsTool
from JetMomentTools.JetMomentToolsConf import JetIsolationTool
from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
from JetMomentTools.JetMomentToolsConf import JetLArHVTool
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
from JetSubStructureMomentTools.JetSubStructureMomentToolsConf import ShowerDeconstructionTool

#--------------------------------------------------------------
# Track selection.
#--------------------------------------------------------------

if jetFlags.useInDetTrackSelection:
  jtm += InDet__InDetDetailedTrackSelectorTool(
    "trk_tracksel",
    TrackSummaryTool     = None,
    z0Max                = 200,
    d0significanceMax    = 100.0,
    pTMin                = 500.0,
    etaMax               = 2.5,
    fitChi2OnNdfMax      = 5.0,
    nHitPix              = 1,
    nHitSct              = 6,
    nHitTrt              = 0,
    nHitSi               = 6,
    IPz0Max              = 1.0e15,
    nHitBLayer           = 0,
    nHitBLayerPlusPix    = 0
  )

else:
  jtm += JetDetailedTrackSelectorTool("trk_tracksel")

jtm += JetTrackSelectionTool(
  "tracksel",
  InputContainer  = jtm.trackContainer,
  OutputContainer = "JetSelectedTracks",
  Selector        = jtm.trk_tracksel
)

#--------------------------------------------------------------
# Track-vertex association.
#--------------------------------------------------------------

jtm += TrackVertexAssociationTool(
  "tvassoc",
  TrackParticleContainer  = jtm.trackContainer,
  TrackVertexAssociation  = "JetTrackVtxAssoc",
  VertexContainer         = jtm.vertexContainer,
  MaxTransverseDistance   = 1.0,
  MaxLongitudinalDistance = 1.0
)

#--------------------------------------------------------------
# Truth selection.
#--------------------------------------------------------------

if jetFlags.useTruth:
  jtm += JetTruthParticleSelectorTool(
    "truthsel",
    SelectionMode = "StableNoMuonNoNu"
  )

  jtm += JetTruthParticleSelectorTool(
    "truthselwz",
    SelectionMode = "NoWZDecay"
  )

#--------------------------------------------------------------
# Pseudojet builders.
#--------------------------------------------------------------

# Clusters.
jtm += PseudoJetGetter(
  "lcget",
  InputContainer = "CaloCalTopoCluster",
  Label = "LCTopo",
  OutputContainer = "PseudoJetLCTopo",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

# EM clusters.
jtm += PseudoJetGetter(
  "emget",
  InputContainer = "CaloCalTopoCluster",
  Label = "EMTopo",
  OutputContainer = "PseudoJetEMTopo",
  SkipNegativeEnergy = True,
  GhostScale = 0.0
)

# Tracks.
jtm += TrackPseudoJetGetter(
  "trackget",
  InputContainer = jtm.tracksel.OutputContainer,
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
  GhostScale = 1e-20
)

# Muon segments
jtm += MuonSegmentPseudoJetGetter(
  "gmusegget",
  InputContainer = "MuonSegments",
  Label = "GhostMuonSegment",
  OutputContainer = "PseudoJetGhostMuonSegment",
  Pt = 1.e-20
)

# Retriever for pflow objects.
jtm += RetrievePFOTool("pflowretriever")

# EM-scale pflow.
jtm += PFlowPseudoJetGetter(
  "empflowget",
  Label = "EMPFlow",
  OutputContainer = "PseudoJetEMPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  InputIsEM = True,
  CalibratePFO = False,
  SkipNegativeEnergy = True,
)

# Calibrated EM-scale pflow.
jtm += PFlowPseudoJetGetter(
  "emcpflowget",
  Label = "EMCPFlow",
  OutputContainer = "PseudoJetEMCPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  InputIsEM = True,
  CalibratePFO = True,
  SkipNegativeEnergy = True,
)

# LC-scale pflow.
jtm += PFlowPseudoJetGetter(
  "lcpflowget",
  Label = "LCPFlow",
  OutputContainer = "PseudoJetLCPFlow",
  RetrievePFOTool = jtm.pflowretriever,
  InputIsEM = False,
  CalibratePFO = False,
  SkipNegativeEnergy = True,
)

# AntiKt3 track jets.
jtm += PseudoJetGetter(
  "gakt3trackget", # give a unique name
  InputContainer = jetFlags.containerNamePrefix() + "AntiKt3PV0TrackJets", # SG key
  Label = "GhostAntiKt3TrackJet",   # this is the name you'll use to retrieve associated ghosts
  OutputContainer = "PseudoJetGhostAntiKt3TrackJet",
  SkipNegativeEnergy = True,
  GhostScale = 1.e-20,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
)

# AntiKt4 track jets.
jtm += PseudoJetGetter(
  "gakt4trackget", # give a unique name
  InputContainer = jetFlags.containerNamePrefix() + "AntiKt4PV0TrackJets", # SG key
  Label = "GhostAntiKt4TrackJet",   # this is the name you'll use to retrieve associated ghosts
  OutputContainer = "PseudoJetGhostAntiKt4TrackJet",
  SkipNegativeEnergy = True,
  GhostScale = 1.e-20,   # This makes the PseudoJet Ghosts, and thus the reco flow will treat them as so.
)

# Truth.
if jetFlags.useTruth:
  jtm += TruthPseudoJetGetter(
    "truthget",
    Label = "Truth",
    InputContainer = "TruthParticle",
    OutputContainer = "PseudoJetTruth",
    TruthSelector = jtm.truthsel,
    GhostScale = 0.0,
    SkipNegativeEnergy = True,

  )
  jtm += TruthPseudoJetGetter(
    "truthwzget",
    Label = "TruthWZ",
    InputContainer = "TruthParticle",
    OutputContainer = "PseudoJetTruthWZ",
    TruthSelector = jtm.truthselwz,
    GhostScale = 0.0,
    SkipNegativeEnergy = True,
    
  )
  jtm += TruthPseudoJetGetter(
    "gtruthget",
    Label = "GhostTruth",
    InputContainer = "TruthParticle",
    OutputContainer = "PseudoJetGhostTruth",
    TruthSelector = jtm.truthsel,
    GhostScale = 1.e-20,
    SkipNegativeEnergy = True,

  )

  # Truth flavor
  for ptype in jetFlags.truthFlavorTags():
    jtm += PseudoJetGetter(
      "gtruthget_" + ptype,
      InputContainer = "TruthLabel" + ptype,
      Label = "Ghost" + ptype,
      OutputContainer = "PseudoJetGhost" + ptype,
      SkipNegativeEnergy = True,
      GhostScale = 1e-20
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

# Quality.
jtm += JetCaloQualityTool(
  "caloqual_cluster",
  DoFracSamplingMax = True,
  DoN90 = True,
  DoLArQuality = True,
  LArQualityCut = 4000,
  TileQualityCut = 254,
  DoTiming = True,
  TimingCellTimeCuts = [],
  TimingClusterTimeCuts = [5, 10],
  DoNegativeE = True,
  DoHECQuality = True,
  DoAverageLArQF = True,
  DoJetCentroid = True,
  ComputeVariableFromCluster = True
)

# Jet width.
jtm += JetWidthTool("width")

# Calo layer energies.
jtm += JetCaloEnergies("jetens")

# Read in missing cell map (needed for the following)

def missingCellFileReader(): 
  import os
  dataPathList = os.environ[ 'DATAPATH' ].split(os.pathsep)
  dataPathList.insert(0, os.curdir)
  from AthenaCommon.Utils.unixtools import FindFile
  RefFileName = FindFile( "JetBadChanCorrTool.root" ,dataPathList, os.R_OK )
  from AthenaCommon.AppMgr import ServiceMgr
  if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
  ServiceMgr.THistSvc.Input += ["JetBadChanCorrTool DATAFILE=\'%s\' OPT=\'READ\'" % RefFileName]
  missingCellFileReader.called = True 

missingCellFileReader()

jtm += MissingCellListTool(
  "missingcells",
  AddCellList = [],
  RemoveCellList = [],
  AddBadCells = True,
  DeltaRmax = 1.0,
  AddCellFromTool = False,
  LArMaskBit = 608517,
  TileMaskBit = 1,
  MissingCellMapName = "MissingCaloCellsMap"
)

# Bad channel corrections from cells
jtm += JetBadChanCorrTool(
  "bchcorrcell",
  NBadCellLimit = 10000,
  StreamName = "/JetBadChanCorrTool/",
  ProfileName = "JetBadChanCorrTool.root",
  ProfileTag = "",
  UseCone = True,
  UseCalibScale = False,
  MissingCellMap = "MissingCaloCellsMap",
  ForceMissingCellCheck = False,
  UseClusters = False,
)
  
# Bad channel corrections from clusters
jtm += JetBadChanCorrTool(
  "bchcorrclus",
  NBadCellLimit = 0,
  StreamName = "",
  ProfileName = "",
  ProfileTag = "",
  UseCone = True,
  UseCalibScale = False,
  MissingCellMap = "",
  ForceMissingCellCheck = False,
  UseClusters = True
)

# Jet vertex fraction.
jtm += JetVertexFractionTool(
  "jvf",
  VertexContainer = jtm.vertexContainer,
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation
)

# Jet track info.
jtm += JetTrackMomentsTool(
  "trkmoms",
  VertexContainer = jtm.vertexContainer,
  AssociatedTracks = "GhostTrack",
  TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
  TrackMinPtCuts = [500, 1000]
)

# Number of associated muon segments.
#jtm += JetMuonSegmentMomentsTool("muonsegs")

# Isolations.
jtm += JetIsolationTool(
  "isolation",
  IsolationCalculations = ["IsoKR:11:Perp", "IsoKR:11:Par", "IsoFixedCone:6:SumPt",],
  ConstituentContainer = "CaloCalTopoCluster",
)

# Bad LAr fractions.
theLArHVCorrTool=LArHVCorrToolDefault()
jtm += theLArHVCorrTool
jtm += JetLArHVTool("larhvcorr", HVCorrTool=theLArHVCorrTool)

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
jtm += JetPullTool("pull", UseEtaInsteadOfY=False)

# Jet charge
jtm += JetChargeTool("charge", K=1.0)

# Shower deconstruction.
jtm += ShowerDeconstructionTool("showerdec")


