# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileHitContainer filling  from TileHitVector
# with TileHitVectToCnt algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileHitGetterTool ( Configured )  :
    """ This getter module creates an instance of the TileHitVectToCnt
    algorithm to fill a TileHitContainer with the TileHits in a list
    of TileHitVectors"""
    
    _outputType = "TileHitContainer"
    _output = { _outputType : "TileHitCnt" }
    
        
    def configure(self):
        mlog = logging.getLogger( 'TileHitGetterTool::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileSimAlgs.TileHitGetterTool import TileHitGetterTool
            theTileHitGetterTool=TileHitGetterTool()
        except:
            mlog.error("could not get handle to TileHitGetterTool Quit")
            print traceback.format_exc()
            return False

        if not theTileHitGetterTool.usable():
            if not self.ignoreConfigError():
                mlog.error("TileHitGetterTool unusable. Quit.")
                return False
            else:
                mlog.error("TileHitGetterTool unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileHitVecToCntTool                
        except:
            mlog.error("could not import TileSimAlgs.TileHitVecToCntTool")
            print traceback.format_exc()
            return False

        theTileHitVecToCntTool=TileHitVecToCntTool()
        self._TileHitVecToCntToolHandle = theTileHitVecToCntTool ;

        # Configure TileHitVecToCntTool here
        # Check  TileDigitizationCosmics_jobOptions.py,
        # TileDigitization_jobOptions.py, TileSimAlgs_jobOptions.py and
        # TileTBDigitization_jobOptions.py for full configurability
        theTileHitVecToCntTool.TileHitVectors=["TileHitVec" , "MBTSHits" ]
        theTileHitVecToCntTool.TileInfoName="TileInfo"
        
        # sets output key  
        theTileHitVecToCntTool.TileHitContainer=self.outputKey()        

        # set the range of bunch crossing times that
        # TileHitVectToCntTool is interested in
        theTileHitVecToCntTool.FirstXing = -200
        theTileHitVecToCntTool.LastXing = 150


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence.PileUpToolsAlg.PileUpTools += [theTileHitVecToCntTool]
        return True

    def TileHitVecToCntToolHandle(self):
        return self._TileHitVecToCntToolHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



