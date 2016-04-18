# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileTTL1 creation from TileHit
# with TileHitToTTL1 algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileTTL1FromHitsGetter ( Configured )  :
    """ This getter module creates an instance of the TileHitToTTL1
    algorithm to obtain TileTTL1 objects from the TileHits stored
    in a TileHitContainer"""
    
    _outputType = "TileTTL1Container"
    _output = { _outputType : "TileTTL1Cnt" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileTTL1FromHitsGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileSimAlgs.TileTTL1FromHitsGetter import TileTTL1FromHitsGetter
            theTileTTL1FromHitsGetter=TileTTL1FromHitsGetter()
        except:
            mlog.error("could not get handle to TileTTL1FromHitsGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileTTL1FromHitsGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileTTL1FromHitsGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileTTL1FromHitsGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileHitToTTL1                
        except:
            mlog.error("could not import TileSimAlgs.TileHitToTTL1")
            print traceback.format_exc()
            return False

        theTileHitToTTL1=TileHitToTTL1()
        self._TileHitToTTL1 = theTileHitToTTL1 ;

        # Configure TileHitToTTL1 here
        # Check TileTTL1_jobOptions.py for full configurability
        theTileHitToTTL1.TileHitContainer="TileHitCnt"
        theTileHitToTTL1.TileInfoName="TileInfo"

        # sets output key  
        theTileHitToTTL1.TileTTL1Container=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileHitToTTL1;
        
        return True

    def TileHitToTTL1(self):
        return self._TileHitToTTL1
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



