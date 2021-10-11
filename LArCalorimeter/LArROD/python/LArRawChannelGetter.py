# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# specifies LArRawChannels getting

from AthenaCommon.Logging import logging
from RecExConfig.Configured import Configured

class LArRawChannelGetter ( Configured )  :
    _outputType = "LArRawChannelContainer"
    _output = { _outputType : "LArRawChannels" } # do not want to list all the IDC
        
    def configure(self):
        mlog = logging.getLogger( 'Py:LArRawChannelGetter::configure %s:' % self.__class__ )

        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        from AthenaCommon.AppMgr import ToolSvc


        # get LArDigitGetter in MC case
        from AthenaCommon.DetFlags import DetFlags
        if DetFlags.digitize.LAr_on() :
            try:
                from LArDigitization.LArDigitGetter import LArDigitGetter
                theLArDigitGetter = LArDigitGetter()
            except Exception:
                mlog.error("could not get handle to LArDigitGetter Quit")
                import traceback
                mlog.error(traceback.format_exc())
                return False
            if not theLArDigitGetter.usable():
                mlog.error("LArDigitGetter unusable. Quite")
                return False

        from LArROD.LArRODFlags import larRODFlags

        # ADC2MeV
        from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
        LArADC2MeVCondAlgDefault()

        from AthenaCommon.GlobalFlags import globalflags
        if globalflags.DataSource()=='data' or larRODFlags.forceIter() :


            from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Data case

            if larRODFlags.readDigits():

                from AthenaCommon.GlobalFlags import globalflags
                if globalflags.InputFormat() == 'bytestream':
                    if not larRODFlags.keepDSPRaw():
                        topSequence.LArRawDataReaderAlg.LArRawChannelKey=""

                if globalflags.DetGeo() == 'ctbh6' or globalflags.DetGeo() == 'ctbh8':        
                    from LArROD.LArRODConf import LArRawChannelBuilder
                    theLArRawChannelBuilder=LArRawChannelBuilder()
                    topSequence += theLArRawChannelBuilder
                    return True

                from LArROD.LArRODConf import LArRawChannelBuilderDriver

                theLArRawChannelBuilder=LArRawChannelBuilderDriver("LArRawChannelBuilder")
                topSequence += theLArRawChannelBuilder

                if larRODFlags.keepDSPRaw():
                    theLArRawChannelBuilder.LArRawChannelContainerName=larRODFlags.RawChannelFromDigitsContainerName()

                # bad channel masking if required
                if not larRODFlags.doBuildBadChannel():
                    # The first tool filters out bad channels
                    from LArROD.LArRODConf import LArRawChannelBuilderToolBadChannelTool
                    theLArRawChannelBuilderToolBadChannel=LArRawChannelBuilderToolBadChannelTool()
                    theLArRawChannelBuilderToolBadChannel.ProblemsToMask=[
                       "deadReadout","deadPhys","almostDead","short",
                       "lowNoiseHG","highNoiseHG","unstableNoiseHG",
                       "lowNoiseMG","highNoiseMG","unstableNoiseMG",
                       "lowNoiseLG","highNoiseLG","unstableNoiseLG"
                      ]
                    theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolBadChannel]
                    ToolSvc+=theLArRawChannelBuilderToolBadChannel

               
                # Pulse reconstruction
                # main method: OFC iteration
                from LArROD.LArRODConf import LArRawChannelBuilderToolOFCIter
                theLArRawChannelBuilderToolOFCIter=LArRawChannelBuilderToolOFCIter()
                theLArRawChannelBuilderToolOFCIter.minSample = 2
                theLArRawChannelBuilderToolOFCIter.maxSample = 12
                theLArRawChannelBuilderToolOFCIter.minADCforIterInSigma=4 # ADCmax at least 4 sigma above noise for iteration
                theLArRawChannelBuilderToolOFCIter.minADCforIter=15 # min adc for iteration (only if no pedestalRMS found)
                theLArRawChannelBuilderToolOFCIter.defaultPhase=0    # starting delay, also the fixed delay for ADC below min.
                theLArRawChannelBuilderToolOFCIter.ECut=250.         # Energy to save quality
                theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolOFCIter]
                theLArRawChannelBuilder+=theLArRawChannelBuilderToolOFCIter 

                # no fallback when emulating exactly DSP computation
                if not larRODFlags.doDSP():
                    # fallback(1): cubic method
                    from LArROD.LArRODConf import LArRawChannelBuilderToolCubic
                    theLArRawChannelBuilderToolCubic=LArRawChannelBuilderToolCubic()
                    theLArRawChannelBuilderToolCubic.minADCforCubic=30 
                    theLArRawChannelBuilder.BuilderTools  += [theLArRawChannelBuilderToolCubic]
                    theLArRawChannelBuilder += theLArRawChannelBuilderToolCubic 

                    # fallback(2) averageing
                    from LArROD.LArRODConf import LArRawChannelBuilderToolAverage
                    theLArRawChannelBuilderToolAverage=LArRawChannelBuilderToolAverage()
                    theLArRawChannelBuilderToolAverage.NScan=0
                    theLArRawChannelBuilder.BuilderTools  += [theLArRawChannelBuilderToolAverage]
                    theLArRawChannelBuilder += theLArRawChannelBuilderToolAverage 


                # Pedestal
                # main method from database
                from LArROD.LArRODConf import LArRawChannelBuilderPedestalDataBase
                theLArRawChannelBuilderPedestalDataBase=LArRawChannelBuilderPedestalDataBase()
                theLArRawChannelBuilderPedestalDataBase.LArPedestalKey = "LArPedestal"
                theLArRawChannelBuilder.PedestalTools  = [theLArRawChannelBuilderPedestalDataBase]
                theLArRawChannelBuilder += theLArRawChannelBuilderPedestalDataBase 
                
                # no fallback when emulating exactly DSP computation
                if not larRODFlags.doDSP():
                    # fallback. sample 0
                    from LArROD.LArRODConf import LArRawChannelBuilderPedestalSampleZero
                    theLArRawChannelBuilderPedestalSampleZero=LArRawChannelBuilderPedestalSampleZero()
                    theLArRawChannelBuilder.PedestalTools  += [theLArRawChannelBuilderPedestalSampleZero]
                    theLArRawChannelBuilder += theLArRawChannelBuilderPedestalSampleZero

                # ADC to energy
                # main method from database
                from LArROD.LArRODConf import LArRawChannelBuilderADC2EDataBase
                theLArRawChannelBuilderADC2EDataBase=LArRawChannelBuilderADC2EDataBase()
                theLArRawChannelBuilder.ADCtoEnergyTools  = [theLArRawChannelBuilderADC2EDataBase]
                theLArRawChannelBuilder += theLArRawChannelBuilderADC2EDataBase 

                # no fallback when emulating exactly DSP computation
                if not larRODFlags.doDSP():
                    # fallback, constant conversion factors
                    from LArROD.LArRODConf import LArRawChannelBuilderADC2EConstants
                    theLArRawChannelBuilderADC2EConstants=LArRawChannelBuilderADC2EConstants()
                    theLArRawChannelBuilder.ADCtoEnergyTools += [theLArRawChannelBuilderADC2EConstants]
                    theLArRawChannelBuilder += theLArRawChannelBuilderADC2EConstants

                #more tools to be configured
                from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
                theLArOFPeakRecoTool=LArOFPeakRecoTool()
                from LArConditionsCommon.LArCondFlags import larCondFlags
                theLArOFPeakRecoTool.UseShape=larCondFlags.useShape()
                if larCondFlags.LArCoolChannelSelection.statusOn:
                    if len(larCondFlags.LArCoolChannelSelection())>0:
                        theLArOFPeakRecoTool.forceHighGain=larCondFlags.useOFCOnlyCoolChannelSelection()
                ToolSvc += theLArOFPeakRecoTool

                #
                # adjust default timing to match first sample information written in cool from Lar online configuration
                #
                nominalPeakSample=2
                if globalflags.DataSource()=='data':
                    from LArConditionsCommon.LArCool import larcool
                    if (larcool is not None):
                        nominalPeakSample = larcool.firstSample()
                        # don't use ramp intercept in calibration if gain type is not auto
                        if larcool.gainType() > 0 :
                            mlog.info(" Gain Type: %d  don't use intercept in ADC to Energy ramp calibration",larcool.gainType())
                            theLArRawChannelBuilderADC2EDataBase.UseHighGainRampIntercept = False
                            theLArRawChannelBuilderADC2EDataBase.UseMedGainRampIntercept = False
                            theLArRawChannelBuilderADC2EDataBase.UseLowGainRampIntercept = False
                        else :
                             mlog.info(" Gain Type: %d   use intercept in ADC to energy ramp calibraion ",larcool.gainType())
                if (nominalPeakSample > 1) :
                    theLArRawChannelBuilder.DefaultShiftTimeSample=nominalPeakSample-2
                else :
                    theLArRawChannelBuilder.DefaultShiftTimeSample=0
                theLArRawChannelBuilder.DataLocation    = "FREE"

            else:

                from AthenaCommon.GlobalFlags import globalflags
                if globalflags.InputFormat() == 'bytestream':
                    if "LArRawChannelContainer/LArRawChannels" not in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
                        svcMgr.ByteStreamAddressProviderSvc.TypeNames+=["LArRawChannelContainer/LArRawChannels"]

            # In the case of DSP monitoring and reading rawchannels, need to give a different name to the LArRawChannels container
            # read from the bytestream ...
            # This name has to be coherent with the name in LArMonTools/LArRODMonTool_jobOptions.py
            if larRODFlags.doDSP() and larRODFlags.readRawChannels():  #Reading LArRawChannel
                print ("Reading RawChannels in DSP physics mode")
                # !!! The name of the LArRawChannels container read from the Bytestream is LArRawChannels_fB !!!
                if "LArRawChannelContainer/LArRawChannels_fB" not in svcMgr.ByteStreamAddressProviderSvc.TypeNames:
                    svcMgr.ByteStreamAddressProviderSvc.TypeNames+=["LArRawChannelContainer/LArRawChannels_fB"]
                print (svcMgr.ByteStreamAddressProviderSvc.TypeNames)
      
        else:

            # MC Case
            try:
                from AthenaCommon import CfgGetter
                topSequence += CfgGetter.getAlgorithm("LArRawChannelBuilder", tryDefaultConfigurable=True)
            except Exception:
                mlog.error("Failed to retrieve LArRawChannelBuilder. Quit")
                import traceback
                mlog.error(traceback.format_exc())
                return False

        return True

# would work only if one output object type
    def outputKey(self):
        return self._output[self._outputType]

    def outputType(self):
        return self._outputType

    def outputTypeKey(self):
        return str(self.outputType()+"#"+self.outputKey())
