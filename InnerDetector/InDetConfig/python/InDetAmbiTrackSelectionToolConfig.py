# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetAmbiTrackSelectionTool package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType, LHCPeriod

def InDetAmbiTrackSelectionToolCfg(flags, name = "InDetAmbiTrackSelectionTool", **kwargs) :
    acc = ComponentAccumulator()

    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    #
    # --- load InnerDetector TrackSelectionTool
    #
    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        AmbiTrackSelectionTool = CompFactory.InDet.InDetDenseEnvAmbiTrackSelectionTool
    else:
        AmbiTrackSelectionTool = CompFactory.InDet.InDetAmbiTrackSelectionTool

    if 'UseParameterization' in kwargs and kwargs.get('UseParameterization',False) :
        from InDetConfig.InDetTrackSelectorToolConfig import InDetTRTDriftCircleCutToolCfg
        kwargs.setdefault("DriftCircleCutTool", acc.popToolsAndMerge(InDetTRTDriftCircleCutToolCfg(flags)))

    from InDetConfig.InDetAssociationToolsConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    kwargs.setdefault("AssociationTool" , InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minHits"         , flags.InDet.Tracking.ActivePass.minClusters)
    kwargs.setdefault("minNotShared"    , flags.InDet.Tracking.ActivePass.minSiNotShared)
    kwargs.setdefault("maxShared"       , flags.InDet.Tracking.ActivePass.maxShared)
    kwargs.setdefault("minTRTHits"      , 0) # used for Si only tracking !!!
    kwargs.setdefault("UseParameterization" , False)
    kwargs.setdefault("Cosmics"             , flags.Beam.Type is BeamType.Cosmics and flags.InDet.Tracking.ActivePass.extension != "DBM")
    kwargs.setdefault("doPixelSplitting"    , flags.InDet.Tracking.doPixelClusterSplitting and flags.InDet.Tracking.ActivePass.extension != "DBM")

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        kwargs.setdefault("sharedProbCut"             , flags.InDet.Tracking.pixelClusterSplitProb1)
        kwargs.setdefault("sharedProbCut2"            , flags.InDet.Tracking.pixelClusterSplitProb2)
        kwargs.setdefault("minSiHitsToAllowSplitting" , 8 if flags.GeoModel.Run is LHCPeriod.Run1 else 9)
        kwargs.setdefault("minUniqueSCTHits"          , 4)
        kwargs.setdefault("minTrackChi2ForSharedHits" , 3)
        kwargs.setdefault("minPtSplit"                , 1000)       #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("maxSharedModulesInROI"     , 3)     #Maximum number of shared modules for tracks in ROI
        kwargs.setdefault("minNotSharedInROI"         , 2)     #Minimum number of unique modules for tracks in ROI
        kwargs.setdefault("minSiHitsToAllowSplittingInROI" , 8)  #Minimum number of Si hits to allow splittings for tracks in ROI
        kwargs.setdefault("phiWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidth"                  , 0.05)     #Split cluster ROI size
        kwargs.setdefault("doEmCaloSeed"              , flags.InDet.Tracking.doCaloSeededAmbi and flags.Detector.EnableCalo)   #Only split in cluster in region of interest
        kwargs.setdefault("EMROIPhiRZContainer"       , "InDetCaloClusterROIPhiRZ10GeV")
        if flags.InDet.Tracking.doCaloSeededAmbi:
            from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROIPhiRZContainerMakerCfg
            acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("doHadCaloSeed"             , flags.InDet.Tracking.doCaloSeededAmbi)   #Do special cuts in region of interest
        kwargs.setdefault("HadROIPhiRZContainer"      , "InDetHadCaloClusterROIPhiRZBjet")
        if flags.InDet.Tracking.doCaloSeededAmbi:
            from InDetConfig.InDetCaloClusterROISelectorConfig import HadCaloClusterROIPhiRZContainerMakerCfg
            acc.merge(HadCaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("minPtConv"                 , 10000)   #Only allow split clusters on track withe pt greater than this MeV
        kwargs.setdefault("minPtBjetROI"              , 10000)
        kwargs.setdefault("phiWidthEM"                , 0.05)     #Split cluster ROI size
        kwargs.setdefault("etaWidthEM"                , 0.05)     #Split cluster ROI size

    elif flags.InDet.Tracking.ActivePass.extension == "DBM":
        kwargs.setdefault("maxShared"             , 1000)
        kwargs.setdefault("maxTracksPerSharedPRD" , 2)
        kwargs.setdefault("minHits"               , 0)
        kwargs.setdefault("minNotShared"          , 0)
        kwargs.setdefault("minScoreShareTracks"   , 0.0)
        kwargs.setdefault("minTRTHits"            , 0)
        kwargs.setdefault("sharedProbCut"         , 0.1)
    else:
        kwargs.setdefault("sharedProbCut", 0.10)

    InDetAmbiTrackSelectionTool = AmbiTrackSelectionTool(name = name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiTrackSelectionTool)
    return acc

def InDetTRTAmbiTrackSelectionToolCfg(flags, name = 'InDetTRT_SeededAmbiTrackSelectionTool', **kwargs):
    acc = ComponentAccumulator()

    if "DriftCircleCutTool" not in kwargs:
        from InDetConfig.InDetTrackSelectorToolConfig import InDetTRTDriftCircleCutToolCfg
        kwargs.setdefault("DriftCircleCutTool", acc.popToolsAndMerge(InDetTRTDriftCircleCutToolCfg(flags)))

    if "AssociationTool" not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))

    kwargs.setdefault("minScoreShareTracks", -1.) # off !
    kwargs.setdefault("minHits", flags.InDet.Tracking.ActivePass.minSecondaryClusters)
    kwargs.setdefault("minNotShared", flags.InDet.Tracking.ActivePass.minSecondarySiNotShared)
    kwargs.setdefault("maxShared", flags.InDet.Tracking.ActivePass.maxSecondaryShared)
    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("UseParameterization", flags.InDet.Tracking.ActivePass.useParameterizedTRTCuts)
    kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("doPixelSplitting", flags.InDet.Tracking.doPixelClusterSplitting)

    acc.setPrivateTools(CompFactory.InDet.InDetAmbiTrackSelectionTool(name, **kwargs))
    return acc

