# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTrackScoringTools package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

#########################
# InDet configs
#########################

### InDetAmbiScoringTool

def InDetAmbiScoringToolBaseCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(InDetExtrapolatorCfg(flags)))

    if "SummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
        kwargs.setdefault("SummaryTool", acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags)))

    if 'DriftCircleCutTool' not in kwargs:
        from InDetConfig.InDetTrackSelectorToolConfig import InDetTRTDriftCircleCutToolCfg
        kwargs.setdefault("DriftCircleCutTool", acc.popToolsAndMerge(InDetTRTDriftCircleCutToolCfg(flags)))

    have_calo_rois = flags.InDet.Tracking.doBremRecovery and flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROIPhiRZContainerMakerCfg
        acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer", "InDetCaloClusterROIPhiRZ5GeV")

    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("maxEta", flags.InDet.Tracking.ActivePass.maxEta )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel )
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )

    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name, **kwargs))
    return acc

def InDetAmbiScoringToolCfg(flags, name='InDetAmbiScoringTool', **kwargs) :
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minPT )
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxPrimaryImpact )
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.ActivePass.minClusters )
    kwargs.setdefault("minPixel", flags.InDet.Tracking.ActivePass.minPixel )
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.ActivePass.maxHoles )
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.ActivePass.maxPixelHoles )
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.ActivePass.maxSctHoles )
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.ActivePass.maxDoubleHoles )

    return InDetAmbiScoringToolBaseCfg(flags, name + flags.InDet.Tracking.ActivePass.extension, **kwargs)

def InDetAmbiScoringToolSiCfg(flags, name='InDetAmbiScoringToolSi', **kwargs) :
    kwargs.setdefault('DriftCircleCutTool', None)
    return InDetAmbiScoringToolCfg(flags, name, **kwargs)

def InDetExtenScoringToolCfg(flags, name='InDetExtenScoringTool', **kwargs) :
    kwargs.setdefault("minTRTonTrk", flags.InDet.Tracking.ActivePass.minTRTonTrk)
    kwargs.setdefault("minTRTPrecisionFraction", flags.InDet.Tracking.ActivePass.minTRTPrecFrac)
    return InDetAmbiScoringToolCfg(flags, name,  **kwargs)

def InDetTRT_SeededScoringToolCfg(flags, name='InDetTRT_SeededScoringTool', **kwargs) :
    kwargs.setdefault("useAmbigFcn", False)
    kwargs.setdefault("useTRT_AmbigFcn", True)
    kwargs.setdefault("minTRTonTrk", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("minTRTPrecisionFraction", flags.InDet.Tracking.ActivePass.minSecondaryTRTPrecFrac)
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minSecondaryPt)
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxSecondaryImpact)
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.ActivePass.minSecondaryClusters)
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.ActivePass.maxSecondaryHoles)
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.ActivePass.maxSecondaryPixelHoles)
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.ActivePass.maxSecondarySCTHoles)
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.ActivePass.maxSecondaryDoubleHoles)

    return InDetAmbiScoringToolBaseCfg(flags, name, **kwargs)

def InDetTrigAmbiScoringToolCfg(flags, name='InDetTrigMT_AmbiguityScoringTool', **kwargs):
  acc = ComponentAccumulator()

  if "SummaryTool" not in kwargs:
      from TrkConfig.TrkTrackSummaryToolConfig import InDetTrigTrackSummaryToolCfg
      kwargs.setdefault("SummaryTool", acc.popToolsAndMerge(InDetTrigTrackSummaryToolCfg(flags)))

  if "Extrapolator" not in kwargs:
      from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg #TODO using offline, consider porting
      kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(InDetExtrapolatorCfg(flags)))

  if "DriftCircleCutTool" not in kwargs:
      from InDetConfig.InDetTrackSelectorToolConfig import InDetTrigTRTDriftCircleCutToolCfg
      kwargs.setdefault("DriftCircleCutTool", acc.popToolsAndMerge(InDetTrigTRTDriftCircleCutToolCfg(flags)))

  kwargs.setdefault("useAmbigFcn", True )
  kwargs.setdefault("useTRT_AmbigFcn", False )
  kwargs.setdefault("maxZImp", flags.InDet.Tracking.ActivePass.maxZImpact )
  kwargs.setdefault("maxEta", flags.InDet.Tracking.ActivePass.maxEta )
  kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel )
  kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT )
  kwargs.setdefault("doEmCaloSeed", False ) #TODO understand and set appropriately, however current setting is probably a correct one

  acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name+flags.InDet.Tracking.ActivePass.name, **kwargs))
  return acc

### InDetCosmicScoringTool

def InDetCosmicsScoringToolCfg(flags, name='InDetCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("nWeightedClustersMin", flags.InDet.Tracking.ActivePass.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )

    if 'SummaryTool' not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
        InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))
        acc.addPublicTool(InDetTrackSummaryTool)
        kwargs.setdefault("SummaryTool", InDetTrackSummaryTool )

    acc.setPrivateTools(CompFactory.InDet.InDetCosmicScoringTool(name+flags.InDet.Tracking.ActivePass.extension, **kwargs ))
    return acc

def InDetCosmicExtenScoringToolCfg(flags, name='InDetCosmicExtenScoringTool',**kwargs):
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minTRTonTrk )
    return InDetCosmicsScoringToolCfg(flags, name, **kwargs)

