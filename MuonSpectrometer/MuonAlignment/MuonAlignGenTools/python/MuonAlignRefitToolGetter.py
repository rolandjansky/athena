# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   MuonAlignRefitToolGetter.py
## @brief:  Creates and configures MuonAlignRefitTool.
## @author: Robert Harrington <roberth@bu.edu>
## @date:   11/26/2010

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

## MuonAlignRefitToolGetter creates and configures MuonAlignRefitTool, but 
## does not add to ToolSvc.  User must add to a tool as a private tool, or add to 
#  ToolSvc.
class MuonAlignRefitToolGetter ( Configured ) :

    _name = 'MuonAlignRefitToolGetter'

    ## creates and configures MuonAlignRefitTool using pre-set flags.
    def configure ( self ) :
        mlog = logging.getLogger( self._name+'::configure :' )

        from TrkAlignGenTools.AlignmentTrackFitter import AlignmentTrackFitter
        #from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonRefitTool
        from MuonTrackFinderTools.MuonTrackFinderToolsConf import Muon__MuonRefitTool
        muonRefitTool = Muon__MuonRefitTool("MuonRefitToolForAlignment",
                                            OutputLevel = align.OutputLevel(),
                                            Fitter = AlignmentTrackFitter().fitter())

        from __main__ import ToolSvc
        ToolSvc += muonRefitTool

        # create tool instance
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MuonAlignRefitTool
        muonAlignRefitTool = Muon__MuonAlignRefitTool(OutputLevel = align.OutputLevel())

        combmu = mualign.selectCombinedMuons()

        muonAlignRefitTool.MuonRefitTool         = muonRefitTool

        muonAlignRefitTool.RemoveBIS8Hits        = False
        muonAlignRefitTool.RemoveTGCHits         = False
        muonAlignRefitTool.RemoveTGCEtaHits      = True
        muonAlignRefitTool.RemoveRPCEtaHits      = True
        muonAlignRefitTool.RemoveCSCPhiHits      = True
        muonAlignRefitTool.DeclusterTGCHits      = False
        muonAlignRefitTool.RemoveSmallBarrelHits = False

        muonAlignRefitTool.RemoveCSCHits         = mualign.removeCSCHits()
        muonAlignRefitTool.RemoveBEEHits         = not mualign.alignEE()
        muonAlignRefitTool.RemoveEEHits          = not mualign.alignEE()

        muonAlignRefitTool.RemoveBarrelHits      = not mualign.alignBarrel()
        muonAlignRefitTool.RemoveEndcapHits      = not mualign.alignEndcap()
        muonAlignRefitTool.RemoveTRTHits         = mualign.removeTRTHits()
        muonAlignRefitTool.RemoveScattering      = not align.includeScatterers()

        muonAlignRefitTool.RecreateStartingParameters = not mualign.selectCombinedMuons()
        muonAlignRefitTool.ExtrapolateToMuonEntry     = not mualign.selectCombinedMuons()

        self._MuonAlignRefitToolHandle = muonAlignRefitTool

        mlog.info(" now adding to ToolSvc" )
        from __main__ import ToolSvc
        ToolSvc += self.MuonAlignRefitToolHandle()

        mlog.info(" leaving configure" )

        return True

    ## returns handle to MuonAlignRefitTool
    def MuonAlignRefitToolHandle ( self ) :
        return self._MuonAlignRefitToolHandle
