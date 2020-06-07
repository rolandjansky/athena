# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
LArRawDataReadingAlg=CompFactory.LArRawDataReadingAlg

def LArRawDataReadingCfg(configFlags, **kwargs):
    acc=ComponentAccumulator()
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    acc.merge(DetDescrCnvSvcCfg(configFlags))
    acc.merge(ByteStreamReadCfg(configFlags))

    if configFlags.Overlay.DataOverlay:
        kwargs.setdefault("LArDigitKey", configFlags.Overlay.BkgPrefix + "FREE")
        kwargs.setdefault("LArRawChannelKey", "")

    kwargs.setdefault("FailOnCorruption",False)

    acc.addEventAlgo(LArRawDataReadingAlg(**kwargs))
    return acc


if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.LAr.doAlign=False
    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc=LArRawDataReadingCfg(ConfigFlags)
    
    DumpLArRawChannels=CompFactory.DumpLArRawChannels
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    acc.merge(LArOnOffIdMappingCfg(ConfigFlags))
    acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels",))

    f=open("LArRawDataReading.pkl","wb")
    acc.store(f)
    f.close()
