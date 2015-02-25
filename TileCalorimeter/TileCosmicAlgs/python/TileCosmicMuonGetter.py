# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# Revised: J. Maneira (Feb08)
# TileEvent/TileCosmicMuon object creation from TileRawChannel
# from TileMuonFitter algorithm

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured
import traceback

class TileCosmicMuonGetter ( Configured )  :
    """ This getter module creates an instance of the TileMuonFitter
    algorithm to obtain TileCosmicMuon objects from the TileRawChannels stored
    in a TileRawChannelContainer"""
    
    _outputType = "TileCosmicMuonContainer"
    _output = { _outputType : "TileCosmicMuonHT" }

        
    def configure(self):
        mlog = logging.getLogger( 'TileCosmicMuon::configure:' )
        mlog.info ('entering')        
 
        # Instantiation of the C++ algorithm
        try:        
            from TileCosmicAlgs.TileCosmicAlgsConf import TileMuonFitter                
        except:
            mlog.error("could not import TileCosmicAlgs.TileMuonFitter")
            print traceback.format_exc()
            return False

        theTileMuonFitter=TileMuonFitter()
        self._TileMuonFitter = theTileMuonFitter;

        # sets output key  
        theTileMuonFitter.TileCosmicMuonKey=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding to topSequence")        
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        topSequence += theTileMuonFitter;
        
        return True

    def TileMuonFitter(self):
        return self._TileMuonFitter
   
    # would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



