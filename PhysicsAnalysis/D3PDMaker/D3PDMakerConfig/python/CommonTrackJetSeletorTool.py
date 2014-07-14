# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def GetCachedTrackSelectorTool():
    from InDetTrackSelectorTool.InDetTrackSelectorToolConf import InDet__InDetDetailedTrackSelectorTool
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += InDet__InDetDetailedTrackSelectorTool( "MyDetailedTrackSelectorTool" )

    ToolSvc.MyDetailedTrackSelectorTool.pTMin                = 500 # MeV
    ToolSvc.MyDetailedTrackSelectorTool.etaMax               = 2.5
    ToolSvc.MyDetailedTrackSelectorTool.nHitBLayer           = 0
    ToolSvc.MyDetailedTrackSelectorTool.nHitPix              = 1 
    ToolSvc.MyDetailedTrackSelectorTool.nHitBLayerPlusPix    = 0
    ToolSvc.MyDetailedTrackSelectorTool.nHitSct              = 6
    ToolSvc.MyDetailedTrackSelectorTool.nHitSi               = 7
    ToolSvc.MyDetailedTrackSelectorTool.nHitTrt              = 0
    ToolSvc.MyDetailedTrackSelectorTool.IPd0Max              = 1.5 # d0 cut
    ToolSvc.MyDetailedTrackSelectorTool.IPz0Max              = 1.5 # z0*sin(theta) cut
    ToolSvc.MyDetailedTrackSelectorTool.z0Max                = 200 # z0 cut
    ToolSvc.MyDetailedTrackSelectorTool.fitChi2OnNdfMax      = 10000  #3
    ToolSvc.MyDetailedTrackSelectorTool.d0significanceMax    = -1.
    ToolSvc.MyDetailedTrackSelectorTool.z0significanceMax    = -1.
    ToolSvc.MyDetailedTrackSelectorTool.Extrapolator = ToolSvc.InDetExtrapolator
    
    ToolSvc.MyDetailedTrackSelectorTool.OutputLevel = 3

    from TrkTrackSummaryTool.AtlasTrackSummaryTool import AtlasTrackSummaryTool
    atst = AtlasTrackSummaryTool()
    ToolSvc += atst
    ToolSvc.MyDetailedTrackSelectorTool.TrackSummaryTool = atst
    from JetSubStructure.JetSubStructureConf import JetSubStructure__CachedTrackSelectorTool
    ToolSvc += JetSubStructure__CachedTrackSelectorTool("CachedTrackSelectorTool")
    ToolSvc.CachedTrackSelectorTool.TrackSelector = ToolSvc.MyDetailedTrackSelectorTool

    return  ToolSvc.CachedTrackSelectorTool
