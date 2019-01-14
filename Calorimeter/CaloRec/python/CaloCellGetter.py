# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# specifies Calo cell making
# so far only handle the RawChannel->CaloCell step
# not all possibility of CaloCellMaker_jobOptions.py integrated yet
from AthenaCommon.Constants import *
from RecExConfig.Configured import Configured
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
import traceback

class CaloCellGetter (Configured)  :
    _outputType = "CaloCellContainer"
    _output = { _outputType : "AllCalo" }

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger('CaloCellGetter::configure:')
        mlog.info ('entering')        

        doStandardCellReconstruction = True
        from CaloRec.CaloCellFlags import jobproperties
        
        if not jobproperties.CaloCellFlags.doFastCaloSim.statusOn:
            doFastCaloSim = False
            mlog.info("doFastCaloSim not set, so not using it")
        else:
            doFastCaloSim = jobproperties.CaloCellFlags.doFastCaloSim()
            if doFastCaloSim:
                mlog.info("doFastCaloSim requested")
                doStandardCellReconstruction = False
                if jobproperties.CaloCellFlags.doFastCaloSimAddCells():
                    doStandardCellReconstruction = True
                    mlog.info("doFastCaloSimAddCells requested: FastCaloSim is added to fullsim calorimeter")
                else:    
                    mlog.info("doFastCaloSimAddCells not requested: Stand alone FastCaloSim is running")
            else:    
                mlog.info("doFastCaloSim explicitly not requested")



        # get handle to upstream object
        # handle tile

        if doStandardCellReconstruction:
            from LArROD.LArRODFlags import larRODFlags
            from AthenaCommon.GlobalFlags import globalflags
            if larRODFlags.readDigits() and globalflags.DataSource() == 'data':
                if not "LArRawChannelContainer/LArRawChannels" in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
                    svcMgr.ByteStreamAddressProviderSvc.TypeNames += ["LArRawChannelContainer/LArRawChannels"]
                from LArROD.LArRawChannelBuilderDefault import LArRawChannelBuilderDefault
                LArRawChannelBuilderDefault()
          
        
        # writing of thinned digits
        if jobproperties.CaloCellFlags.doLArThinnedDigits.statusOn and jobproperties.CaloCellFlags.doLArThinnedDigits():
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.DataSource() == 'data':
                try:
                    from LArROD.LArDigits import DefaultLArDigitThinner
                    LArDigitThinner = DefaultLArDigitThinner('LArDigitThinner')  # automatically added to topSequence
                    LArDigitThinner.InputContainerName = "FREE"
                    LArDigitThinner.OutputContainerName = "LArDigitContainer_Thinned"
                except Exception:
                    treatException("Problem with LArDigitThinner ")

        # now configure the algorithm, part of this could be done in a separate class
        # cannot have same name
        try:        
            from CaloRec.CaloRecConf import CaloCellMaker                
        except:
            mlog.error("could not import CaloRec.CaloCellMaker")
            print traceback.format_exc()
            return False

        theCaloCellMaker = CaloCellMaker()
        self._CaloCellMakerHandle = theCaloCellMaker ;

        from AthenaCommon.AppMgr import ToolSvc

        if doStandardCellReconstruction:
            # configure CaloCellMaker here
            # check LArCellMakerTool_jobOptions.py for full configurability 
            # FIXME 

            from RecExConfig.RecFlags import rec

            if rec.doLArg():
                from LArCabling.LArCablingAccess import LArOnOffIdMapping
                LArOnOffIdMapping()

                try:
                    from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool
                    theLArCellBuilder = LArCellBuilderFromLArRawChannelTool()
                except:
                    mlog.error("could not get handle to LArCellBuilderFromLArRawChannel Quit")
                    print traceback.format_exc()
                    return False

                if jobproperties.CaloCellFlags.doLArCreateMissingCells():
                    theLArCellBuilder.addDeadOTX = True

                # add the tool to list of tool ( should use ToolHandle eventually) 
                theCaloCellMaker += theLArCellBuilder
                theCaloCellMaker.CaloCellMakerToolNames += [theLArCellBuilder]
            

            if rec.doTile():

                from AthenaCommon.GlobalFlags import globalflags
                if globalflags.DataSource() == 'data' and globalflags.InputFormat() == 'bytestream':
                    try:
                        svcMgr.ByteStreamCnvSvc.ROD2ROBmap = [ "-1" ]
                        if not "TileDigitsContainer/TileDigitsCnt" in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
                            svcMgr.ByteStreamAddressProviderSvc.TypeNames+=["TileBeamElemContainer/TileBeamElemCnt",
                                                                            "TileDigitsContainer/TileDigitsCnt",
                                                                            "TileL2Container/TileL2Cnt",
                                                                            "TileLaserObject/TileLaserObj",
                                                                            "TileMuonReceiverContainer/TileMuRcvCnt" ]
                    except:
                        mlog.warning("Cannot add TileDigitsContainer/TileDigitsCnt et al. to bytestream list")

                    # set options for TileRawChannelMaker
                    from TileRecUtils.TileRecFlags import jobproperties
                    jobproperties.TileRecFlags.TileRunType = 1;  # physics run type

                    # reading of digits can be disabled before calling CaloCellGetter
                    # if this is not done, but digits are not available in BS file
                    # reading of digits is automatically disabled at start of run
                    if jobproperties.TileRecFlags.readDigits()                \
                        and not (jobproperties.TileRecFlags.doTileFlat        \
                                 or jobproperties.TileRecFlags.doTileFit      \
                                 or jobproperties.TileRecFlags.doTileFitCool  \
                                 or jobproperties.TileRecFlags.doTileOpt      \
                                 or jobproperties.TileRecFlags.doTileOF1      \
                                 or jobproperties.TileRecFlags.doTileOpt2     \
                                 or jobproperties.TileRecFlags.doTileOptATLAS \
                                 or jobproperties.TileRecFlags.doTileMF):
                        
                        from AthenaCommon.BeamFlags import jobproperties
                        # run Opt filter with iterations by default, both for cosmics and collisions before 2011
                        # run Opt filter without iterations for collisions in 2011 and later 
                        if not 'doTileOpt2' in dir():
                            from RecExConfig.AutoConfiguration import GetRunNumber
                            rn = GetRunNumber()
                            if rn > 0 and rn < 171194:  
                                doTileOpt2 = True
                            elif jobproperties.Beam.beamType()=='collisions':
                                doTileOpt2 = False; # use OF without iterations for collisions
                            else:
                                doTileOpt2 = True; # always run OF with iterations for cosmics
                                    
                        # jobproperties.TileRecFlags.calibrateEnergy=True; # use pCb for RawChannels
                        # please, note that time correction and best phase are used only for collisions
                        if doTileOpt2:
                            jobproperties.TileRecFlags.doTileOpt2 = True;  # run optimal filter with iterations
                            jobproperties.TileRecFlags.doTileOptATLAS = False;  # disable optimal filter without iterations
                            jobproperties.TileRecFlags.correctAmplitude = False;  # don't do parabolic correction
                            if jobproperties.Beam.beamType() == 'collisions':
                                jobproperties.TileRecFlags.correctTime = True;  # apply time correction in physics runs
                                jobproperties.TileRecFlags.BestPhaseFromCOOL = False;  # best phase is not needed for iterations
                        else:
                            jobproperties.TileRecFlags.doTileOpt2 = False;  # disable optimal filter with iterations
                            jobproperties.TileRecFlags.doTileOptATLAS = True;  # run optimal filter without iterations
                            jobproperties.TileRecFlags.correctAmplitude = True;  # apply parabolic correction
                            if jobproperties.Beam.beamType() == 'collisions':
                                jobproperties.TileRecFlags.correctTime = False;  # don't need time correction if best phase is used
                                jobproperties.TileRecFlags.BestPhaseFromCOOL = True;  # use best phase stored in DB

                    try:
                        from TileRecUtils.TileRawChannelGetter import TileRawChannelGetter
                        theTileRawChannelGetter = TileRawChannelGetter()
                    except:
                        mlog.error("could not load TileRawChannelGetter Quit")
                        print traceback.format_exc()
                        return False

                    try:
                        from TileRecAlgs.TileRecAlgsConf import TileDigitsFilter
                        from AthenaCommon.AlgSequence import AlgSequence
                        topSequence = AlgSequence()
                        topSequence += TileDigitsFilter()
                    except:
                        mlog.error("Could not configure TileDigitsFilter")

                from AthenaCommon.AlgSequence import AthSequencer
                condSequence = AthSequencer("AthCondSeq")
                checkDCS = hasattr(condSequence, 'TileDCSCondAlg')
                try:
                    from TileRecUtils.TileRecUtilsConf import TileCellBuilder
                    theTileCellBuilder = TileCellBuilder(CheckDCS = checkDCS)
                    from TileRecUtils.TileRecFlags import jobproperties
                    theTileCellBuilder.TileRawChannelContainer = jobproperties.TileRecFlags.TileRawChannelContainer()

                    rawChannelContainer = ''
                    if globalflags.DataSource() == 'data' and globalflags.InputFormat() == 'bytestream':
                        if jobproperties.TileRecFlags.readDigits():
                            # everything is already corrected at RawChannel level
                            theTileCellBuilder.correctTime = False;
                            theTileCellBuilder.correctAmplitude = False;
                        else:
                            rawChannelContainer = 'TileRawChannelCnt'
                            # by default parameters are tuned for opt.filter without iterations
                            theTileCellBuilder.correctTime = jobproperties.TileRecFlags.correctTime()
                            theTileCellBuilder.correctAmplitude = jobproperties.TileRecFlags.correctAmplitude()
                            theTileCellBuilder.AmpMinForAmpCorrection = jobproperties.TileRecFlags.AmpMinForAmpCorrection()
                            if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() <= jobproperties.TileRecFlags.TimeMinForAmpCorrection() :
                                from AthenaCommon.BeamFlags import jobproperties
                                mlog.info("adjusting min/max time of parabolic correction for %s" % jobproperties.Beam.bunchSpacing)
                                halfBS = jobproperties.Beam.bunchSpacing.get_Value()/2.
                                jobproperties.TileRecFlags.TimeMinForAmpCorrection = -halfBS
                                jobproperties.TileRecFlags.TimeMaxForAmpCorrection = halfBS
                            if jobproperties.TileRecFlags.TimeMaxForAmpCorrection() > jobproperties.TileRecFlags.TimeMinForAmpCorrection():
                                theTileCellBuilder.TimeMinForAmpCorrection = jobproperties.TileRecFlags.TimeMinForAmpCorrection()
                                theTileCellBuilder.TimeMaxForAmpCorrection = jobproperties.TileRecFlags.TimeMaxForAmpCorrection()

                    theCaloCellMaker += theTileCellBuilder
                    theCaloCellMaker.CaloCellMakerToolNames += [theTileCellBuilder]
                except:    
                    mlog.error("could not get handle to TileCellBuilder Quit")
                    print traceback.format_exc()
                    return False
             

        if doFastCaloSim:
            mlog.info ('configuring FastCaloSim here')        
            
            try:
                from FastCaloSim.FastCaloSimConf import EmptyCellBuilderTool
                theEmptyCellBuilderTool=EmptyCellBuilderTool()
                theCaloCellMaker += theEmptyCellBuilderTool
                theCaloCellMaker.CaloCellMakerToolNames += [ theEmptyCellBuilderTool ]

                print theEmptyCellBuilderTool
                mlog.info("configure EmptyCellBuilderTool worked")
            except:
                mlog.error("could not get handle to EmptyCellBuilderTool Quit")
                print traceback.format_exc()
                return False
            

            try:
                from FastCaloSim.FastCaloSimFactory import FastCaloSimFactory
                theFastShowerCellBuilderTool=FastCaloSimFactory()

                theCaloCellMaker += theFastShowerCellBuilderTool
                theCaloCellMaker.CaloCellMakerToolNames += [ theFastShowerCellBuilderTool ]
                mlog.info("configure FastShowerCellBuilderTool worked")
            except:
                mlog.error("could not get handle to FastShowerCellBuilderTool Quit")
                print traceback.format_exc()
                return False


            doFastCaloSimNoise=jobproperties.CaloCellFlags.doFastCaloSimNoise()
            if doFastCaloSimNoise:
                try:
                    from FastCaloSim.FastCaloSimConf import AddNoiseCellBuilderTool
                    theAddNoiseCellBuilderTool=AddNoiseCellBuilderTool()
                    
                    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
                    theCaloNoiseTool = CaloNoiseToolDefault()
                    from AthenaCommon.AppMgr import ToolSvc
                    ToolSvc += theCaloNoiseTool
                    
                    theAddNoiseCellBuilderTool.CaloNoiseTool=theCaloNoiseTool.getFullName()
                    
                    print theAddNoiseCellBuilderTool

                    theCaloCellMaker += theAddNoiseCellBuilderTool
                    theCaloCellMaker.CaloCellMakerToolNames += [ theAddNoiseCellBuilderTool ]
                    mlog.info("configure AddNoiseCellBuilderTool worked")
                except:
                    mlog.error("could not get handle to AddNoiseCellBuilderTool Quit")
                    print traceback.format_exc()
                    return False




        #
        # CaloCellContainerFinalizerTool : closing container and setting up iterators
        #
    
        from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool     
        theCaloCellContainerFinalizerTool=CaloCellContainerFinalizerTool()
        theCaloCellMaker += theCaloCellContainerFinalizerTool
        theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool ]

        #
        # Mergeing of calo cellcontainer with sparse raw channel container with improved energies
        #

        doLArMerge = False
        if  globalflags.DataSource() == 'data'  and jobproperties.CaloCellFlags.doLArRawChannelMerge.statusOn and jobproperties.CaloCellFlags.doLArRawChannelMerge():
            from LArROD.LArRODFlags import larRODFlags
            if larRODFlags.readDigits() and larRODFlags.keepDSPRaw():
                doLArMerge = True
        if doLArMerge:
            try:
                from LArCellRec.LArCellRecConf import LArCellMerger
                theLArCellMerger = LArCellMerger()
            except:
                mlog.error("could not get handle to LArCellMerge Quit")
                print traceback.format_exc()
                return False
            theLArCellMerger.RawChannelsName = larRODFlags.RawChannelFromDigitsContainerName()
            theCaloCellMaker += theLArCellMerger
            theCaloCellMaker.CaloCellMakerToolNames += [theLArCellMerger]


        #
        # masking of noisy and sporadic noisy cells in LAr
        #

        doNoiseMask = False
        if jobproperties.CaloCellFlags.doLArNoiseMasking.statusOn and jobproperties.CaloCellFlags.doLArNoiseMasking():
            doNoiseMask = True
        doSporadicMask = False
        if jobproperties.CaloCellFlags.doLArSporadicMasking.statusOn and jobproperties.CaloCellFlags.doLArSporadicMasking():
            doSporadicMask = True
   
        if doNoiseMask or doSporadicMask :
            try:
                from LArCellRec.LArCellRecConf import LArCellNoiseMaskingTool
                theLArCellNoiseMaskingTool = LArCellNoiseMaskingTool()
            except:
                mlog.error("could not get handle to LArCellNoiseMaskingTool Quit")
                print traceback.format_exc()
                return False

            if doSporadicMask:
                try:
                    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
                    theLArSporadicNoiseMasker = LArBadChannelMasker("LArSporadicNoiseMasker")
                except:
                    mlog.error("could not access bad channel tool Quit")
                    print traceback.format_exc()
                    return False
                theLArSporadicNoiseMasker.DoMasking = True
                theLArSporadicNoiseMasker.ProblemsToMask = ["sporadicBurstNoise"]
                ToolSvc += theLArSporadicNoiseMasker
                theLArCellNoiseMaskingTool.MaskingSporadicTool = theLArSporadicNoiseMasker

            if doNoiseMask:
                try:
                    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
                    theLArNoiseMasker = LArBadChannelMasker("LArNoiseMasker")
                except:
                    mlog.error("could not access bad channel tool Quit")
                    print traceback.format_exc()
                    return False
                theLArNoiseMasker.DoMasking=True
                theLArNoiseMasker.ProblemsToMask= ["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"]
                ToolSvc+=theLArNoiseMasker
                theLArCellNoiseMaskingTool.MaskingTool = theLArNoiseMasker

            theLArCellNoiseMaskingTool.maskNoise = doNoiseMask
            theLArCellNoiseMaskingTool.maskSporadic = doSporadicMask
            # quality cut for sporadic noise masking
            theLArCellNoiseMaskingTool.qualityCut=4000
            theCaloCellMaker += theLArCellNoiseMaskingTool
            theCaloCellMaker.CaloCellMakerToolNames += [theLArCellNoiseMaskingTool]

        # 
        #  masking of Feb problems
        #
        doBadFebMasking = False
        if jobproperties.CaloCellFlags.doLArBadFebMasking.statusOn and jobproperties.CaloCellFlags.doLArBadFebMasking():
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.DataSource() == 'data':
                doBadFebMasking = True

        if doBadFebMasking:
            try:
                from LArCellRec.LArCellRecConf import LArBadFebMaskingTool
                theLArBadFebMaskingTool = LArBadFebMaskingTool()
                if (rec.doExpressProcessing() or athenaCommonFlags.isOnline()): # In online or express processing, EventInfo::LArError is triggered if >=4 FEB with data corrupted
                    theLArBadFebMaskingTool.minFebInError = 4
            except:
                mlog.error("could not get handle to LArBadFebMaskingTool Quit")
                print traceback.format_exc()
                return False
            theCaloCellMaker += theLArBadFebMaskingTool

            theCaloCellMaker.CaloCellMakerToolNames += [theLArBadFebMaskingTool]

        #
        #  emulate gain pathologies on MC
        #
        doGainPathology=False
        if jobproperties.CaloCellFlags.doLArCellGainPathology.statusOn and jobproperties.CaloCellFlags.doLArCellGainPathology():
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.DataSource() == 'geant4': 
                doGainPathology=True

        if doGainPathology:
            try:
                from LArCellRec.LArCellRecConf import LArCellGainPathology
                theLArCellGainPathology = LArCellGainPathology()
            except:
                mlog.error("could not get handle to LArCellGainPatholog< Quit")
                print traceback.format_exc()
                return False
            theCaloCellMaker += theLArCellGainPathology

            theCaloCellMaker.CaloCellMakerToolNames += [theLArCellGainPathology]




        # lar miscalibration if MC only  (should be done after finalisation)  

        if not jobproperties.CaloCellFlags.doLArCellEmMisCalib.statusOn:
            # the flag has not been set, so decide a reasonable default
            # this is the old global flags should use the new one as
            # soon as monitoring does 
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.DataSource() == 'data':
                doLArCellEmMisCalib = False
                mlog.info("jobproperties.CaloCellFlags.doLArMisCalib not set and real data: do not apply LArCellEmMisCalibTool")
            else:
                doLArCellEmMisCalib = True
                mlog.info("jobproperties.CaloCellFlags.doLArMisCalib not set and Monte Carlo: apply LArCellEmMisCalibTool")          
        else:
            doLArCellEmMisCalib=jobproperties.CaloCellFlags.doLArCellEmMisCalib()
            if doLArCellEmMisCalib:
                mlog.info("LArCellEmMisCalibTool requested")
            else:    
                mlog.info("LArCellEmMisCalibTool explicitly not requested")
                
        if doLArCellEmMisCalib:        
            try:
                from LArCellRec.LArCellRecConf import LArCellEmMiscalib
                theLArCellEmMiscalib = LArCellEmMiscalib("LArCellEmMiscalib")
            except:
                mlog.error("could not get handle to LArCellEmMisCalib Quit")
                print traceback.format_exc()
                return False

            # examples on how to change miscalibration. Default values are 0.005 and 0.007 
            #        theLArCellEmMiscalib.SigmaPerRegion = 0.005;
            #        theLArCellEmMiscalib.SigmaPerCell = 0.005;

            ToolSvc += theLArCellEmMiscalib


            try:
                from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
                from CaloIdentifier import SUBCALO
                theMisCalibTool = CaloCellContainerCorrectorTool("MisCalibTool",
                        CaloNums=[ SUBCALO.LAREM ],
                        CellCorrectionToolNames=[ theLArCellEmMiscalib])
            except:
                mlog.error("could not get handle to MisCalibTool Quit")
                print traceback.format_exc()
                return False

            theCaloCellMaker+=theMisCalibTool
            theCaloCellMaker.CaloCellMakerToolNames += [theMisCalibTool]

        #
        # Pedestal shift correction
        #
        doPedestalCorr = False
        if jobproperties.CaloCellFlags.doPedestalCorr.statusOn:
           from AthenaCommon.GlobalFlags import globalflags
           if jobproperties.CaloCellFlags.doPedestalCorr() and (globalflags.DataSource() == 'data' or jobproperties.CaloCellFlags.doPileupOffsetBCIDCorr) : 
               doPedestalCorr = True
               mlog.info("Apply cell level pedestal shift correction")

        if doPedestalCorr:
            try:
                from CaloCellCorrection.CaloCellPedestalCorrDefault import CaloCellPedestalCorrDefault
                theCaloCellPedestalCorr = CaloCellPedestalCorrDefault()
                theCaloCellMaker += theCaloCellPedestalCorr
                theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellPedestalCorr]
            except:
                mlog.error("could not get handle to CaloCellPedestalCorr")
                print traceback.format_exc()


        # 
        # HV correction for offline reprocessing, reading HV from Cool-DCS database
        #
        doHVCorr = False
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.dcs.LAr_on():
            if jobproperties.CaloCellFlags.doLArHVCorr.statusOn:
               from AthenaCommon.GlobalFlags import globalflags
               if jobproperties.CaloCellFlags.doLArHVCorr() and globalflags.DataSource() == 'data': 
                   doHVCorr = True
                   mlog.info("Redoing HV correction at cell level from COOL/DCS database")
       

        if doHVCorr:
            from LArCellRec.LArCellRecConf import LArCellHVCorrAlg
            theLArCellHVCorrAlg = LArCellHVCorrAlg()
            svcMgr.ToolSvc += theLArCellHVCorrAlg
            
            try:
                from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
                from CaloIdentifier import SUBCALO 
                theHVCorrTool = CaloCellContainerCorrectorTool("HVCorrTool",
                        CaloNums=[ SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.LARFCAL ],
                        CellCorrectionToolNames=[ theLArCellHVCorrAlg])
            except:
                mlog.error("could not get handle to HVCorrTool Quit")
                print traceback.format_exc()
                return False
            theCaloCellMaker += theHVCorrTool
            theCaloCellMaker.CaloCellMakerToolNames += [theHVCorrTool]

        #
        # correction to undo online calibration and apply new LAr electronics calibration for ADC->MeV conversion
        #
        doLArRecalibration = False
        if jobproperties.CaloCellFlags.doLArRecalibration.statusOn:
           from AthenaCommon.GlobalFlags import globalflags
           from LArConditionsCommon.LArCondFlags import larCondFlags
           if jobproperties.CaloCellFlags.doLArRecalibration() and globalflags.DataSource() == 'data' and (not larCondFlags.SingleVersion()) :
               doLArRecalibration = True
               mlog.info("Redoing LAr electronics calibration for ADC->MeV")


        if doLArRecalibration:

            # get tool for cell recalibration
            try:
                from LArCellRec.LArCellRecConf import LArCellRecalibration
                theLArCellRecalibration = LArCellRecalibration("LArCellRecalibration")
            except:
                mlog.error("could not get handle to LArCellRecalibration Quit")
                print traceback.format_exc()
                return False
            ToolSvc += theLArCellRecalibration

            # get new ADC2MeVTool
            try:
                from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
                theLArADC2MeVToolDefault = LArADC2MeVToolDefault()
            except:
                mlog.error("Could not get handle to LArADC2MeVToolDefault Quit")
                print traceback.format_exc()
                return False
            ToolSvc += theLArADC2MeVToolDefault

            # get old  ADC2MeVTool
            try:
                from LArRecUtils.LArADC2MeVToolOnline import LArADC2MeVToolOnline
                theLArADC2MeVToolOnline = LArADC2MeVToolOnline()
            except:
                mlog.error("Could not get handle to LArADC2MeVToolOnline Quit")
                print traceback.format_exc()
                return False
            ToolSvc += theLArADC2MeVToolOnline

            theLArCellRecalibration.adc2MeVTool = theLArADC2MeVToolDefault
            theLArCellRecalibration.adc2MeVToolOnline = theLArADC2MeVToolOnline


            try:
                from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
                from CaloIdentifier import SUBCALO 
                theLArRecalibrationTool = CaloCellContainerCorrectorTool("LArRecalibrationTool",
                        CaloNums=[ SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.LARFCAL ],
                        CellCorrectionToolNames=[ theLArCellRecalibration])
            except:
                mlog.error("could not get handle to HVCorrTool Quit")
                print traceback.format_exc()
                return False
            theCaloCellMaker += theLArRecalibrationTool
            theCaloCellMaker.CaloCellMakerToolNames += [theLArRecalibrationTool]


        #
        # Correction for MinBias energy shift for MC pileup reco
        #
        doMinBiasAverage = False
        if jobproperties.CaloCellFlags.doMinBiasAverage.statusOn:
           from AthenaCommon.GlobalFlags import globalflags  
           from AthenaCommon.BeamFlags import jobproperties
           if jobproperties.CaloCellFlags.doMinBiasAverage() and globalflags.DataSource() == 'geant4' and (not jobproperties.Beam.zeroLuminosity()):
              doMinBiasAverage = True
        
        if doMinBiasAverage:

           try:
               from CaloTools.CaloMBAverageToolDefault import CaloMBAverageToolDefault
               theCaloMBAverageTool = CaloMBAverageToolDefault()
           except:
               mlog.error("could not get handle to CaloMBAverageTool  Quit")
               print traceback.format_exc()
               return False
           ToolSvc+=theCaloMBAverageTool

           try:
               from CaloCellCorrection.CaloCellCorrectionConf import CaloCellMBAverageCorr
               theCaloCellMBAverageCorr = CaloCellMBAverageCorr("CaloCellMBAverageCorr")
               theCaloCellMBAverageCorr.CaloMBAverageTool = theCaloMBAverageTool
           except:
               mlog.error("could not get handle to  CaloCellMBAverageCorr  Quit")
               print traceback.format_exc()
               return False
           ToolSvc +=  theCaloCellMBAverageCorr

           try:
              from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
              from CaloIdentifier import SUBCALO 
              theMBAverageTool = CaloCellContainerCorrectorTool("MBAverageTool",
                           CaloNums = [ SUBCALO.NSUBCALO],
                           CellCorrectionToolNames = [theCaloCellMBAverageCorr] )
           except:
               mlog.error("could not get handle to CaloCellContainerCorrectorTool/MBAverageTool Quit")
               print traceback.format_exc()
               return False

           theCaloCellMaker += theMBAverageTool
           theCaloCellMaker.CaloCellMakerToolNames += [theMBAverageTool]


        #
        # Correction for dead cells, where we average the energy density of neighbor cells                     
        #
        doNeighborsAverage = False
        if jobproperties.CaloCellFlags.doDeadCellCorr.statusOn:
            if jobproperties.CaloCellFlags.doDeadCellCorr():
                doNeighborsAverage = True

        if doNeighborsAverage :
           try:
               from CaloCellCorrection.CaloCellCorrectionConf import CaloCellNeighborsAverageCorr
               theCaloCellNeighborsAverageCorr = CaloCellNeighborsAverageCorr("CaloCellNeighborsAverageCorr")
               theCaloCellNeighborsAverageCorr.testMode = False;
           except:
               mlog.error("could not get handle to  CaloCellNeighborsAverageCorr  Quit")
               print traceback.format_exc()
               return False
           theCaloCellMaker +=  theCaloCellNeighborsAverageCorr
           theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellNeighborsAverageCorr]


        # 
        # correction for missing Febs based on L1 readout
        doLArDeadOTXCorr = False
        if jobproperties.CaloCellFlags.doLArDeadOTXCorr.statusOn and jobproperties.CaloCellFlags.doLArCreateMissingCells.statusOn :
           if jobproperties.CaloCellFlags.doLArDeadOTXCorr() and jobproperties.CaloCellFlags.doLArCreateMissingCells() and doStandardCellReconstruction:
               if rec.doTrigger():
                   doLArDeadOTXCorr=True
               else:
                   mlog.warning("Trigger is switched off. Can't run deadOTX correction.")

        if doLArDeadOTXCorr:

            try:
                from LArCellRec.LArCellDeadOTXCorrToolDefault import LArCellDeadOTXCorrToolDefault
                theLArCellDeadOTXCorr = LArCellDeadOTXCorrToolDefault()
            except:
                mlog.error("could not get handle to LArCellDeadOTXCorr Quit")
                print traceback.format_exc()

            theCaloCellMaker += theLArCellDeadOTXCorr
            theCaloCellMaker.CaloCellMakerToolNames += [theLArCellDeadOTXCorr]

        doCaloEnergyRescaler=False
        if jobproperties.CaloCellFlags.doCaloCellEnergyCorr() and globalflags.DataSource() == 'data' and not athenaCommonFlags.isOnline():
            
            try:
                from CaloCellCorrection.CaloCellCorrectionConf import CaloCellEnergyRescaler
                theCCERescalerTool = CaloCellEnergyRescaler()
                theCCERescalerTool.Folder = "/LAR/CellCorrOfl/EnergyCorr"
                from IOVDbSvc.CondDB import conddb
                # conddb.addFolder("","/LAR/CellCorrOfl/EnergyCorr<tag>EnergyScale-00</tag><db>sqlite://;schema=escale.db;dbname=COMP200</db>")
                conddb.addFolder("LAR_OFL", "/LAR/CellCorrOfl/EnergyCorr")
                theCaloCellMaker += theCCERescalerTool
                theCaloCellMaker.CaloCellMakerToolNames += [theCCERescalerTool]
            except:
                mlog.error("could not get handle to CaloCellEnergyRescaler Quit")
                print traceback.format_exc()
                return False
            pass


        if jobproperties.CaloCellFlags.doCaloCellTimeCorr() and globalflags.DataSource() == 'data' and not athenaCommonFlags.isOnline():
            try:
                from CaloRec.CaloRecConf import CaloCellContainerCorrectorTool
                from CaloCellCorrection.CaloCellCorrectionConf import CaloCellTimeCorrTool
                theLArTimeCorr = CaloCellTimeCorrTool()
                theLArTimeCorr.Folder = "/LAR/TimeCorrectionOfl/CellTimeOffset"
                ToolSvc += theLArTimeCorr
                from IOVDbSvc.CondDB import conddb
                # conddb.addFolder("","/LAR/TimeCorrectionOfl/CellTimeOffset<tag>LARTimeCorrectionOflCellTimeOffset-empty</tag><db>sqlite://;schema=timecorr.db;dbname=COMP200</db>")
                conddb.addFolder("LAR_OFL", "/LAR/TimeCorrectionOfl/CellTimeOffset")
                theCaloTimeCorrTool = CaloCellContainerCorrectorTool("LArTimeCorrTool",
                                                                   CellCorrectionToolNames=[theLArTimeCorr]
                                                                   )
                theCaloCellMaker += theCaloTimeCorrTool
                theCaloCellMaker.CaloCellMakerToolNames += [theCaloTimeCorrTool]
                
            except:
                mlog.error("could not get handle to CaloCellTimeCorrTool Quit")
                print traceback.format_exc()
                return False
  
            pass
    
        
        # make lots of checks (should not be necessary eventually)
        # to print the check add:

        from CaloRec.CaloRecConf import CaloCellContainerCheckerTool   
        theCaloCellContainerCheckerTool = CaloCellContainerCheckerTool()     
        # FIXME
        # theCaloCellContainerCheckerTool.OutputLevel=DEBUG

        theCaloCellMaker += theCaloCellContainerCheckerTool
        theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool] 


        #

        # sets output key  
        theCaloCellMaker.CaloCellsOutputName=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore
        objKeyStore.addStreamESD(self.outputType(),self.outputKey())

        # Also note that we produce it as a transient output.
        objKeyStore.addTransient (self.outputType(),self.outputKey())

        from TileRecUtils.TileDQstatusAlgDefault import TileDQstatusAlgDefault
        TileDQstatusAlgDefault (TileRawChannelContainer = rawChannelContainer)
        
        # now add algorithm to topSequence
        # this should always come at the end

        mlog.info(" now adding CaloCellMaker to topSequence")        

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

        topSequence += theCaloCellMaker ;
        
        return True

    def CaloCellMakerHandle(self):
        return self._CaloCellMakerHandle

   
# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType



