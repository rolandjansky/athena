# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileTTL1 creation from TileRawChannel
# with TileRawChannelToTTL1 algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileTTL1FromRawChGetter ( Configured )  :
    _outputType = "TileTTL1Container"
    _output = { _outputType : "TileTTL1Cnt" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileTTL1FromRawChGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileRecAlgs.TileTTL1FromRawChGetter import TileTTL1FromRawChGetter
            theTileTTL1FromRawChGetter=TileTTL1FromRawChGetter()
        except:
            mlog.error("could not get handle to TileTTL1FromRawChGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileTTL1FromRawChGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileTTL1FromRawChGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileTTL1FromRawChGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileRecAlgs.TileRecAlgsConf import TileRawChannelToTTL1                
        except:
            mlog.error("could not import TileRecAlgs.TileRawChannelToTTL1")
            print traceback.format_exc()
            return False

        theTileRawChannelToTTL1=TileRawChannelToTTL1()
        self._TileRawChannelToTTL1Handle = theTileRawChannelToTTL1 ;

        # Configure TileRawChannelsToTTL1 here
        theTileRawChannelToTTL1.TileRawChannelContainer="TileRawChannelCnt"
        theTileRawChannelToTTL1.TileInfoName="TileInfo"

        # sets output key  
        theTileRawChannelToTTL1.TileTTL1Container=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileRawChannelToTTL1;
        
        return True

    def TileRawChannelToTTL1Handle(self):
        return self._TileRawChannelToTTL1Handle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



