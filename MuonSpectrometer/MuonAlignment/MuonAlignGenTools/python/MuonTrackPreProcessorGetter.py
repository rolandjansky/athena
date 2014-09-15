# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MuonTrackPreProcessorGetter.py
## @brief:  Creates and configures MuonTrackPreProcessorGetter.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   10/19/2009

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## MuonTrackPreProcessorGetter creates and configures MuonTrackPreProcessor, but 
#  does not add to ToolSvc.  User must add to a tool as a private tool, or add to 
#  ToolSvc.
class MuonTrackPreProcessorGetter ( Configured ) :

    _name = 'MuonTrackPreProcessorGetter'

    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )
        
        # create tool instance
        from TrkAlignGenTools.AlignmentTrackFitter import AlignmentTrackFitter
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MuonAlignHelperTool,Muon__MuonTrackPreProcessor
        
        from MuonAlignGenTools.MuonAlignRefitToolGetter import MuonAlignRefitToolGetter
        muonAlignRefitTool = MuonAlignRefitToolGetter().MuonAlignRefitToolHandle()

        muonAlignHelperTool = Muon__MuonAlignHelperTool(OutputLevel = align.OutputLevel())

        from __main__ import ToolSvc
        ToolSvc += muonAlignHelperTool

        muonTrackPreProcessor = Muon__MuonTrackPreProcessor(OutputLevel = align.OutputLevel(),
                                                            TrackFitterTool = AlignmentTrackFitter().fitter(),
                                                            MuonAlignRefitTool  = muonAlignRefitTool)

        if align.useSLFitter() :
            muonTrackPreProcessor.SLTrackFitterTool = AlignmentTrackFitter().slfitter()

        muonTrackPreProcessor.RefitTracks          = align.refitTracks()
        muonTrackPreProcessor.ParticleNumber       = align.particleNumber()
        muonTrackPreProcessor.RunOutlierRemoval    = align.runOutlierRemoval()
        muonTrackPreProcessor.RedoErrorScaling     = mualign.redoErrorScaling()
        muonTrackPreProcessor.pTCorrectTrack       = mualign.pTCorrectTrack()
        muonTrackPreProcessor.RemovePerigeeBeforeRefit = mualign.removePerigeeBeforeRefit()

        muonTrackPreProcessor.ResetScatteringAngles= mualign.resetScatteringAngles()

        muonTrackPreProcessor.ApplyStandardSelectionCuts  = mualign.applyStandardSelectionCuts()
        muonTrackPreProcessor.ApplySelectionCuts          = mualign.applySelectionCuts()
        muonTrackPreProcessor.RequireOneHitPerTubeLayerEC = mualign.requireOneHitPerTubeLayerEC()
        muonTrackPreProcessor.RequireInnerLayerEndcapMdt  = mualign.requireInnerLayerECTracks()
        muonTrackPreProcessor.RequireOuterLayerEndcapMdt  = mualign.requireOuterLayerECTracks()
        muonTrackPreProcessor.RequireBarrelECOverlap      = mualign.requireBarrelECOverlap()
        muonTrackPreProcessor.CutOnBarrel                 = mualign.cutOnBarrelTracks()
        muonTrackPreProcessor.RequireRPCPhiHit            = mualign.requireRPCPhiHit()
        muonTrackPreProcessor.RequireSmallLargeOverlap    = mualign.requireSmallLargeOverlap()

        muonTrackPreProcessor.MuonContainer        = mualign.MuonContainerName()
        muonTrackPreProcessor.SelectCombinedMuons  = mualign.selectCombinedMuons()

        muonTrackPreProcessor.MSID_ptDiffCut       = mualign.msidPtDiffCut()
        muonTrackPreProcessor.chiSqCut             = mualign.cutOnTrackChisquare()
        muonTrackPreProcessor.nAllowedHoles        = mualign.cutOnNumHolesOnTrack()
        muonTrackPreProcessor.nAllowedOutliers     = mualign.cutOnNumOutlierOnTrack()
        muonTrackPreProcessor.MaxDriftSignSum      = mualign.cutOnMaxDriftSignSum()

        self._MuonTrackPreProcessorHandle = muonTrackPreProcessor

        mlog.info(" adding MuonTrackPreProcessor to ToolSvc" )
        ToolSvc += muonAlignHelperTool


        mlog.info(" leaving configure" )

        return True

    def MuonTrackPreProcessorHandle ( self ) :
        return self._MuonTrackPreProcessorHandle
