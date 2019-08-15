# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg

def LArRawDataReadingCfg(configFlags):
    acc=ComponentAccumulator()
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    acc.merge(DetDescrCnvSvcCfg(configFlags))
    acc.merge(ByteStreamReadCfg(configFlags))    
    acc.addEventAlgo(LArRawDataReadingAlg())
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
    
    from LArEventTest.LArEventTestConf import DumpLArRawChannels
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    acc.merge(LArOnOffIdMappingCfg(ConfigFlags))
    acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels",))

    f=open("LArRawDataReading.pkl","w")
    acc.store(f)
    f.close()
