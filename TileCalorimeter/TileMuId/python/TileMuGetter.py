# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileMu creation from TileCell
# with TileLookForMuAlg algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileMuGetter ( Configured )  :
    _outputType = "TileMuContainer"
    _output = { _outputType : "TileMuObj" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileMuGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileMuId.TileMuGetter import TileMuGetter
            theTileMuGetter=TileMuGetter()
        except:
            mlog.error("could not get handle to TileMuGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileMuGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileMuGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileMuGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileMuId.TileMuIdConf import TileLookForMuAlg                
        except:
            mlog.error("could not import TileMuId.TileLookForMuAlg")
            print traceback.format_exc()
            return False

        theTileLookForMuAlg=TileLookForMuAlg()
        self._TileLookForMuAlgHandle = theTileLookForMuAlg ;

        # Configure TileLookForMuAlg here
        # As in TileMuId_cosmics_jobOptions.py, check also
        # jobOptions_TileMuId.py and TileMuId_jobOptions.py
        #========== TILELOOKFORMUALG ==
        theTileLookForMuAlg.CellsNames="AllCalo"

        #=== thresholds in MeV ====================
        #  upper values are calculated considering muon with pt<=9GeV/c
        theTileLookForMuAlg.LowerTresh0MeV=80.0
        theTileLookForMuAlg.LowerTresh1MeV=80.0
        theTileLookForMuAlg.LowerTresh2MeV=80.0
        theTileLookForMuAlg.LowerTreshScinMeV=160.0
        #scintillators  in eta=+-1.1 implemented as +-A5
        # but eff/fakes still to be studied and settings to be optimized
        # tmp fix for cosmics (no rejection vs fake is needed )
        theTileLookForMuAlg.UpperTresh2MeV=[2370.0, 2100.0, 900.0, 1050.0,
                                            1050.0, 1110.0, 1590.0, 1110.0,
                                            1050.0, 1050.0, 900.0,  2100.0,
                                            2370.0]
        theTileLookForMuAlg.UpperTresh1MeV=[1680.0, 1500.0, 1440.0, 1380.0,
                                            1050.0, 390.0,  1110.0, 1860.0,
                                            1890.0, 1800.0, 1860.0, 1890.0,
                                            1770.0, 1980.0, 2550.0, 2550.0,
                                            1980.0, 1770.0, 1890.0, 1860.0,
                                            1800.0, 1890.0, 1860.0, 1110.0,
                                            390.0,  1050.0, 1380.0, 1440.0,
                                            1500.0, 1680.0]
        theTileLookForMuAlg.UpperTresh0MeV=[1680.0, 1380.0, 1230.0, 1140.0,
                                            210.0,   900.0,  960.0,  840.0,
                                            930.0,   840.0,  840.0,  750.0,
                                            870.0,   960.0, 1350.0, 1350.0,
                                            960.0,   870.0,  750.0,  840.0,
                                            840.0,   930.0,  840.0,  960.0,
                                            900.0,   210.0, 1140.0, 1230.0,
                                            1380.0, 1680.0]
        #======================== patterns for the  muon search============
        # 13*6 and 30*6 values:
        #  for instance:  starting from the 7th cell in the 3rd layer
        #  check the 16th  and 17th in the 2nd layer etc.
        theTileLookForMuAlg.From3to2=[5,0,1,2,3,4,    5,2,3,4,5,6,   2,6,7,0,0,0,
                                      3,7,8,9,0,0,    3,9,10,11,0,0, 2,12,13,0,0,0,
                                      2,14,15,0,0,0,  2,16,17,0,0,0, 3,18,19,20,0,0,
                                      3,20,21,22,0,0, 2,22,23,0,0,0, 5,23,24,25,26,27,
                                      5,25,26,27,28,29]
        theTileLookForMuAlg.From2to1=[2,1,2,0,0,0,    3,1,2,3,0,0,    3,2,3,4,0,0,
                                      3,3,4,5,0,0,    2,4,5,0,0,0,    1,5,0,0,0,0,
                                      1,6,0,0,0,0,    1,7,0,0,0,0,    1,8,0,0,0,0,
                                      1,9,0,0,0,0,    1,10,0,0,0,0,   1,11,0,0,0,0,
                                      1,12,0,0,0,0,   1,13,0,0,0,0,   1,14,0,0,0,0,
                                      1,15,0,0,0,0,   1,16,0,0,0,0,   1,17,0,0,0,0,
                                      1,18,0,0,0,0,   1,19,0,0,0,0,   1,20,0,0,0,0,
                                      1,21,0,0,0,0,   1,22,0,0,0,0,   1,23,0,0,0,0,
                                      1,24,0,0,0,0,   2,24,25,0,0,0,  3,24,25,26,0,0,
                                      3,25,26,27,0,0, 3,26,27,28,0,0, 2,27,28,0,0,0]


        # sets output key  
        theTileLookForMuAlg.TileMuTagsOutputName=self.outputKey() 

        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileLookForMuAlg;
        
        return True

    def TileLookForMuAlgHandle(self):
        return self._TileLookForMuAlgHandle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



