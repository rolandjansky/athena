# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
LArRawCalibDataReadingAlg=CompFactory.LArRawCalibDataReadingAlg

def LArRawCalibDataReadingCfg(configFlags,gain="HIGH",doAccDigit=False,doAccCalibDigit=False,doCalibDigit=False):
    acc=ComponentAccumulator()
    from DetDescrCnvSvc.DetDescrCnvSvcConfig import DetDescrCnvSvcCfg
    acc.merge(DetDescrCnvSvcCfg(configFlags))
    acc.merge(ByteStreamReadCfg(configFlags))    
    accKey=""
    accCalibKey=""
    calibKey=""
    if doAccDigit:
       accKey=gain
    if doAccCalibDigit:
       accCalibKey=gain
    if doCalibDigit:
       calibKey=gain

    cread = LArRawCalibDataReadingAlg(LArAccDigitKey=accKey, LArAccCalibDigitKey=accCalibKey,
                                      LArCalibDigitKey=calibKey, LArFebHeaderKey="LArFebHeader")
    acc.addEventAlgo(cread)
    return acc


if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    log.setLevel(DEBUG)

    ConfigFlags.LAr.doAlign=False
    ConfigFlags.Input.Files = ["/eos/atlas/atlastier0/rucio/data20_calib/calibration_LArElec-Pedestal-32s-High-All/00383505/data20_calib.00383505.calibration_LArElec-Pedestal-32s-High-All.daq.RAW/data20_calib.00383505.calibration_LArElec-Pedestal-32s-High-All.daq.RAW._lb0000._SFO-3._0001.data"]
    ConfigFlags.lock()

    acc = MainServicesCfg( ConfigFlags )
    from AtlasGeoModel.AtlasGeoModelConfig import AtlasGeometryCfg
    acc.merge(AtlasGeometryCfg(ConfigFlags))
    acc.merge(LArRawCalibDataReadingCfg(ConfigFlags,doAccCalibDigit=True))
    
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    acc.merge(LArOnOffIdMappingCfg(ConfigFlags))

    #f=open("LArRawCalibDataReading.pkl","wb")
    #acc.store(f)
    #f.close()
    acc.run(-1,OutputLevel=DEBUG)
