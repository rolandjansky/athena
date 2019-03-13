# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileRawChannel creation from TileHit
# with TileHitToRawChannel algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileRawChannelFromHitsGetter ( Configured )  :
    """ This getter module creates an instance of the TileHitToRawChannel
    algorithm to obtain TileRawChannels from the TileHits stored
    in a TileHitContainer"""

    _outputType = "TileRawChannelContainer"
    _output = { _outputType : "TileRawChannelCnt" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileRawChannelFromHitsGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileSimAlgs.TileRawChannelFromHitsGetter import TileRawChannelFromHitsGetter
            theTileRawChannelFromHitsGetter=TileRawChannelFromHitsGetter()
        except:
            mlog.error("could not get handle to TileRawChannelFromHitsGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileRawChannelFromHitsGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileRawChannelFromHitsGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileRawChannelFromHitsGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileHitToRawChannel                
        except:
            mlog.error("could not import TileSimAlgs.TileHitToRawChannel")
            print traceback.format_exc()
            return False

        theTileHitToRawChannel=TileHitToRawChannel()
        self._TileHitToRawChannelHandle = theTileHitToRawChannel ;

        # Configure TileHitToRawChannel here
        # Check TileRawChannel_jobOptions.py for full configurability
        theTileHitToRawChannel.TileHitContainer="TileHitCnt"
        theTileHitToRawChannel.TileInfoName="TileInfo"
        theTileHitToRawChannel.DeltaT=-1
        
        # sets output key  
        theTileHitToRawChannel.TileRawChannelContainer=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())

        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileHitToRawChannel;
        
        return True

    def TileHitToRawChannelHandle(self):
        return self._TileHitToRawChannelHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



