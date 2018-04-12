# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileDigits creation from TileHit
# with TileDigitsMaker algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileDigitsGetter ( Configured )  :
    """ This getter module creates an instance of the TileDigitsMaker
    algorithm to obtain TileDigits from the TileHits stored
    in a TileHitContainer"""
    
    _outputType = "TileDigitsContainer"
    _output = { _outputType : "TileDigitsCnt" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileDigitsGetter::configure:' )
        mlog.info ('entering')        


        # get handle to upstream object
        try:
            from TileSimAlgs.TileDigitsGetter import TileDigitsGetter
            theTileDigitsGetter=TileDigitsGetter()
        except:
            mlog.error("could not get handle to TileDigitsGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileDigitsGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileDigitsGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileDigitsGetter unusable. Continue nevertheless")
                
        from TileConditions.TileInfoConfigurator import TileInfoConfigurator
        tileInfoConfigurator = TileInfoConfigurator()
        tileInfoConfigurator.setupCOOLPHYPULSE()

        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileDigitsMaker                
        except:
            mlog.error("could not import TileSimAlgs.TileDigitsMaker")
            print traceback.format_exc()
            return False

        theTileDigitsMaker=TileDigitsMaker()
        self._TileDigitsMakerHandle = theTileDigitsMaker ;

        # Configure TileDigitsMaker here
        # Check TileDigitization_jobOptions.py for full configurability
        theTileDigitsMaker.TileHitContainer="TileHitCnt"
        theTileDigitsMaker.TileInfoName="TileInfo"

        theTileDigitsMaker.CalibrationRun=False

        # Random number engine
        from Digitization.DigitizationFlags import digitizationFlags
        theTileDigitsMaker.RndmSvc=digitizationFlags.rndmSvc()
        digitizationFlags.rndmSeedList.addSeed("Tile_DigitsMaker", 4789899, 989240512)

        # Save integer numbers in digits vector if not pile-up premixing
        theTileDigitsMaker.IntegerDigits = not digitizationFlags.PileUpPremixing() 

        # sets output key  
        theTileDigitsMaker.TileDigitsContainer=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileDigitsMaker;
        
        return True

    def TileDigitsMakerHandle(self):
        return self._TileDigitsMakerHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



