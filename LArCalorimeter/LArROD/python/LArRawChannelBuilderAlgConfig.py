# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
LArRawChannelBuilderAlg=CompFactory.LArRawChannelBuilderAlg
from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
from LArRecUtils.LArRecUtilsConfig import LArOFCCondAlgCfg

def LArRawChannelBuilderAlgCfg(configFlags, **kwargs):

    acc = LArADC2MeVCondAlgCfg(configFlags)

    kwargs.setdefault("name", "LArRawChannelBuilder")
    kwargs.setdefault("firstSample", configFlags.LAr.ROD.FirstSample)
    if configFlags.Input.isMC:
        # need OFC configuration, which includes appropriate ElecCalibDb
        acc.merge(LArOFCCondAlgCfg(configFlags))
        kwargs.setdefault("LArRawChannelKey", "LArRawChannels")
        kwargs.setdefault("ShapeKey", "LArShapeSym")
        fld="/LAR/NoiseOfl/DSPThresholds"
        dbString="OFLP200"
        dbInstance="LAR_OFL"
        if configFlags.Digitization.PileUpPremixing:
            kwargs.setdefault("LArDigitKey", configFlags.Overlay.BkgPrefix() + "LArDigitContainer_MC")
        else:
            kwargs.setdefault("LArDigitKey", "LArDigitContainer_MC")
    else:
        acc.merge(LArElecCalibDbCfg(configFlags,("OFC","Shape","Pedestal")))
        if configFlags.Overlay.DataOverlay:
            kwargs.setdefault("LArDigitKey", "LArDigitContainer_MC")
            kwargs.setdefault("LArRawChannelKey", "LArRawChannels")
        else:
            kwargs.setdefault("LArRawChannelKey", "LArRawChannels_FromDigits")
        fld="/LAR/Configuration/DSPThresholdFlat/Thresholds"
        dbString="CONDBR2"
        dbInstance="LAR_ONL"

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    acc.merge(addFolders(configFlags,fld, dbInstance, className="AthenaAttributeList", db=dbString))

    kwargs.setdefault("DSPThresholdsKey",fld)

    acc.addEventAlgo(LArRawChannelBuilderAlg(**kwargs))

    return acc


if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.Input.isMC = False
    ConfigFlags.lock()


    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg    

    acc=MainServicesCfg(ConfigFlags)
    acc.merge(LArRawDataReadingCfg(ConfigFlags))
    acc.merge(LArRawChannelBuilderAlgCfg(ConfigFlags))
    
    DumpLArRawChannels=CompFactory.DumpLArRawChannels
    acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels_FromDigits",),sequenceName="AthAlgSeq")

    acc.run(3)
