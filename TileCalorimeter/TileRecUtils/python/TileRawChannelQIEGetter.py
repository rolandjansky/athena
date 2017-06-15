# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileRawChannel creation from TileDigits 
# TileRawChannelMaker algorithm using

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import AthenaCommon.CfgMgr as CfgMgr
from RecExConfig.Configured import Configured
import traceback

from RecExConfig.Configured import Configured
#from TileRecUtils.TileRawChannelMakerBase import TileRawChannelMakerBase 

class TileRawChannelQIEGetter ( Configured):
    """ This getter module creates an instance of the TileRawChannelMaker
    algorithm to obtain TileRawChannel objects from the TileDigits stored
    in a TileDigitsContainer.
    According to the values of the TileRecFlags jobProperties, the
    corresponding AlgTools are added tothe ToolSvc and associated to
    TileRawChannelMaker algorithm.
    """
    
    _outputType = "TileRawChannelContainer"
    # _output = { _outputType : "TileRawChannelQIE" }

    def configure(self):

        mlog = logging.getLogger( 'TileRawChannelQIEGetter::configure:' )
        mlog.info ("entering")        
        
        # Instantiation of the C++ algorithm
        try:        
            from TileRecUtils.TileRecUtilsConf import TileRawChannelMaker               
            theTileRawChannelMakerQIE = TileRawChannelMaker("TileRChMakerQIE")
            theTileRawChannelMakerQIE.FitOverflow = False
        except:
            mlog.error("could not import TileRecUtils.TileRawChannelMaker")
            print traceback.format_exc()
            return False
    
        self._TileRChMakerQIE = theTileRawChannelMakerQIE;

        # Configure TileInfoLoader
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr, "TileInfoLoader" ):
            from TileConditions.TileInfoConfigurator import TileInfoConfigurator
            tileInfoConfigurator = TileInfoConfigurator()

        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore
        
        from AthenaCommon.AppMgr import ToolSvc

        from TileRecUtils.TileRecFlags import jobproperties

        if (not hasattr(ToolSvc, "TileBeamInfoProvider")):

            from TileRecUtils.TileRecUtilsConf import TileBeamInfoProvider
            theTileBeamInfoProvider = TileBeamInfoProvider()
            if hasattr( ServiceMgr, "TileDCSSvc" ):
                theTileBeamInfoProvider.CheckDCS = True

            # true for real data, false for MC - GlobalFlags.DataSource.is_data()
            # true for nominal ATLAS configuration - GlobalFlags.DetGeo.is_atlas()
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.DataSource() == 'data':
                mlog.warning('Reconstruction of real data with QIE seems to be not implemented yet!')
            else:
                theTileBeamInfoProvider.TileBeamElemContainer = "";
                theTileBeamInfoProvider.TileDigitsContainer = "";
                theTileBeamInfoProvider.TileRawChannelContainer = "";
                
            ToolSvc += theTileBeamInfoProvider
        
        jobproperties.TileRecFlags.print_JobProperties('tree&value')
        
        # run optimal filter only if readDigits is set
        if jobproperties.TileRecFlags.readDigits():

            if jobproperties.TileRecFlags.doTileQIE():
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderQIEFilter
                    theTileRawChannelBuilderQIEFilter = TileRawChannelBuilderQIEFilter()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderQIEFilter Quit")
                    print traceback.format_exc()
                    return False
                
                #TileRawChannelBuilderQIEFilter Options:
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelQIE"
                theTileRawChannelBuilderQIEFilter.TileRawChannelContainer = "TileRawChannelQIE"
                theTileRawChannelBuilderQIEFilter.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderQIEFilter.calibrateEnergy  = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderQIEFilter.correctTime      = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderQIEFilter.PedestalMode = 1
      
                mlog.info(" adding now TileRawChannelBuilderQIEFilter to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderQIEFilter
      
                theTileRawChannelMakerQIE.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderQIEFilter]
            

            # now add algorithm to topSequence
            # this should always come at the end

            mlog.info(" now adding to topSequence")        
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()

            jobproperties.TileRecFlags.print_JobProperties('tree&value')

            theTileRawChannelMakerQIE.TileDigitsContainer = 'TileDigitsQIE'
            topSequence += theTileRawChannelMakerQIE;

        else:
            mlog.info(" Disable QIE methods because readDigits flag set to False ")
            jobproperties.TileRecFlags.doTileQIE = False
            jobproperties.TileRecFlags.print_JobProperties('tree&value')

        return True

    def TileRChMakerQIE(self):
        return self._TileRChMakerQIE
   
##    # would work only if one output object type
##    def outputKey(self):
##        return self._output[self._outputType]

##    def outputType(self):
##        return self._outputType


