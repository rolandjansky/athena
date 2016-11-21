# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


from JetMomentTools.JetMomentToolsConf import JetCaloQualityTool
from JetMomentTools.JetMomentToolsConf import JetWidthTool
from JetMomentTools.JetMomentToolsConf import JetCaloEnergies
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

def declareDefaultTools():

  from JetRecConfig.JetRecFlags import jetFlags
  from JetRecConfig.JetRecStandardToolManager import jtm
  try:
    from JetMomentTools.JetMomentToolsConf import JetCaloCellQualityTool
    jtm.haveJetCaloCellQualityTool = True
  except ImportError:
    jtm.haveJetCaloCellQualityTool = False

  try:
    from JetMomentTools.JetMomentToolsConf import JetJetBadChanCorrTool
    jtm.haveJetBadChanCorrTool = True
  except ImportError:
    jtm.haveJetBadChanCorrTool = False

  from JetRecTools.JetRecToolsConf import TrackVertexAssociationTool
  from JetRec.JetRecConf import PseudoJetGetter
  from JetRecTools.JetRecToolsConf import TrackPseudoJetGetter
  from InDetTrackSelectionTool.InDetTrackSelectionToolConf import InDet__InDetTrackSelectionTool
  from JetRecTools.JetRecToolsConf import JetTrackSelectionTool
  from JetRecTools.JetRecToolsConf import SimpleJetTrackSelectionTool

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
  jtm += JetWidthTool("width", WeightPFOToolEM=jtm.pflowweighter, WeightPFOToolLC=jtm.pflowweighterLC)

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

  # Bad LAr fractions.
  jtm += JetECPSFractionTool(
    "ecpsfrac",
  )

  #--------------------------------------------------------------
  # Track-vertex association.
  #--------------------------------------------------------------
  from TrackVertexAssociationTool.TrackVertexAssociationToolConf import CP__TightTrackVertexAssociationTool
  jtm += CP__TightTrackVertexAssociationTool("jetTighTVAtool", dzSinTheta_cut=3, doPV=True)

  jtm += TrackVertexAssociationTool(
    "tvassoc",
    TrackParticleContainer  = jtm.trackContainer,
    TrackVertexAssociation  = "JetTrackVtxAssoc",
    VertexContainer         = jtm.vertexContainer,
    TrackVertexAssoTool     = jtm.jetTighTVAtool,
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
  # Track selection.
  #--------------------------------------------------------------

  # This is the InDet loose selection from
  # https://twiki.cern.ch/twiki/bin/view/AtlasProtected/InDetTrackingPerformanceGuidelines
  # October 28, 2014
  #jtm += InDet__InDetDetailedTrackSelectionTool(
  jtm += InDet__InDetTrackSelectionTool(
    "trk_trackselloose",
    CutLevel                = "Loose"
  )

  jtm += JetTrackSelectionTool(
    "trackselloose",
    InputContainer  = jtm.trackContainer,
    OutputContainer = "JetSelectedTracks",
    Selector        = jtm.trk_trackselloose
  )

  jtm += InDet__InDetTrackSelectionTool(
    "trk_trackselloose_trackjets",
    CutLevel                = "Loose"
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
    GhostScale = 1e-20
  )


  # Jet vertex fraction.
  jtm += JetVertexFractionTool(
    "jvfold",
    VertexContainer = jtm.vertexContainer,
    AssociatedTracks = "GhostTrack",
    TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
    JVFName = "JVFOld"
  )

  # Jet vertex fraction with selection.
  jtm += JetVertexFractionTool(
    "jvf",
    VertexContainer = jtm.vertexContainer,
    AssociatedTracks = "GhostTrack",
    TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
    TrackSelector = jtm.trackselloose,
    JVFName = "JVF"
  )

  # Jet vertex tagger.
  jtm += JetVertexTaggerTool(
    "jvt",
    VertexContainer = jtm.vertexContainer,
    TrackParticleContainer  = jtm.trackContainer,
    AssociatedTracks = "GhostTrack",
    TrackVertexAssociation = jtm.tvassoc.TrackVertexAssociation,
    TrackSelector = jtm.trackselloose,
    JVTName = "Jvt",
    K_JVFCorrScale = 0.01,
    Z0Cut = 3.0,
    PUTrkPtCut = 30000.0
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

