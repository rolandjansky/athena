# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author: J. Poveda (Ximo.Poveda@cern.ch)
# TileRawChannel creation from TileDigits 
# TileRawChannelMaker algorithm using

from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.Logging import logging
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.Configured import Configured
import traceback

from RecExConfig.Configured import Configured
#from TileRecUtils.TileRawChannelMakerBase import TileRawChannelMakerBase 

class TileRawChannelGetter ( Configured)  :
    """ This getter module creates an instance of the TileRawChannelMaker
    algorithm to obtain TileRawChannel objects from the TileDigits stored
    in a TileDigitsContainer.
    According to the values of the TileRecFlags jobProperties, the
    corresponding AlgTools are added tothe ToolSvc and associated to
    TileRawChannelMaker algorithm.
    """
    
    _outputType = "TileRawChannelContainer"
    # _output = { _outputType : "TileRawChannelFit" }

    def configure(self):

        mlog = logging.getLogger( 'TileRawChannelGetter::configure:' )
        mlog.info ("entering")        
        
        # Instantiation of the C++ algorithm
        try:        
            from TileRecUtils.TileRecUtilsConf import TileRawChannelMaker               
            theTileRawChannelMaker = TileRawChannelMaker("TileRChMaker")
        except:
            mlog.error("could not import TileRecUtils.TileRawChannelMaker")
            print traceback.format_exc()
            return False
    
        self._TileRChMaker = theTileRawChannelMaker;

        # Configure TileInfoLoader
        from AthenaCommon.AppMgr import ServiceMgr
        if not hasattr( ServiceMgr, "TileInfoLoader" ):
            from TileConditions.TileInfoConfigurator import TileInfoConfigurator
            tileInfoConfigurator = TileInfoConfigurator()

        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore
        
        from AthenaCommon.AppMgr import ToolSvc

        from TileRecUtils.TileRecFlags import jobproperties
        from TileRecUtils.TileRecUtilsConf import TileBeamInfoProvider
        theTileBeamInfoProvider = TileBeamInfoProvider()
        if hasattr( ServiceMgr, "TileDCSSvc" ):
            theTileBeamInfoProvider.CheckDCS = True

        # true for real data, false for MC - GlobalFlags.DataSource.is_data()
        # true for nominal ATLAS configuration - GlobalFlags.DetGeo.is_atlas()
        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource()=='data':
            # apply noise filter for real data (if this option was not set before)
            if jobproperties.TileRecFlags.noiseFilter() < 0:
                jobproperties.TileRecFlags.noiseFilter=1

            if jobproperties.TileRecFlags.TileRunType() == 1 :
                theTileBeamInfoProvider.TileBeamElemContainer="";
            else:
                theTileBeamInfoProvider.TileBeamElemContainer="TileBeamElemCnt";
            if jobproperties.TileRecFlags.readDigits():
                theTileBeamInfoProvider.TileDigitsContainer="TileDigitsCnt";
            else:
                theTileBeamInfoProvider.TileDigitsContainer="";
            theTileBeamInfoProvider.TileRawChannelContainer="TileRawChannelCnt";
        else:
            theTileBeamInfoProvider.TileBeamElemContainer="";
            theTileBeamInfoProvider.TileDigitsContainer="";
            theTileBeamInfoProvider.TileRawChannelContainer="";

        # set time window for amplitude correction if it was not set correctly before
        if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() <= jobproperties.TileRecFlags.TimeMinForAmpCorrection() :
            from AthenaCommon.BeamFlags import jobproperties
            mlog.info("adjusting min/max time of parabolic correction for %s" % jobproperties.Beam.bunchSpacing)
            halfBS = jobproperties.Beam.bunchSpacing.get_Value()/2.
            if halfBS > 25.1:
                mlog.info("Bunch spacing is too big, keeping default limits for parabolic correction")
            else:
                jobproperties.TileRecFlags.TimeMinForAmpCorrection = -halfBS
                jobproperties.TileRecFlags.TimeMaxForAmpCorrection = halfBS

        ToolSvc += theTileBeamInfoProvider
        
        NoiseFilterTools = []
        if jobproperties.TileRecFlags.noiseFilter() == 1:

            if globalflags.DataSource() == 'data' and not athenaCommonFlags.isOnline():
                from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                tileInfoConfigurator = TileInfoConfigurator()
                # check if there OFC in DB for OF1 method
                if tileInfoConfigurator.setupCOOLOFC(ofcType = 'OF1'):
                    tileInfoConfigurator.setupCOOLTIME(online = True)

                    from TileRecUtils.TileRecUtilsConf import TileRawChannelOF1Corrector
                    theTileRawChannelOF1Corrector = TileRawChannelOF1Corrector()
                    ToolSvc += theTileRawChannelOF1Corrector
                    NoiseFilterTools += [theTileRawChannelOF1Corrector]

            from TileRecUtils.TileRecUtilsConf import TileRawChannelNoiseFilter
            theTileRawChannelNoiseFilter = TileRawChannelNoiseFilter()
            ToolSvc += theTileRawChannelNoiseFilter
            NoiseFilterTools += [theTileRawChannelNoiseFilter]



        TileFrameLength = ServiceMgr.TileInfoLoader.NSamples
        if TileFrameLength!=7:
            mlog.info("disabling reading of OFC from COOL because Nsamples!=7")
            jobproperties.TileRecFlags.OfcFromCOOL = False

        jobproperties.TileRecFlags.print_JobProperties('tree&value')
        
        # run optimal filter only if readDigits is set
        if jobproperties.TileRecFlags.readDigits():

            if jobproperties.TileRecFlags.doTileQIE():
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderQIEFilter
                    theTileRawChannelBuilderQIEFilter= TileRawChannelBuilderQIEFilter()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderQIEFilter Quit")
                    print traceback.format_exc()
                    return False
                
                ## setup COOL to get OFCs
                #if jobproperties.TileRecFlags.OfcFromCOOL():
                #    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                #    tileInfoConfigurator = TileInfoConfigurator()
                #    tileInfoConfigurator.setupCOOLOFC()
                #else:
                #    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                #    tileInfoConfigurator = TileInfoConfigurator()
                #    tileInfoConfigurator.setupCOOLPHYPULSE()
                #    tileInfoConfigurator.setupCOOLAutoCr()
                    
                #TileRawChannelBuilderQIEFilter Options:
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelQIE"
                theTileRawChannelBuilderQIEFilter.TileRawChannelContainer = "TileRawChannelQIE"
                theTileRawChannelBuilderQIEFilter.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderQIEFilter.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderQIEFilter.correctTime     = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderQIEFilter.NoiseFilterTools= NoiseFilterTools
                ##theTileRawChannelBuilderQIEFilter.BestPhase       = False; # no point to use best phase with interations
                theTileRawChannelBuilderQIEFilter.PedestalMode = 1
                ##theTileRawChannelBuilderQIEFilter.MaxIterations = 5
                ##theTileRawChannelBuilderQIEFilter.Minus1Iteration = True
                ##theTileRawChannelBuilderQIEFilter.AmplitudeCorrection = False; # don't need correction after iterations
                ##theTileRawChannelBuilderQIEFilter.OfcfromCool = jobproperties.TileRecFlags.OfcFromCOOL()            
                #from TileConditions.TileCondToolConf import getTileCondToolPulseShape
                #ToolSvc.TileCondToolOfc.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','PHY')
      
                mlog.info(" adding now TileRawChannelBuilderQIEFilter to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderQIEFilter
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderQIEFilter]
            
            # fit with several amplitudes 
            if jobproperties.TileRecFlags.doTileManyAmps():
      
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderManyAmps
                    theTileRawChannelBuilderManyAmps= TileRawChannelBuilderManyAmps()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderManyAmps Quit")
                    print traceback.format_exc()
                    return False
      
                #TileRawChannelBuilderManyAmps Options:
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelManyAmp"
                theTileRawChannelBuilderManyAmps.TileRawChannelContainer = "TileRawChannelManyAmp"
                theTileRawChannelBuilderManyAmps.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderManyAmps.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderManyAmps.correctTime     = jobproperties.TileRecFlags.correctTime()    
                theTileRawChannelBuilderManyAmps.NoiseFilterTools= NoiseFilterTools
                 
                mlog.info(" adding now TileRawChannelBuilderManyAmps to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderManyAmps
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderManyAmps]
      
            # flat filter - sum of 5 samples
            if jobproperties.TileRecFlags.doTileFlat():
      
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderFlatFilter
                    theTileRawChannelBuilderFlatFilter= TileRawChannelBuilderFlatFilter()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderFlatFilter Quit")
                    print traceback.format_exc()
                    return False
      
                #TileRawChannelBuilderFlatFilter Options: 
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFlat"
                theTileRawChannelBuilderFlatFilter.TileRawChannelContainer = "TileRawChannelFlat"
                theTileRawChannelBuilderFlatFilter.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderFlatFilter.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderFlatFilter.correctTime     = jobproperties.TileRecFlags.correctTime()    
                theTileRawChannelBuilderFlatFilter.NoiseFilterTools= NoiseFilterTools
                theTileRawChannelBuilderFlatFilter.FrameLength = TileFrameLength
                theTileRawChannelBuilderFlatFilter.SignalLength = TileFrameLength - 1
      
                mlog.info(" adding now TileRawChannelBuilderFlatFilter to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderFlatFilter
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderFlatFilter]
      
            # Fit method
            if jobproperties.TileRecFlags.doTileFit() or jobproperties.TileRecFlags.doTileOverflowFit():
          
                # configure TileRawChannelMaker here
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderFitFilter
                    theTileRawChannelBuilderFitFilter= TileRawChannelBuilderFitFilter()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderFitFilter Quit")
                    print traceback.format_exc()
                    return False
                
                #TileRawChannelBuilderFitFilter Options: 
                theTileRawChannelBuilderFitFilter.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderFitFilter.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderFitFilter.correctTime     = jobproperties.TileRecFlags.correctTime()    
                theTileRawChannelBuilderFitFilter.NoiseFilterTools= NoiseFilterTools
                theTileRawChannelBuilderFitFilter.FrameLength = TileFrameLength
                
                # add the tool to list of tool ( should use ToolHandle eventually)
                mlog.info(" adding now TileRawChannelBuilderFitFilter to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderFitFilter

                if jobproperties.TileRecFlags.doTileFit():
                    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFit"
                    theTileRawChannelBuilderFitFilter.TileRawChannelContainer = "TileRawChannelFit"
                    theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderFitFilter]    
                    
                if jobproperties.TileRecFlags.doTileOverflowFit(): 
                    theTileRawChannelMaker.FitOverflow = True
                    theTileRawChannelMaker.TileRawChannelBuilderFitOverflow = ToolSvc.TileRawChannelBuilderFitFilter
                    mlog.info(" set up TileRawChannelBuilderFitOverflow to TileRawChannelBuilderFitFilter") 
                
            # Fit method with reading from COOL
            if jobproperties.TileRecFlags.doTileFitCool():
          
                # configure TileRawChannelMaker here
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderFitFilterCool
                    theTileRawChannelBuilderFitFilterCool= TileRawChannelBuilderFitFilterCool()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderFitFilterCool Quit")
                    print traceback.format_exc()
                    return False
                
                #TileRawChannelBuilderFitFilterCool Options: 
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFitCool"
                theTileRawChannelBuilderFitFilterCool.TileRawChannelContainer = "TileRawChannelFitCool"
                theTileRawChannelBuilderFitFilterCool.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderFitFilterCool.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderFitFilterCool.correctTime     = jobproperties.TileRecFlags.correctTime()    
                theTileRawChannelBuilderFitFilterCool.NoiseFilterTools= NoiseFilterTools
                theTileRawChannelBuilderFitFilterCool.FrameLength = TileFrameLength
                
                # add the tool to list of tool ( should use ToolHandle eventually)
                mlog.info(" adding now TileRawChannelBuilderFitFilterCool to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderFitFilterCool
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderFitFilterCool]
                
            # matched filter 
            if jobproperties.TileRecFlags.doTileMF():
      
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderMF
                    theTileRawChannelBuilderMF= TileRawChannelBuilderMF()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderMF Quit")
                    print traceback.format_exc()
                    return False
                                    
                # setup COOL to get OFCs, needed for COF to retrieve pulse shape and derivatives
                if jobproperties.TileRecFlags.OfcFromCOOL():
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLOFC()
                else:
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLPHYPULSE()
                    tileInfoConfigurator.setupCOOLAutoCr()

                #TileRawChannelBuilderMF Options:
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelMF"
                theTileRawChannelBuilderMF.TileRawChannelContainer = "TileRawChannelMF"
                theTileRawChannelBuilderMF.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderMF.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                if jobproperties.TileRecFlags.BestPhaseFromCOOL(): # can't correct time and use best phase at the same time
                    theTileRawChannelBuilderMF.correctTime = False
                else:
                    theTileRawChannelBuilderMF.correctTime = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderMF.BestPhase       = jobproperties.TileRecFlags.BestPhaseFromCOOL()
                theTileRawChannelBuilderMF.NoiseFilterTools = NoiseFilterTools
                theTileRawChannelBuilderMF.MaxIterations = 5; # iterative mode on
                theTileRawChannelBuilderMF.AmplitudeCorrection = False
                theTileRawChannelBuilderMF.TimeFromCOF = False
                theTileRawChannelBuilderMF.OfcfromCool = jobproperties.TileRecFlags.OfcFromCOOL()            
                theTileRawChannelBuilderMF.AmpMinForAmpCorrection = jobproperties.TileRecFlags.AmpMinForAmpCorrection()
                if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() > jobproperties.TileRecFlags.TimeMinForAmpCorrection():
                    theTileRawChannelBuilderMF.TimeMinForAmpCorrection = jobproperties.TileRecFlags.TimeMinForAmpCorrection()
                    theTileRawChannelBuilderMF.TimeMaxForAmpCorrection = jobproperties.TileRecFlags.TimeMaxForAmpCorrection()

                mlog.info(" adding now TileRawChannelBuilderMF to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderMF
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderMF]

            if jobproperties.TileRecFlags.doTileOpt():
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOptFilter
                    theTileRawChannelBuilderOptFilter= TileRawChannelBuilderOptFilter()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderOptFilter Quit")
                    print traceback.format_exc()
                    return False
                
                #TileRawChannelBuilderOptFilter Options:
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelOpt"
                theTileRawChannelBuilderOptFilter.TileRawChannelContainer = "TileRawChannelOpt"
                theTileRawChannelBuilderOptFilter.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderOptFilter.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderOptFilter.correctTime     = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderOptFilter.OF2 = True
                theTileRawChannelBuilderOptFilter.PedestalMode = 1
                theTileRawChannelBuilderOptFilter.MaxIterations = 5
                theTileRawChannelBuilderOptFilter.Minus1Iteration = True
                theTileRawChannelBuilderOptFilter.AmplitudeCorrection = False; # don't need correction after iterations
                theTileRawChannelBuilderOptFilter.TimeCorrection = False # don't need correction after iterations
                
                ServiceMgr.TileInfoLoader.LoadOptFilterWeights=True
                
                mlog.info(" adding now TileRawChannelBuilderOptFilter to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderOptFilter
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderOptFilter]
      
            if jobproperties.TileRecFlags.doTileOF1():
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOpt2Filter
                    theTileRawChannelBuilderOF1 = TileRawChannelBuilderOpt2Filter("TileRawChannelBuilderOF1")
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderOF1 Quit")
                    print traceback.format_exc()
                    return False
                
                # setup COOL to get OFCs
                if jobproperties.TileRecFlags.OfcFromCOOL():
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLOFC()
                else:
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLPHYPULSE()
                    tileInfoConfigurator.setupCOOLAutoCr()
                    
                #TileRawChannelBuilderOF1 Options:
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelOF1"
                theTileRawChannelBuilderOF1.TileRawChannelContainer = "TileRawChannelOF1"
                theTileRawChannelBuilderOF1.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderOF1.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                if jobproperties.TileRecFlags.BestPhaseFromCOOL(): # can't correct time and use best phase at the same time
                    theTileRawChannelBuilderOF1.correctTime = False
                else:
                    theTileRawChannelBuilderOF1.correctTime = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderOF1.BestPhase       = jobproperties.TileRecFlags.BestPhaseFromCOOL()
                theTileRawChannelBuilderOF1.NoiseFilterTools= NoiseFilterTools
                theTileRawChannelBuilderOF1.OF2 = False
                theTileRawChannelBuilderOF1.PedestalMode = -1
                theTileRawChannelBuilderOF1.MaxIterations = 1; # just one iteration
                theTileRawChannelBuilderOF1.Minus1Iteration = False; # assume that max sample is at t=0
                theTileRawChannelBuilderOF1.AmplitudeCorrection = jobproperties.TileRecFlags.correctAmplitude()
                theTileRawChannelBuilderOF1.TimeCorrection = False
                theTileRawChannelBuilderOF1.OfcfromCool = jobproperties.TileRecFlags.OfcFromCOOL()            
                theTileRawChannelBuilderOF1.AmpMinForAmpCorrection = jobproperties.TileRecFlags.AmpMinForAmpCorrection()
                if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() > jobproperties.TileRecFlags.TimeMinForAmpCorrection():
                    theTileRawChannelBuilderOF1.TimeMinForAmpCorrection = jobproperties.TileRecFlags.TimeMinForAmpCorrection()
                    theTileRawChannelBuilderOF1.TimeMaxForAmpCorrection = jobproperties.TileRecFlags.TimeMaxForAmpCorrection()
      
                mlog.info(" adding now TileRawChannelBuilderOF1 to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderOF1
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderOF1]

            if jobproperties.TileRecFlags.doTileOpt2():
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOpt2Filter
                    theTileRawChannelBuilderOpt2Filter= TileRawChannelBuilderOpt2Filter()
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderOpt2Filter Quit")
                    print traceback.format_exc()
                    return False
                
                # setup COOL to get OFCs
                if jobproperties.TileRecFlags.OfcFromCOOL():
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLOFC()
                else:
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLPHYPULSE()
                    tileInfoConfigurator.setupCOOLAutoCr()
                    
                #TileRawChannelBuilderOpt2Filter Options:
                jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelOpt2"
                theTileRawChannelBuilderOpt2Filter.TileRawChannelContainer = "TileRawChannelOpt2"
                theTileRawChannelBuilderOpt2Filter.RunType = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderOpt2Filter.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                theTileRawChannelBuilderOpt2Filter.correctTime     = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderOpt2Filter.NoiseFilterTools= NoiseFilterTools
                theTileRawChannelBuilderOpt2Filter.BestPhase       = False; # no point to use best phase with interations
                theTileRawChannelBuilderOpt2Filter.OF2 = True
                theTileRawChannelBuilderOpt2Filter.PedestalMode = 1
                theTileRawChannelBuilderOpt2Filter.MaxIterations = 5
                theTileRawChannelBuilderOpt2Filter.Minus1Iteration = True
                theTileRawChannelBuilderOpt2Filter.AmplitudeCorrection = False; # don't need correction after iterations
                theTileRawChannelBuilderOpt2Filter.TimeCorrection    = False; # don't need correction after iterations
                theTileRawChannelBuilderOpt2Filter.OfcfromCool = jobproperties.TileRecFlags.OfcFromCOOL()            
                #from TileConditions.TileCondToolConf import getTileCondToolPulseShape
                #ToolSvc.TileCondToolOfc.TileCondToolPulseShape = getTileCondToolPulseShape('COOL','PHY')
      
                mlog.info(" adding now TileRawChannelBuilderOpt2Filter to ToolSvc")   
                ToolSvc += theTileRawChannelBuilderOpt2Filter
      
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderOpt2Filter]

      
      
            if jobproperties.TileRecFlags.doTileOptATLAS():
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawChannelBuilderOpt2Filter
                    theTileRawChannelBuilderOptATLAS= TileRawChannelBuilderOpt2Filter("TileRawChannelBuilderOptATLAS")
                except:
                    mlog.error("could not get handle to TileRawChannelBuilderOpt2Filter Quit")
                    print traceback.format_exc()
                    return False
                
                # setup COOL to get OFCs
                if jobproperties.TileRecFlags.OfcFromCOOL():
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLOFC()
                else:
                    from TileConditions.TileInfoConfigurator import TileInfoConfigurator
                    tileInfoConfigurator = TileInfoConfigurator()
                    tileInfoConfigurator.setupCOOLPHYPULSE()
                    tileInfoConfigurator.setupCOOLAutoCr()
                    
                #TileRawChannelBuilderOptATLAS Options:
                if globalflags.DataSource()=='data': # don't use the name which is used for reco data from DSP
                    if jobproperties.TileRecFlags.TileRawChannelContainer == "TileRawChannelCnt":
                        jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelFixed"
                    theTileRawChannelBuilderOptATLAS.TileRawChannelContainer = "TileRawChannelFixed"
                else:
                    jobproperties.TileRecFlags.TileRawChannelContainer = "TileRawChannelCnt"
                    theTileRawChannelBuilderOptATLAS.TileRawChannelContainer = "TileRawChannelCnt"
                theTileRawChannelBuilderOptATLAS.RunType         = jobproperties.TileRecFlags.TileRunType()
                theTileRawChannelBuilderOptATLAS.calibrateEnergy = jobproperties.TileRecFlags.calibrateEnergy()
                if jobproperties.TileRecFlags.BestPhaseFromCOOL(): # can't correct time and use best phase at the same time
                    theTileRawChannelBuilderOptATLAS.correctTime = False
                else:
                    theTileRawChannelBuilderOptATLAS.correctTime = jobproperties.TileRecFlags.correctTime()
                theTileRawChannelBuilderOptATLAS.BestPhase       = jobproperties.TileRecFlags.BestPhaseFromCOOL()
                theTileRawChannelBuilderOptATLAS.NoiseFilterTools= NoiseFilterTools
                theTileRawChannelBuilderOptATLAS.OF2 = True
                #theTileRawChannelBuilderOptATLAS.PedestalMode = 1; # not sure if we need this option here
                theTileRawChannelBuilderOptATLAS.MaxIterations = 1; # just one iteration
                theTileRawChannelBuilderOptATLAS.Minus1Iteration = False; # assume that max sample is at t=0
                theTileRawChannelBuilderOptATLAS.AmplitudeCorrection = jobproperties.TileRecFlags.correctAmplitude()
                theTileRawChannelBuilderOptATLAS.TimeCorrection = jobproperties.TileRecFlags.correctTimeNI()
                theTileRawChannelBuilderOptATLAS.OfcfromCool = jobproperties.TileRecFlags.OfcFromCOOL()            
                theTileRawChannelBuilderOptATLAS.AmpMinForAmpCorrection = jobproperties.TileRecFlags.AmpMinForAmpCorrection()
                if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() > jobproperties.TileRecFlags.TimeMinForAmpCorrection():
                    theTileRawChannelBuilderOptATLAS.TimeMinForAmpCorrection = jobproperties.TileRecFlags.TimeMinForAmpCorrection()
                    theTileRawChannelBuilderOptATLAS.TimeMaxForAmpCorrection = jobproperties.TileRecFlags.TimeMaxForAmpCorrection()
                
                mlog.info(" adding now TileRawChannelBuilderOpt2Filter with name TileRawChannelBuilderOptATLAS to ToolSvc")
                ToolSvc += theTileRawChannelBuilderOptATLAS
                
                theTileRawChannelMaker.TileRawChannelBuilder += [ToolSvc.TileRawChannelBuilderOptATLAS]
            

            jobproperties.TileRecFlags.print_JobProperties('tree&value')

            # now add algorithm to topSequence
            # this should always come at the end

            mlog.info(" now adding to topSequence")        
            from AthenaCommon.AlgSequence import AlgSequence
            topSequence = AlgSequence()

            if jobproperties.TileRecFlags.noiseFilter() == 2:
                # Instantiation of the C++ algorithm
                try:
                    from TileRecUtils.TileRecUtilsConf import TileRawCorrelatedNoise
                    theTileRawCorrelatedNoise=TileRawCorrelatedNoise("TileRCorreNoise")
                except:
                    mlog.error("could not import TileRecUtils.TileRawCorrelatedNoise")
                    print traceback.format_exc()
                    return False
                #theTileRawCorrelatedNoise.UseMeanFiles = False
                #theTileRawCorrelatedNoise.PMTOrder = True
                theTileRawChannelMaker.TileDigitsContainer = "NewDigitsContainer"
                topSequence += theTileRawCorrelatedNoise;

            topSequence += theTileRawChannelMaker;

        else:
            mlog.info(" Disable all OF methods because readDigits flag set to False ")
            jobproperties.TileRecFlags.doTileFlat = False
            jobproperties.TileRecFlags.doTileFit = False
            jobproperties.TileRecFlags.doTileFitCool = False
            jobproperties.TileRecFlags.doTileOpt = False
            jobproperties.TileRecFlags.doTileOpt2 = False
            jobproperties.TileRecFlags.doTileOptATLAS = False
            jobproperties.TileRecFlags.doTileManyAmps = False
            jobproperties.TileRecFlags.doTileMF = False
            jobproperties.TileRecFlags.doTileOF1 = False
            jobproperties.TileRecFlags.OfcFromCOOL = False
            jobproperties.TileRecFlags.print_JobProperties('tree&value')

        return True

    def TileRChMaker(self):
        return self._TileRChMaker
   
##    # would work only if one output object type
##    def outputKey(self):
##        return self._output[self._outputType]

##    def outputType(self):
##        return self._outputType


