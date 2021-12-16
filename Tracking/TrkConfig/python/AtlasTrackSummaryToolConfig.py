# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.AccumulatorCache import AccumulatorCache

@AccumulatorCache
def AtlasTrackSummaryToolCfg(flags, name="AtlasTrackSummaryTool", **kwargs):
    # Based on AtlasTrackSummaryTool.py
    # FIXME - check all of this once the ID configuration is available, because probably we can simplify this a lot

    # Setup Atlas Extrapolator
    result = AtlasExtrapolatorCfg(flags)
    extrapolator = result.getPrimary()
    result.addPublicTool(extrapolator)

    # Setup Association Tool
    from InDetConfig.InDetRecToolConfig import InDetPrdAssociationToolCfg
    atlasPrdAssociationTool = result.getPrimaryAndMerge(InDetPrdAssociationToolCfg(flags,
                                                                              name='AtlasPrdAssociationTool'))
    atlasPrdAssociationTool.addTRToutliers = False

    # Loading Configurable HoleSearchTool
    from InDetConfig.InDetRecToolConfig import InDetTrackHoleSearchToolCfg
    atlasHoleSearchTool = result.getPrimaryAndMerge(InDetTrackHoleSearchToolCfg(flags,
                                                                           name="AtlasHoleSearchTool",
                                                                           Extrapolator=extrapolator))

    # FIXME - need InDet to provide configuration for PixelConditionsSummaryTool
    # Also assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    from InDetConfig.InDetRecToolConfig import InDetTrackSummaryHelperToolCfg
    indet_track_summary_helper_tool = result.getPrimaryAndMerge(InDetTrackSummaryHelperToolCfg(flags,
                                                                                          name="AtlasTrackSummaryHelperTool",
                                                                                          AssoTool=atlasPrdAssociationTool,
                                                                                          DoSharedHits=False,
                                                                                          HoleSearch=atlasHoleSearchTool))

    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryHelperToolCfg
    muon_track_summary_helper_tool = result.getPrimaryAndMerge(MuonTrackSummaryHelperToolCfg(flags))

    track_summary_tool = CompFactory.Trk.TrackSummaryTool(name=name,
                                                          doSharedHits=False,
                                                          doHolesInDet=True,
                                                          doHolesMuon=False,
                                                          AddDetailedInDetSummary=True,
                                                          AddDetailedMuonSummary=True,
                                                          InDetSummaryHelperTool=indet_track_summary_helper_tool,
                                                          TRT_ElectronPidTool=None,
                                                          PixelToTPIDTool=None,
                                                          MuonSummaryHelperTool=muon_track_summary_helper_tool,
                                                          PixelExists=True)
    result.setPrivateTools(track_summary_tool)
    return result
