# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileCell creation from TileHit
# with TileHitToCell algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileCellFromHitsGetter ( Configured )  :
    """ This getter module creates an instance of the TileHitToCell
    algorithm to obtain TileCells from the TileHits stored
    in a TileHitContainer"""
    
    _outputType = "TileCellContainer"
    _output = { _outputType : "TileCellCantainerKey" }
    
        
    def configure(self):
        mlog = logging.getLogger( 'TileCellFromHitsGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileSimAlgs.TileCellFromHitsGetter import TileCellFromHitsGetter
            theTileCellFromHitsGetter=TileCellFromHitsGetter()
        except:
            mlog.error("could not get handle to TileCellFromHitsGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileCellFromHitsGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileCellFromHitsGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileCellFromHitsGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileHitToCell                
        except:
            mlog.error("could not import TileSimAlgs.TileHitToCell")
            print traceback.format_exc()
            return False

        theTileHitToCell=TileHitToCell()
        self._TileHitToCellHandle = theTileHitToCell ;

        # Configure TileHitToCell here
        # Check TileSimAlgs_jobOptions.py for full configurability
        theTileHitToCell.TileHitContainer="TileHitCnt"
        theTileHitToCell.TileInfoName="TileInfo"

        # sets output key  
        theTileHitToCell.TileCellContainer=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileHitToCell;
        
        return True

    def TileHitToCellHandle(self):
        return self._TileHitToCellHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



