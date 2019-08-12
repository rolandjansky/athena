# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from LArROD.LArRODConf import LArRawChannelBuilderAlg
from LArRecUtils.LArADC2MeVCondAlgConfig import LArADC2MeVCondAlgCfg
from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg

def LArRawChannelBuilderAlgCfg(configFlags):
    acc=LArADC2MeVCondAlgCfg(configFlags)
    acc.merge(LArElecCalibDbCfg(configFlags,("OFC","Shape","Pedestal")))


    if configFlags.Input.isMC:
        #defaults are fine .. 
        acc.addEventAlgo(LArRawChannelBuilderAlg())
    else:
        acc.addEventAlgo(LArRawChannelBuilderAlg(LArRawChannelKey="LArRawChannels_fromDigits"))
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

    from LArByteStream.LArByteStreamConf import LArRawDataReadingAlg
    from  ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
    acc=ByteStreamReadCfg(ConfigFlags)
    acc.addEventAlgo(LArRawDataReadingAlg())
    acc.merge(LArRawChannelBuilderAlgCfg(ConfigFlags))
    
    from LArEventTest.LArEventTestConf import DumpLArRawChannels
    acc.addEventAlgo(DumpLArRawChannels(LArRawChannelContainerName="LArRawChannels_fromDigits",))

    f=open("LArRawChannelBuilderAlg.pkl","w")
    acc.store(f)
    f.close()
