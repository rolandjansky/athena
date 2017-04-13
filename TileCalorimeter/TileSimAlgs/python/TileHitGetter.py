# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileHitContainer filling  from TileHitVector
# with TileHitVectToCnt algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileHitGetter ( Configured )  :
    """ This getter module creates an instance of the TileHitVectToCnt
    algorithm to fill a TileHitContainer with the TileHits in a list
    of TileHitVectors"""
    
    _outputType = "TileHitContainer"
    _output = { _outputType : "TileHitCnt" }
    
        
    def configure(self):
        mlog = logging.getLogger( 'TileHitGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileSimAlgs.TileHitGetter import TileHitGetter
            theTileHitGetter=TileHitGetter()
        except:
            mlog.error("could not get handle to TileHitGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileHitGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileHitGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileHitGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileHitVecToCnt                
        except:
            mlog.error("could not import TileSimAlgs.TileHitVecToCnt")
            print traceback.format_exc()
            return False

        theTileHitVecToCnt=TileHitVecToCnt()
        self._TileHitVecToCntHandle = theTileHitVecToCnt ;

        # Configure TileHitVecToCnt here
        # Check  TileDigitizationCosmics_jobOptions.py,
        # TileDigitization_jobOptions.py, TileSimAlgs_jobOptions.py and
        # TileTBDigitization_jobOptions.py for full configurability
        theTileHitVecToCnt.TileHitVectors=["TileHitVec" , "MBTSHits" ]
        theTileHitVecToCnt.TileInfoName="TileInfo"
        
        # sets output key  
        theTileHitVecToCnt.TileHitContainer=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileHitVecToCnt;
        
        return True

    def TileHitVecToCntHandle(self):
        return self._TileHitVecToCntHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



