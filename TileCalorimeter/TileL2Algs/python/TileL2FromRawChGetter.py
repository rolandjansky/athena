# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileL2 creation from TileRawChannel
# with TileRawChannelToL2 algorithm

__author__  = 'J. Poveda'

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileL2FromRawChGetter ( Configured )  :

    """This getter module creates an instance of the TileRawChannelToL2 algorithm to obtain TileL2 objects from the TileRawChannels stored in a TileRawChannelContainer"""
    
    _outputType = "TileL2Container"
    _output = { _outputType : "TileL2Cnt" }


    def configure(self):
        mlog = logging.getLogger( 'TileL2FromRawChGetter::configure:' )
        mlog.info ('entering')        

        # get handle to upstream object
        try:
            from TileL2Algs.TileL2FromRawChGetter import TileL2FromRawChGetter
            theTileL2FromRawChGetter=TileL2FromRawChGetter()
        except:
            mlog.error("could not get handle to TileL2FromRawChGetter Quit")
            print traceback.format_exc()
            return False

        if not theTileL2FromRawChGetter.usable():
            if not self.ignoreConfigError():
                mlog.error("TileL2FromRawChGetter unusable. Quit.")
                return False
            else:
                mlog.error("TileL2FromRawChGetter unusable. Continue nevertheless")
                
        # Instantiation of the C++ algorithm
        try:        
            from TileL2Algs.TileL2AlgsConf import TileRawChannelToL2                
        except:
            mlog.error("could not import TileL2Algs.TileRawChannelToL2")
            print traceback.format_exc()
            return False

        theTileRawChannelToL2=TileRawChannelToL2()
        self._TileRawChannelToL2Handle = theTileRawChannelToL2 ;

        # sets output key  
        theTileRawChannelToL2.TileL2Container=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileRawChannelToL2;
        
        return True

    def TileRawChannelToL2Handle(self):
        return self._TileRawChannelToL2Handle
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



