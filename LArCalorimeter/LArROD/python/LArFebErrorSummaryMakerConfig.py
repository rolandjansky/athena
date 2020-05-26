# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
LArFebErrorSummaryMaker=CompFactory.LArFebErrorSummaryMaker
from LArBadChannelTool.LArBadChannelConfig import LArBadFebCfg

def LArFebErrorSummaryMakerCfg(configFlags):

    febSummaryMaker =LArFebErrorSummaryMaker()
    febSummaryMaker.MaskFebScacStatus = [0x38080000]
    febSummaryMaker.MaskFebEvtId      = [0x38080000]
    febSummaryMaker.WriteKey="StoreGateSvc+LArFebErrorSummary"
    # needed only if it is not in DB.
    #febSummaryMaker.MaskFebZeroSample = [0x39618000,0x39930000,0x3b1b0000,0x38db0000,0x38f60000,0x39ae8000,0x3bb90000]

    #from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = LArBadFebCfg(configFlags)

    acc.addEventAlgo(febSummaryMaker)

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


    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from LArByteStream.LArRawDataReadingConfig import LArRawDataReadingCfg    

    acc=MainServicesCfg(ConfigFlags)
    acc.merge(LArRawDataReadingCfg(ConfigFlags))
    acc.merge(LArFebErrorSummaryMakerCfg(ConfigFlags))
    
    acc.run(3)
