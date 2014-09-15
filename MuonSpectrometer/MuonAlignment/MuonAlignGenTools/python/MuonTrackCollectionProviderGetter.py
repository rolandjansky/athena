# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file:   MuonTrackCollectionProviderGetter.py
## @brief:  to instantiate the MuonTrackCollectionProvider
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   7/29/2010

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align
from MuonAlignGenTools.MuonAlignmentFlags import MuonAlignmentFlags as mualign

class MuonTrackCollectionProviderGetter ( Configured ) :
    
    _name = 'MuonTrackCollectionProviderGetter'

    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :' )

        from MuonAlignGenTools.MuonAlignRefitToolGetter import MuonAlignRefitToolGetter
        muonAlignRefitTool = MuonAlignRefitToolGetter().MuonAlignRefitToolHandle()

        # create tool instance
        from MuonAlignGenTools.MuonAlignGenToolsConf import Muon__MuonTrackCollectionProvider
        muonTrackCollectionProvider=Muon__MuonTrackCollectionProvider("MuonTrackCollectionProvider",
                                                                      OutputLevel = align.OutputLevel(),
                                                                      InputMuonCol=mualign.MuonContainerName(),
                                                                      InputTrackCol=align.inputTrkCol(),
                                                                      InputIDTrackCol=mualign.inputIDTrackCol(),
                                                                      UseStandaloneTracks = mualign.useStandaloneTracks(),
                                                                      TrackType = mualign.inputTrackCollectionType(),
                                                                      MomentumCut=mualign.momentumCut(),
                                                                      MuonAlignRefitTool=muonAlignRefitTool)
        
        self._muonTrackCollectionProviderHandle = muonTrackCollectionProvider

        mlog.info(" now adding to ToolSvc" )
        from __main__ import ToolSvc
        ToolSvc += self.MuonTrackCollectionProviderHandle()

        mlog.info(" leaving configure")

        return True

    def MuonTrackCollectionProviderHandle(self):
        return self._muonTrackCollectionProviderHandle
