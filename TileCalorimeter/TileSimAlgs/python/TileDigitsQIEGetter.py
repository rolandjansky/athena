# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileDigits creation from TileHit
# with TileDigitsQIEMaker algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileDigitsQIEGetter ( Configured )  :
    """ This getter module creates an instance of the TileDigitsMakerQIE
    algorithm to obtain TileDigits from the TileHits stored
    in a TileHitContainer"""
    
    _outputType = "TileDigitsContainer"
    _output = { _outputType : "TileDigitsQIE" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileDigitsQIEGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileSimAlgs.TileDigitsQIEGetter import TileDigitsQIEGetter
            theTileDigitsQIEGetter = TileDigitsQIEGetter()
        except:
            mlog.error("could not get handle to TileDigitsQIEGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileDigitsQIEGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileDigitsQIEGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileDigitsQIEGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileSimAlgs.TileSimAlgsConf import TileDigitsMakerQIE                
        except:
            mlog.error("could not import TileSimAlgs.TileDigitsMakerQIE")
            print traceback.format_exc()
            return False

        theTileDigitsMakerQIE = TileDigitsMakerQIE()
        self._TileDigitsMakerQIEHandle = theTileDigitsMakerQIE ;

        # Configure TileDigitsMakerQIE here
        # Check TileDigitization_jobOptions.py for full configurability
        theTileDigitsMakerQIE.TileHitContainer = "TileHitCnt"
        theTileDigitsMakerQIE.TileInfoName = "TileInfo"
        theTileDigitsMakerQIE.IntegerDigits = True; # save integer numbers in digits vector
        theTileDigitsMakerQIE.CalibrationRun = False

        # Random number engine
        from Digitization.DigitizationFlags import digitizationFlags
        theTileDigitsMakerQIE.RndmSvc = digitizationFlags.rndmSvc()
        digitizationFlags.rndmSeedList.addSeed("Tile_DigitsMakerQIE", 4789899, 989240512)

        # sets output key  
        theTileDigitsMakerQIE.TileDigitsContainer = self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileDigitsMakerQIE;
        
        return True

    def TileDigitsMakerQIEHandle(self):
        return self._TileDigitsMakerQIEHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