def InDetCosmicScoringTool_TRTCfg(flags, name='InDetCosmicScoringTool_TRT',**kwargs):
    acc = ComponentAccumulator()

    if 'SummaryTool' not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolNoHoleSearchCfg
        InDetTrackSummaryToolNoHoleSearch = acc.popToolsAndMerge(InDetTrackSummaryToolNoHoleSearchCfg(flags))
        acc.addPublicTool(InDetTrackSummaryToolNoHoleSearch)
        kwargs.setdefault("SummaryTool", InDetTrackSummaryToolNoHoleSearch)

    kwargs.setdefault("minTRTHits", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)

    acc.setPrivateTools(acc.popToolsAndMerge(InDetCosmicExtenScoringToolCfg(flags, name, **kwargs)))
    return acc

### InDetNNScoringTool

def InDetNNScoringToolBaseCfg(flags, name='InDetNNScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    have_calo_rois = flags.InDet.Tracking.doBremRecovery and flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois :
        from InDetConfig.InDetCaloClusterROISelectorConfig import CaloClusterROIPhiRZContainerMakerCfg
        acc.merge(CaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer", "InDetCaloClusterROIPhiRZ5GeV")

    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(InDetExtrapolatorCfg(flags)))

    if "SummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
        kwargs.setdefault("SummaryTool", acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags)))

    if 'DriftCircleCutTool' not in kwargs:
        from InDetConfig.InDetTrackSelectorToolConfig import InDetTRTDriftCircleCutToolCfg
        kwargs.setdefault("DriftCircleCutTool", acc.popToolsAndMerge(InDetTRTDriftCircleCutToolCfg(flags)))

    kwargs.setdefault("nnCutConfig", "dev/TrackingCP/LRTAmbiNetwork/20200727_225401/nn-config.json" )
    kwargs.setdefault("nnCutThreshold", flags.InDet.Tracking.nnCutLargeD0Threshold )

    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.ActivePass.maxZImpact )
    kwargs.setdefault("maxEta", flags.InDet.Tracking.ActivePass.maxEta )
    kwargs.setdefault("usePixel", flags.InDet.Tracking.ActivePass.usePixel )
    kwargs.setdefault("useSCT", flags.InDet.Tracking.ActivePass.useSCT )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )

    acc.setPrivateTools(CompFactory.InDet.InDetNNScoringTool(name, **kwargs ))
    return acc

def InDetNNScoringToolCfg(flags, name='InDetNNScoringTool', **kwargs) :
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minPT )
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxPrimaryImpact )
    kwargs.setdefault("minSiClusters", flags.InDet.Tracking.ActivePass.minClusters )
    kwargs.setdefault("minPixel", flags.InDet.Tracking.ActivePass.minPixel )
    kwargs.setdefault("maxSiHoles", flags.InDet.Tracking.ActivePass.maxHoles )
    kwargs.setdefault("maxPixelHoles", flags.InDet.Tracking.ActivePass.maxPixelHoles )
    kwargs.setdefault("maxSCTHoles", flags.InDet.Tracking.ActivePass.maxSctHoles )
    kwargs.setdefault("maxDoubleHoles", flags.InDet.Tracking.ActivePass.maxDoubleHoles)

    return InDetNNScoringToolBaseCfg(flags, name+flags.InDet.Tracking.ActivePass.extension, **kwargs )

def InDetNNScoringToolSiCfg(flags, name='InDetNNScoringToolSi', **kwargs) :
    kwargs.setdefault('DriftCircleCutTool', None)
    return InDetNNScoringToolCfg(flags,name, **kwargs)


#########################
# ITk configs
#########################

### InDetAmbiScoringTool

def ITkAmbiScoringToolCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    if 'SummaryTool' not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolCfg
        kwargs.setdefault("SummaryTool", acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags)))

    if 'InDetEtaDependentCutsSvc' not in kwargs:
        from InDetConfig.InDetEtaDependentCutsConfig import ITkEtaDependentCutsSvcCfg
        acc.merge(ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.ActivePass.extension))

    have_calo_rois = flags.ITk.Tracking.doBremRecovery and flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        from InDetConfig.InDetCaloClusterROISelectorConfig import ITkCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(ITkCaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer","ITkCaloClusterROIPhiRZ0GeV")

    kwargs.setdefault("DriftCircleCutTool", None )
    kwargs.setdefault("useAmbigFcn", True )
    kwargs.setdefault("useTRT_AmbigFcn", False )
    kwargs.setdefault("maxEta", flags.ITk.Tracking.ActivePass.maxEta )
    kwargs.setdefault("usePixel", flags.ITk.Tracking.ActivePass.useITkPixel )
    kwargs.setdefault("useSCT", flags.ITk.Tracking.ActivePass.useITkStrip )
    kwargs.setdefault("doEmCaloSeed", have_calo_rois )
    kwargs.setdefault("useITkAmbigFcn", True )
    kwargs.setdefault("minTRTonTrk", 0 )
    kwargs.setdefault("minTRTPrecisionFraction", 0 )

    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name + flags.ITk.Tracking.ActivePass.extension, **kwargs))
    return acc

### InDetCosmicScoringTool

def ITkCosmicsScoringToolCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    if 'SummaryTool' not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolCfg
        ITkTrackSummaryTool = acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags))
        acc.addPublicTool(ITkTrackSummaryTool)
        kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )

    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.ActivePass.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )

    acc.setPrivateTools(CompFactory.InDet.InDetCosmicScoringTool(name+flags.ITk.Tracking.ActivePass.extension, **kwargs))
    return acc
