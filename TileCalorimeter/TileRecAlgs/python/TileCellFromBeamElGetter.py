# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileCell creation from TileBeamElem
# with TileBeamElemToCell algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileCellFromBeamElGetter ( Configured )  :
    _outputType = "TileCellContainer"
    _output = { _outputType : "TileTBCellCnt" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileCellFromBeamElGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileRecAlgs.TileCellFromBeamElGetter import TileCellFromBeamElGetter
            theTileCellFromBeamElGetter=TileCellFromBeamElGetter()
        except:
            mlog.error("could not get handle to TileCellFromBeamElGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileCellFromBeamElGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileCellFromBeamElGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileCellFromBeamElGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileRecAlgs.TileRecAlgsConf import TileBeamElemToCell                
        except:
            mlog.error("could not import TileRecAlgs.TileBeamElemToCell")
            print traceback.format_exc()
            return False

        theTileBeamElemToCell=TileBeamElemToCell()
        self._TileBeamElemToCellHandle = theTileBeamElemToCell ;

        # Configure TileBeamElemToCell here
        # Check TileRecAlgs_jobOptions.py for full configurability
        theTileBeamElemToCell.TileBeamElemContainer="TileBeamElemCnt"
        theTileBeamElemToCell.TileInfoName="TileInfo"

        # sets output key  
        theTileBeamElemToCell.TileCellContainer=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileBeamElemToCell;
        
        return True

    def TileBeamElemToCellHandle(self):
        return self._TileBeamElemToCellHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



