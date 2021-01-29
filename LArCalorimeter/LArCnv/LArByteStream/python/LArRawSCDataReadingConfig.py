# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import os
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
LArRawSCDataReadingAlg=CompFactory.LArRawSCDataReadingAlg
LArLATOMEDecoder=CompFactory.LArLATOMEDecoder

def LArRawSCDataReadingCfg(configFlags, **kwargs):
    acc=ComponentAccumulator()
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    acc.merge(DetDescrCnvSvcCfg(configFlags))
    acc.merge(ByteStreamReadCfg(configFlags))

    LArRawSCDataReadingAlg1 = LArRawSCDataReadingAlg(**kwargs)
    LArRawSCDataReadingAlg1.LATOMEDecoder = LArLATOMEDecoder("LArLATOMEDecoder")
    LArRawSCDataReadingAlg1.LATOMEDecoder.latomeInfoFileName = "LATOME/latomeInput.txt"
    LArRawSCDataReadingAlg1.LATOMEDecoder.latomeInfoFilePath = str(os.environ['TestArea'])+"/../run/"
    LArRawSCDataReadingAlg1.LATOMEDecoder.ProtectSourceId = True
    acc.addEventAlgo(LArRawSCDataReadingAlg1)
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

    acc = MainServicesCfg( ConfigFlags )
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    acc.merge(AtlasGeometryCfg(ConfigFlags))
    acc.merge(LArRawSCDataReadingCfg(ConfigFlags))
    
    acc.run(2,OutputLevel=DEBUG)

