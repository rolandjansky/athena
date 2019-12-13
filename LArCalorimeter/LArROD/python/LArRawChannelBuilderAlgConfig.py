# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from LArROD.LArRODConf import LArRawChannelBuilderAlg
from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
from LArRecUtils.LArRecUtilsConfig import LArOFCCondAlgCfg

def LArRawChannelBuilderAlgCfg(configFlags, **kwargs):

    acc = LArADC2MeVCondAlgCfg(configFlags)

    if configFlags.Input.isMC:
        # need OFC configuration, which includes appropriate ElecCalibDb
        acc.merge(LArOFCCondAlgCfg(configFlags))
        kwargs.setdefault("LArRawChannelKey", "LArRawChannels")
        kwargs.setdefault("ShapeKey", "LArShapeSym")
        if configFlags.Digitization.PileUpPremixing:
            kwargs.setdefault("LArDigitKey", configFlags.Overlay.BkgPrefix() + "LArDigitContainer_MC")
        else:
            kwargs.setdefault("LArDigitKey", "LArDigitContainer_MC")
    else:
        acc.merge(LArElecCalibDbCfg(configFlags,("OFC","Shape","Pedestal")))
        kwargs.setdefault("LArRawChannelKey", "LArRawChannels_FromDigits")

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
    ConfigFlags.lock()


    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg    

    acc=MainServicesSerialCfg()
    acc.merge(LArRawDataReadingCfg(ConfigFlags))
    acc.merge(LArRawChannelBuilderAlgCfg(ConfigFlags))
    
    from LArEventTest.LArEventTestConf import DumpLArRawChannels
    acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels_FromDigits",),sequenceName="AthAlgSeq")

    acc.run(3)
