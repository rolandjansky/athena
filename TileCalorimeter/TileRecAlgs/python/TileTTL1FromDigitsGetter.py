# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileTTL1 creation from TileDigits
# with TileDigitsToTTL1 algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileTTL1FromDigitsGetter ( Configured )  :
    _outputType = "TileTTL1Container"
    _output = { _outputType : "TileTTL1Cnt" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileTTL1FromDigitsGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileRecAlgs.TileTTL1FromDigitsGetter import TileTTL1FromDigitsGetter
            theTileTTL1FromDigitsGetter=TileTTL1FromDigitsGetter()
        except:
            mlog.error("could not get handle to TileTTL1FromDigitsGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileTTL1FromDigitsGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileTTL1FromDigitsGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileTTL1FromDigitsGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileRecAlgs.TileRecAlgsConf import TileDigitsToTTL1                
        except:
            mlog.error("could not import TileRecAlgs.TileDigitsToTTL1")
            print traceback.format_exc()
            return False

        theTileDigitsToTTL1=TileDigitsToTTL1()
        self._TileDigitsToTTL1Handle = theTileDigitsToTTL1 ;

        # Configure TileDigitsToTTL1 here
        theTileDigitsToTTL1.TileDigitsContainer="TileDigitsCnt"
        theTileDigitsToTTL1.TileInfoName="TileInfo"

        # sets output key  
        theTileDigitsToTTL1.TileTTL1Container=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileDigitsToTTL1;
        
        return True

    def TileDigitsToTTL1Handle(self):
        return self._TileDigitsToTTL1Handle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



