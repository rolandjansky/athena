# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
from AthenaConfiguration.MainServicesConfig import MainServicesCfg

def LArRawCalibDataReadingCfg(configFlags,gain="HIGH",doAccDigit=False,doAccCalibDigit=False,doCalibDigit=False):
    acc=ComponentAccumulator()
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    acc.merge(LArGMCfg(configFlags))
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

    acc.addEventAlgo(CompFactory.LArRawCalibDataReadingAlg(LArAccDigitKey=accKey, 
                                      LArAccCalibDigitKey=accCalibKey,
                                      LArCalibDigitKey=calibKey, LArFebHeaderKey="LArFebHeader"))
    return acc


if __name__=="__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    ConfigFlags.LAr.doAlign=False
    ConfigFlags.Input.Files = ["/eos/atlas/atlastier0/rucio/data22_calib/calibration_LArElec-Pedestal-32s-High-All/00420537/data22_calib.00420537.calibration_LArElec-Pedestal-32s-High-All.daq.RAW/data22_calib.00420537.calibration_LArElec-Pedestal-32s-High-All.daq.RAW._lb0000._SFO-4._0001.data"]
    ConfigFlags.lock()

    acc = MainServicesCfg( ConfigFlags )
    acc.merge(LArRawCalibDataReadingCfg(ConfigFlags,doAccCalibDigit=True))
    
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    acc.merge(LArOnOffIdMappingCfg(ConfigFlags))

    #f=open("LArRawCalibDataReading.pkl","wb")
    #acc.store(f)
    #f.close()
    acc.run(-1,OutputLevel=DEBUG)
