# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


## @file:   TrackCollectionProviderGetter.py
## @brief:  to instantiate the TrackCollectionProvider
## 	    with the default configuration
## @author: Robert Harrington <roberth@bu.edu>
## @date:   7/29/2010

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
from TrkAlignGenTools.AlignmentFlags import AlignmentFlags as align

class TrackCollectionProviderGetter ( Configured ) :
    
    _name = 'TrackCollectionProviderGetter'

    def configure(self):
        mlog = logging.getLogger( self._name+'::configure :' )

        # create tool instance
        from TrkAlignGenTools.TrkAlignGenToolsConf import Trk__TrackCollectionProvider
        trackCollectionProvider=Trk__TrackCollectionProvider("TrackCollectionProvider",
                                                             OutputLevel = align.OutputLevel(),
                                                             InputTrkCol = align.inputTrkCol())
                                                             
        self._trackCollectionProviderHandle = trackCollectionProvider

        mlog.info(" leaving configure")

        return True

    def TrackCollectionProviderGetterHandle(self):
        return self._trackCollectionProviderHandle
        
