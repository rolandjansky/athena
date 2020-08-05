# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileDigits creation from TileHit
# with TileDigitsMaker algorithm

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
        except Exception:
            mlog.error("could not get handle to TileDigitsGetter Quit")
            traceback.print_exc()
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
        except Exception:
            mlog.error("could not import TileSimAlgs.TileDigitsMaker")
            traceback.print_exc()
            return False

        theTileDigitsMaker=TileDigitsMaker()
        self._TileDigitsMakerHandle = theTileDigitsMaker

        from Digitization.DigitizationFlags import digitizationFlags
        theTileDigitsMaker.DoHSTruthReconstruction = digitizationFlags.doDigiTruth()
        # Configure TileDigitsMaker here
        # Check TileDigitization_jobOptions.py for full configurability
        theTileDigitsMaker.TileHitContainer="TileHitCnt"
        theTileDigitsMaker.TileHitContainer_DigiHSTruth="TileHitCnt_DigiHSTruth"
        theTileDigitsMaker.TileInfoName="TileInfo"

        theTileDigitsMaker.CalibrationRun=False

        # Save integer numbers in digits vector if not pile-up premixing
        theTileDigitsMaker.IntegerDigits = not digitizationFlags.PileUpPremixing()

        from TileConditions.TileConditionsConf import TileCondToolNoiseSample
        theTileDigitsMaker.TileCondToolNoiseSample = TileCondToolNoiseSample (TileOnlineSampleNoise = '')

        # sets output key  
        if digitizationFlags.PileUpPremixing and 'OverlayMT' in digitizationFlags.experimentalDigi():
            from OverlayCommonAlgs.OverlayFlags import overlayFlags
            theTileDigitsMaker.TileDigitsContainer = overlayFlags.bkgPrefix() + self.outputKey()
        else:
            theTileDigitsMaker.TileDigitsContainer = self.outputKey()


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        # get a handle on topalg
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileDigitsMaker
        
        return True

    def TileDigitsMakerHandle(self):
        return self._TileDigitsMakerHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