def InDetTrigTrackSelectionToolCfg(flags, name = 'InDetTrigAmbiTrackSelectionTool', **kwargs):
  acc = ComponentAccumulator()

  #TODO add configurations fro beamgas and cosmic see: trackSelectionTool_getter
  from InDetConfig.InDetAssociationToolsConfig import TrigPRDtoTrackMapToolGangedPixelsCfg
  kwargs.setdefault("DriftCircleCutTool", None)
  kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(TrigPRDtoTrackMapToolGangedPixelsCfg(flags)))
  kwargs.setdefault("minHits", flags.InDet.Tracking.ActivePass.minClusters)
  kwargs.setdefault("minNotShared", flags.InDet.Tracking.ActivePass.minSiNotShared)
  kwargs.setdefault("maxShared", flags.InDet.Tracking.ActivePass.maxShared)
  kwargs.setdefault("minTRTHits", 0) # used for Si only tracking !!!
  kwargs.setdefault("Cosmics", False) #there is a different instance
  kwargs.setdefault("UseParameterization", False)
  
  acc.addPublicTool(CompFactory.InDet.InDetAmbiTrackSelectionTool(name, **kwargs), primary=True)
  return acc

def ITkAmbiTrackSelectionToolCfg(flags, name = "ITkAmbiTrackSelectionTool", **kwargs) :
    acc = ComponentAccumulator()

    # ------------------------------------------------------------
    #
    # ---------- Ambiguity solving
    #
    # ------------------------------------------------------------

    #
    # --- load InnerDetector TrackSelectionTool
    #

    from InDetConfig.InDetAssociationToolsConfig import ITkPRDtoTrackMapToolGangedPixelsCfg
    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(ITkPRDtoTrackMapToolGangedPixelsCfg(flags))

    kwargs.setdefault("DriftCircleCutTool", None)
    kwargs.setdefault("AssociationTool" , ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("minTRTHits"      , 0) # used for Si only tracking !!!
    kwargs.setdefault("UseParameterization" , False)
    kwargs.setdefault("Cosmics"             , flags.Beam.Type is BeamType.Cosmics)
    kwargs.setdefault("doPixelSplitting"    , flags.ITk.Tracking.doPixelClusterSplitting )
    kwargs.setdefault("doITk" , True)

    kwargs.setdefault("sharedProbCut"             , flags.ITk.Tracking.pixelClusterSplitProb1)
    kwargs.setdefault("sharedProbCut2"            , flags.ITk.Tracking.pixelClusterSplitProb2)
    kwargs.setdefault("minSiHitsToAllowSplitting" , 9)
    kwargs.setdefault("minUniqueSCTHits"          , 4)
    kwargs.setdefault("minTrackChi2ForSharedHits" , 3)
    kwargs.setdefault("minPtSplit"                , 1000)       #Only allow split clusters on track withe pt greater than this MeV
    kwargs.setdefault("maxSharedModulesInROI"     , 3)     #Maximum number of shared modules for tracks in ROI
    kwargs.setdefault("minNotSharedInROI"         , 2)     #Minimum number of unique modules for tracks in ROI
    kwargs.setdefault("minSiHitsToAllowSplittingInROI" , 8)  #Minimum number of Si hits to allow splittings for tracks in ROI
    kwargs.setdefault("phiWidth"                  , 0.05)     #Split cluster ROI size
    kwargs.setdefault("etaWidth"                  , 0.05)     #Split cluster ROI size
    kwargs.setdefault("doEmCaloSeed"              , flags.ITk.Tracking.doCaloSeededAmbi)   #Only split in cluster in region of interest
    kwargs.setdefault("EMROIPhiRZContainer"       , "ITkCaloClusterROIPhiRZ10GeV")
    if flags.Detector.EnableCalo:
        from InDetConfig.InDetCaloClusterROISelectorConfig import ITkCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(ITkCaloClusterROIPhiRZContainerMakerCfg(flags))
    kwargs.setdefault("doHadCaloSeed"             , flags.ITk.Tracking.doCaloSeededAmbi)   #Do special cuts in region of interest
    kwargs.setdefault("HadROIPhiRZContainer"      , "ITkHadCaloClusterROIPhiRZBjet")
    if flags.Detector.EnableCalo:
        from InDetConfig.InDetCaloClusterROISelectorConfig import ITkHadCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(ITkHadCaloClusterROIPhiRZContainerMakerCfg(flags))
    kwargs.setdefault("minPtConv"                 , 10000)   #Only allow split clusters on track withe pt greater than this MeV
    kwargs.setdefault("minPtBjetROI"              , 10000)
    kwargs.setdefault("phiWidthEM"                , 0.05)     #Split cluster ROI size
    kwargs.setdefault("etaWidthEM"                , 0.05)     #Split cluster ROI size

    if 'InDetEtaDependentCutsSvc' not in kwargs :
        from InDetConfig.InDetEtaDependentCutsConfig import ITkEtaDependentCutsSvcCfg
        acc.merge(ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.ActivePass.extension))

    ITkAmbiTrackSelectionTool = CompFactory.InDet.InDetDenseEnvAmbiTrackSelectionTool(name = name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(ITkAmbiTrackSelectionTool)
    return acc
