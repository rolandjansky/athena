# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileCell creation from TileBeamElem
# with TileRawChannelToHit algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileHitFromRawChGetter ( Configured )  :
    _outputType = "TileHitContainer"
    _output = { _outputType : "TileHitVec" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileHitFromRawChGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileRecAlgs.TileHitFromRawChGetter import TileHitFromRawChGetter
            theTileHitFromRawChGetter=TileHitFromRawChGetter()
        except:
            mlog.error("could not get handle to TileHitFromRawChGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileHitFromRawChGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileHitFromRawChGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileHitFromRawChGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileRecAlgs.TileRecAlgsConf import TileRawChannelToHit                
        except:
            mlog.error("could not import TileRecAlgs.TileRawChannelToHit")
            print traceback.format_exc()
            return False

        theTileRawChannelToHit=TileRawChannelToHit()
        self._TileRawChannelToHit = theTileRawChannelToHit ;

        # Configure TileRawChannelToHit here
        # Check TileRecAlgs_jobOptions.py for full configurability
        theTileRawChannelToHit.TileRawChannelContainer="TileRawChannelCnt"
        theTileRawChannelToHit.TileInfoName="TileInfo"
        theTileRawChannelToHit.UseSamplFract = False
        
        # sets output key  
        theTileRawChannelToHit.TileHitContainer=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileRawChannelToHit;
        
        return True

    def TileRawChannelToHit(self):
        return self._TileRawChannelToHit
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



