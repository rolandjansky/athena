# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

#############################################################################################
# BackTracking
#############################################################################################

def ITkAmbiScoringToolBaseCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolCfg
    ITkTrackSummaryTool = acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags))

    have_calo_rois = flags.ITk.Tracking.doBremRecovery and flags.ITk.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo
    if have_calo_rois:
        from InDetConfig.InDetCaloClusterROISelectorConfig import ITkCaloClusterROIPhiRZContainerMakerCfg
        acc.merge(ITkCaloClusterROIPhiRZContainerMakerCfg(flags))
        kwargs.setdefault("EMROIPhiRZContainer","ITkCaloClusterROIPhiRZ0GeV")
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )
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

    if 'InDetEtaDependentCutsSvc' not in kwargs:
        from InDetConfig.InDetEtaDependentCutsConfig import ITkEtaDependentCutsSvcCfg
        acc.merge(ITkEtaDependentCutsSvcCfg(flags))
        kwargs.setdefault("InDetEtaDependentCutsSvc", acc.getService("ITkEtaDependentCutsSvc"+flags.ITk.Tracking.ActivePass.extension))

    the_name = name + flags.ITk.Tracking.ActivePass.extension
    acc.setPrivateTools(CompFactory.InDet.InDetAmbiScoringTool(name = the_name, **kwargs))
    return acc

def ITkCosmicsScoringToolBaseCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    acc = ComponentAccumulator()

    from TrkConfig.TrkTrackSummaryToolConfig import ITkTrackSummaryToolCfg
    ITkTrackSummaryTool = acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags))
    acc.addPublicTool(ITkTrackSummaryTool)

    kwargs.setdefault("nWeightedClustersMin", flags.ITk.Tracking.ActivePass.nWeightedClustersMin )
    kwargs.setdefault("minTRTHits", 0 )
    kwargs.setdefault("SummaryTool", ITkTrackSummaryTool )

    acc.setPrivateTools( CompFactory.InDet.InDetCosmicScoringTool(name, **kwargs ) )
    return acc

def ITkCosmicExtenScoringToolCfg(flags, name='ITkCosmicExtenScoringTool',**kwargs) :
    kwargs.setdefault("nWeightedClustersMin", 0)
    kwargs.setdefault("minTRTHits", 0 )
    return ITkCosmicsScoringToolBaseCfg(flags, name = 'ITkCosmicExtenScoringTool', **kwargs)

def ITkAmbiScoringToolCfg(flags, name='ITkAmbiScoringTool', **kwargs) :
    return ITkAmbiScoringToolBaseCfg(flags, name = name + flags.ITk.Tracking.ActivePass.extension, **kwargs)


#############################################################################################
#TrackingSiPatternConfig
#############################################################################################

def ITkPRDtoTrackMapToolCfg(flags, name='ITkPRDtoTrackMapTool',**kwargs) :
    acc = ComponentAccumulator()
    acc.setPrivateTools(CompFactory.Trk.PRDtoTrackMapTool(name, **kwargs))
    return acc

def ITkCosmicsScoringToolCfg(flags, name='ITkCosmicsScoringTool', **kwargs) :
    return ITkCosmicsScoringToolBaseCfg(flags,
                                        name=name+flags.ITk.Tracking.ActivePass.extension)
