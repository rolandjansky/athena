# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
from AthenaConfiguration.ComponentFactory import CompFactory


def AtlasTrackSummaryToolCfg(flags, name="", **kwargs):
    # Based on AtlasTrackSummaryTool.py
    # FIXME - check all of this once the ID configuration is available, because probably we can simplify this a lot
    from InDetConfig.InDetRecToolConfig import InDetBoundaryCheckToolCfg, InDetTrackSummaryHelperToolCfg

    # Setup Atlas Extrapolator
    result = AtlasExtrapolatorCfg(flags)
    extrapolator = result.getPrimary()
    result.addPublicTool(extrapolator)

    # Setup Association Tool
    # FIXME - use InDetConfig when it works...
    # acc = InDetPrdAssociationToolCfg(flags,
    #                                  name='AtlasPrdAssociationTool',
    #                                  PixelClusterAmbiguitiesMapName="PixelClusterAmbiguitiesMap")
    # atlasPrdAssociationTool = acc.getPrimary()
    atlasPrdAssociationTool = CompFactory.InDet.InDetPRD_AssociationToolGangedPixels(name='AtlasPrdAssociationTool',  
                                                            PixelClusterAmbiguitiesMapName = "PixelClusterAmbiguitiesMap")
    # result.merge(acc)

    # Setup Boundary Check Tool
    acc = InDetBoundaryCheckToolCfg(flags,
                                    name="AtlasBoundaryCheckTool",
                                    UsePixel=flags.Detector.EnablePixel,
                                    UseSCT=flags.Detector.EnableSCT)
    atlasBoundaryCheckTool = acc.getPrimary()
    result.merge(acc)

    # Loading Configurable HoleSearchTool
    from InDetConfig.InDetRecToolConfig import InDetTrackHoleSearchToolCfg
    acc = InDetTrackHoleSearchToolCfg(flags,
                                      name="AtlasHoleSearchTool",
                                      Extrapolator=extrapolator,
                                      BoundaryCheckTool=atlasBoundaryCheckTool)
    atlasHoleSearchTool = acc.getPrimary()
    result.merge(acc)

    # FIXME - need InDet to provide configuration for PixelConditionsSummaryTool
    # Also assuming we don't use DetailedPixelHoleSearch (since it seems to be off in standard workflows)
    acc = InDetTrackSummaryHelperToolCfg(flags,
                                         name="AtlasTrackSummaryHelperTool",
                                         AssoTool=atlasPrdAssociationTool,
                                         DoSharedHits=False,
                                         HoleSearch=atlasHoleSearchTool)
    indet_track_summary_helper_tool = acc.getPrimary()
    result.merge(acc)

    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryHelperToolCfg
    acc = MuonTrackSummaryHelperToolCfg(flags)
    muon_track_summary_helper_tool = acc.getPrimary()
    track_summary_tool = CompFactory.Trk.TrackSummaryTool(name="CombinedMuonTrackSummary",
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
    result.merge(acc)
    result.setPrivateTools(track_summary_tool)
    return result
