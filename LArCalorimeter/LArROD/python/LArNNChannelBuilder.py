# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
from LArConfiguration.LArConfigFlags import RawChannelSource



def LArNNRawChannelBuilderCfg(configFlags, name="LArNNRawChannelBuilder", **kwargs):

    acc = LArADC2MeVCondAlgCfg(configFlags)

    kwargs.setdefault("firstSample", configFlags.LAr.ROD.FirstSample)

    if configFlags.Input.isMC:
        kwargs.setdefault("LArRawChannelKey", "LArRawChannels")

        if configFlags.Common.ProductionStep == ProductionStep.PileUpPresampling:
            kwargs.setdefault("LArDigitKey", configFlags.Overlay.BkgPrefix + "LArDigitContainer_MC")
        else:
            kwargs.setdefault("LArDigitKey", "LArDigitContainer_MC")
    else:
        acc.merge(LArElecCalibDbCfg(configFlags,("OFC","Shape","Pedestal")))
        if configFlags.Overlay.DataOverlay:
            kwargs.setdefault("LArDigitKey", "Bkg_LArDigitContainer_MC")
            kwargs.setdefault("LArRawChannelKey", "LArRawChannels")
        else:
            kwargs.setdefault("LArRawChannelKey", "LArRawChannels_FromDigits")

    if configFlags.LAr.ROD.forceIter or configFlags.LAr.RawChannelSource is RawChannelSource.Calculated:
        # iterative OFC procedure
        LArRawChannelBuilderIterAlg=CompFactory.LArRawChannelBuilderIterAlg
        kwargs.setdefault('minSample',2)
        kwargs.setdefault('maxSample',12)
        kwargs.setdefault('minADCforIterInSigma',4)
        kwargs.setdefault('minADCforIter',15)
        kwargs.setdefault('defaultPhase',12)
        nominalPeakSample=2
        from LArConditionsCommon.LArRunFormat import getLArFormatForRun
        larformat=getLArFormatForRun(configFlags.Input.RunNumber[0],connstring="COOLONL_LAR/"+configFlags.IOVDb.DatabaseInstance)
        if larformat is not None:
          nominalPeakSample = larformat.firstSample()
        else:
          print("WARNING: larformat not found, use nominalPeakSample = 2")
          nominalPeakSample = 2
        if (nominalPeakSample > 1) :
          kwargs.setdefault('DefaultShiftTimeSample',nominalPeakSample-2)
        else :
          kwargs.setdefault('DefaultShiftTimeSample',0)

        acc.addEventAlgo(LArRawChannelBuilderIterAlg(**kwargs))
    else:

        kwargs.setdefault('NNJsonPath', configFlags.LAr.ROD.nnJson)
        kwargs.setdefault('NetworkOutputNode', configFlags.LAr.ROD.nnOutputNode)
        kwargs.setdefault('NetworkInputNode', configFlags.LAr.ROD.nnInputNode)
       
        acc.addEventAlgo(CompFactory.LArNNRawChannelBuilder(name, **kwargs))

    return acc