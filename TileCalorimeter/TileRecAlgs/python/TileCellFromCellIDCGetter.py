# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileCell copy from TileIDC container to TileCellContainer
# without any corrections using TileCellIDCToCell algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileCellFromCellIDCGetter ( Configured )  :
    _outputType = "TileCellContainer"
    _output = { _outputType : "TileCellContainerKey" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileCellFromCellIDCGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileRecAlgs.TileCellFromCellIDCGetter import TileCellFromCellIDCGetter
            theTileCellFromCellIDCGetter=TileCellFromCellIDCGetter()
        except:
            mlog.error("could not get handle to TileCellFromCellIDCGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileCellFromCellIDCGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileCellFromCellIDCGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileCellFromCellIDCGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileRecAlgs.TileRecAlgsConf import TileCellIDCToCell                
        except:
            mlog.error("could not import TileRecAlgs.TileCellIDCToCell")
            print traceback.format_exc()
            return False

        theTileCellIDCToCell=TileCellIDCToCell()
        self._TileCellIDCToCellHandle = theTileCellIDCToCell ;

        # Configure TileCellIDCToCell here
        # Check TileByteStream/ReadTileCellBS_jobOptions.py for full configurability
        theTileCellIDCToCell.TileCellIDC = "TileCellIDC"
        theTileCellIDCToCell.TileInfoName="TileInfo"

        # sets output key  
        theTileCellIDCToCell.TileCellContainer=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())
        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileCellIDCToCell;
        
        return True

    def TileCellIDCToCellHandle(self):
        return self._TileCellIDCToCellHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



