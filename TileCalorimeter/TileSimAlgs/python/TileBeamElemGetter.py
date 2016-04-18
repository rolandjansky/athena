# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileBeamElem creation from TileHitVector
# with TileTBHitToBeamElem algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback


class TileBeamElemGetter ( Configured )  :
    """ This getter module creates an instance of the TileTBHitToBeamElem
    algorithm to obtain TileBeamElements from the TileTBHits stored
    in a TileTBHitContainer"""
    
    _outputType = "TileBeamElemContainer"
    _output = { _outputType : "TileBeamElemCnt" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileBeamElemGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileSimAlgs.TileBeamElemGetter import TileBeamElemGetter
            theTileBeamElemGetter=TileBeamElemGetter()
        except:
            mlog.error("could not get handle to TileBeamElemGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileBeamElemGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileBeamElemGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileBeamElemGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileTBHitToBeamElem                
        except:
            mlog.error("could not import TileSimAlgs.TileTBHitToBeamElem")
            print traceback.format_exc()
            return False

        theTileTBHitToBeamElem=TileTBHitToBeamElem()
        self._TileTBHitToBeamElemHandle = theTileTBHitToBeamElem ;

        # Configure TileTBHitToBeamElem here
        # Check  TileTBDigitization_jobOptions.py for full configurability
        theTileTBHitToBeamElem.TileTBHitVector="TileTBHits"
        theTileTBHitToBeamElem.TileInfoName="TileInfo"
        
        # sets output key  
        theTileTBHitToBeamElem.TileBeamElemContainer=self.outputKey()        

        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())

        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileTBHitToBeamElem;
        
        return True

    def TileTBHitToBeamElemHandle(self):
        return self._TileTBHitToBeamElemHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



