# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from RecExConfig.Configured import Configured
from AthenaCommon.Logging import logging

class TrkTrackPostExecStream(Configured):
    
    def configure(self):

        mlog = logging.getLogger ('TrkTrackPostExecStream.py::configure:')
        mlog.info('entering')
        #
        mlog.info("Now adding to AOD Stream")
        #Add to stream AOD
        streamName = "StreamAOD"
        from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
        stream = MSMgr.GetStream(streamName)
        stream.AddItem("TrackCollection#GSFTracks")
        stream.AddItem("TrackCollection#CombinedMuonTracks")
        mlog.info("Done adding to AOD Stream")
        mlog.info(stream.Stream.ItemList)
        
        return True
