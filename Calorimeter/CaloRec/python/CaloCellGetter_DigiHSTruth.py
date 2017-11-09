# specifies Calo cell making
# so far only handle the RawChannel->CaloCell step
# not all possibility of CaloCellMaker_jobOptions.py integrated yet
from AthenaCommon.Constants import *
from RecExConfig.Configured import Configured
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec

class CaloCellGetter_DigiHSTruth (Configured)  :
    _outputType = "CaloCellContainer"
    _output = { _outputType : "AllCalo_DigiHSTruth" }

        
    def configure(self):
        from AthenaCommon.Logging import logging
        mlog = logging.getLogger('CaloCellGetter_DigiHSTruth::configure:')
        mlog.info ('entering')        
        from CaloRec.CaloCellFlags import jobproperties
        doStandardCellReconstruction = True

        # get handle to upstream object
        # handle tile

        if doStandardCellReconstruction:
            # handle LAr
            import traceback
            try:
                from LArROD.LArRawChannelGetter_DigiHSTruth import LArRawChannelGetter_DigiHSTruth
                theLArRawChannelGetter = LArRawChannelGetter_DigiHSTruth()
            except:
                mlog.error("could not get handle to LArRawChannel_DigiHSTruth Quit")
                print traceback.format_exc()
                return False

            if not theLArRawChannelGetter.usable():
                if not self.ignoreConfigError():
                    mlog.error("LArRawChannelGetter_DigiHSTruth unusable. Quit.")
                    return False
                else:
                    mlog.error("LArRawChannelGetter_DigiHSTruth unusable. Continue nevertheless")


        # now configure the algorithm, part of this could be done in a separate class
        # cannot have same name
        try:        
            from CaloRec.CaloRecConf import CaloCellMaker                
        except:
            mlog.error("could not import CaloRec.CaloCellMaker")
            print traceback.format_exc()
            return False

        theCaloCellMaker = CaloCellMaker("CaloCellMaker_DigiHSTruth")
        self._CaloCellMakerHandle = theCaloCellMaker ;

        from AthenaCommon.AppMgr import ToolSvc

        if doStandardCellReconstruction:
            # configure CaloCellMaker here
            # check LArCellMakerTool_jobOptions.py for full configurability 
            # FIXME 

            from RecExConfig.RecFlags import rec

            if rec.doLArg():
                try:
                    from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool
                    theLArCellBuilder = LArCellBuilderFromLArRawChannelTool("LArCellBuilder_DigiHSTruth")
                    theLArCellBuilder.RawChannelsName = "LArRawChannels_DigiHSTruth"
                except:
                    mlog.error("could not get handle to LArCellBuilderFromLArRawChannel Quit")
                    print traceback.format_exc()
                    return False

                if jobproperties.CaloCellFlags.doLArCreateMissingCells():
                    # bad channel tools
                    try:
                        from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
                        theLArBadChannelTool = LArBadChanTool()
                    except:
                        mlog.error("could not access bad channel tool Quit")
                        print traceback.format_exc()
                        return False
                    ToolSvc += theLArBadChannelTool
                    theLArCellBuilder.addDeadOTX = True
                    theLArCellBuilder.badChannelTool = theLArBadChannelTool

                # add the tool to list of tool ( should use ToolHandle eventually) 
                ToolSvc += theLArCellBuilder
                theCaloCellMaker.CaloCellMakerToolNames += [theLArCellBuilder]



            #if rec.doTile():

                from AthenaCommon.GlobalFlags import globalflags
                try:
                    from TileRecUtils.TileRecUtilsConf import TileCellBuilder
                    theTileCellBuilder = TileCellBuilder("TileCellBuilder_DigiHSTruth")
                    #from TileRecUtils.TileRecFlags import jobproperties
                    theTileCellBuilder.TileRawChannelContainer = "TileRawChannelCnt_DigiHSTruth"
                    #theTileCellBuilder.TileRawChannelContainer = "TileRawChannelCnt"
                    theTileCellBuilder.E4prContainer = "E4prContainer2_DigiHSTruth"
                    theTileCellBuilder.MBTSContainer = "MBTSContainer2_DigiHSTruth"
                    theTileCellBuilder.TileDSPRawChannelContainer = "TileRawChannelCnt_DigiHSTruth"
                    if not hasattr( ToolSvc, "TileBeamInfoProvider" ):
                        from TileRecUtils.TileRecUtilsConf import TileBeamInfoProvider
                        ToolSvc += TileBeamInfoProvider()
                    ToolSvc += theTileCellBuilder
                    theCaloCellMaker.CaloCellMakerToolNames += [theTileCellBuilder]
                except:    
                    mlog.error("could not get handle to TileCellBuilder Quit")
                    print traceback.format_exc()
                    return False
            

        #
        # CaloCellContainerFinalizerTool : closing container and setting up iterators
        #
    
        from CaloRec.CaloRecConf import CaloCellContainerFinalizerTool     
        theCaloCellContainerFinalizerTool=CaloCellContainerFinalizerTool()
        ToolSvc += theCaloCellContainerFinalizerTool
        theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerFinalizerTool ]
        from RecExConfig.RecFlags import rec


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

            # bad channel tools
            try:
                from LArBadChannelTool.LArBadChannelToolConf import LArBadChanTool
                theLArBadChannelTool = LArBadChanTool()
            except:
                mlog.error("could not access bad channel tool Quit")
                print traceback.format_exc()
                return False
            ToolSvc += theLArBadChannelTool

            if doSporadicMask:
                try:
                    from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
                    theLArSporadicNoiseMasker = LArBadChannelMasker("LArSporadicNoiseMasker")
                except:
                    mlog.error("could not access bad channel tool Quit")
                    print traceback.format_exc()
                    return False
                theLArSporadicNoiseMasker.TheLArBadChanTool = theLArBadChannelTool
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
                theLArNoiseMasker.TheLArBadChanTool = theLArBadChannelTool
                theLArNoiseMasker.DoMasking=True
                theLArNoiseMasker.ProblemsToMask= ["highNoiseHG","highNoiseMG","highNoiseLG","deadReadout","deadPhys"]
                ToolSvc+=theLArNoiseMasker
                theLArCellNoiseMaskingTool.MaskingTool = theLArNoiseMasker

            theLArCellNoiseMaskingTool.maskNoise = doNoiseMask
            theLArCellNoiseMaskingTool.maskSporadic = doSporadicMask
            # quality cut for sporadic noise masking
            theLArCellNoiseMaskingTool.qualityCut=4000
            ToolSvc += theLArCellNoiseMaskingTool
            theCaloCellMaker.CaloCellMakerToolNames += [theLArCellNoiseMaskingTool]


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
            ToolSvc += theLArCellGainPathology

            theCaloCellMaker.CaloCellMakerToolNames += [theLArCellGainPathology]


        # lar miscalibration if MC only  (should be done after finalisation)  

        if not jobproperties.CaloCellFlags.doLArCellEmMisCalib.statusOn:
            # the flag has not been set, so decide a reasonable default
            # this is the old global flags should use the new one as
            # soon as monitoring does 
            doLArCellEmMisCalib = True
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

            ToolSvc+=theMisCalibTool
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

        import os
        if doPedestalCorr and os.getenv("CMTPATH") and "AtlasTrigger" in os.getenv("CMTPATH"):
            try:
                from CaloCellCorrection.CaloCellPedestalCorrDefault import CaloCellPedestalCorrDefault
                theCaloCellPedestalCorr = CaloCellPedestalCorrDefault()
                ToolSvc += theCaloCellPedestalCorr
                theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellPedestalCorr]
            except:
                mlog.error("could not get handle to CaloCellPedestalCorr")
                print traceback.format_exc()




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

           ToolSvc += theMBAverageTool
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
           ToolSvc +=  theCaloCellNeighborsAverageCorr
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

            ToolSvc += theLArCellDeadOTXCorr
            theCaloCellMaker.CaloCellMakerToolNames += [theLArCellDeadOTXCorr]

        
        # make lots of checks (should not be necessary eventually)
        # to print the check add:

        from CaloRec.CaloRecConf import CaloCellContainerCheckerTool   
        theCaloCellContainerCheckerTool = CaloCellContainerCheckerTool()     
        # FIXME
        # theCaloCellContainerCheckerTool.OutputLevel=DEBUG

        ToolSvc += theCaloCellContainerCheckerTool
        theCaloCellMaker.CaloCellMakerToolNames += [theCaloCellContainerCheckerTool] 


        # sets output key  
        theCaloCellMaker.CaloCellsOutputName=self.outputKey()        


        # register output in objKeyStore
        from RecExConfig.ObjKeyStore import objKeyStore

        objKeyStore.addStreamESD(self.outputType(),self.outputKey())


        
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



