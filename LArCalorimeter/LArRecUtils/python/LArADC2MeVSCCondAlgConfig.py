# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArADC2MeVSCCondAlgCfg(configFlags):
    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg, LArElecCalibDBMCSCCfg
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg

    result=ComponentAccumulator()
    from LArCabling.LArCablingConfig import LArOnOffIdMappingSCCfg
    result.merge(LArOnOffIdMappingSCCfg(configFlags))
    result.merge(LArGMCfg(configFlags)) #Needed for identifier helpers

    theADC2MeVCondAlg=CompFactory.LArADC2MeVCondAlg(name = "LArADC2MeVSCCondAlg", LArADC2MeVKey = 'LArADC2MeVSC')

    isMC=configFlags.Input.isMC

    if isMC:
        requiredConditions=["RampSC","DAC2uASC","uA2MeVSC"]
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSC"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASC"
        theADC2MeVCondAlg.LArRampKey="LArRampSC"
        theADC2MeVCondAlg.LArMphysOverMcalKey=""
        theADC2MeVCondAlg.LArHVScaleCorrKey=""
        theADC2MeVCondAlg.UseFEBGainTresholds=False
        theADC2MeVCondAlg.LArOnOffIdMappingKey="LArOnOffIdMapSC"
        theADC2MeVCondAlg.isSuperCell=True
        result.merge(LArElecCalibDBMCSCCfg(configFlags,requiredConditions))
    else: # not MC:
        requiredConditions=["Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr"]
        from LArRecUtils.LArFebConfigCondAlgConfig import LArFebConfigCondAlgCfg
        if 'COMP200' in configFlags.IOVDb.DatabaseInstance: # Run1 case
            theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSC"
            theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASC"
        result.merge(LArFebConfigCondAlgCfg(configFlags))
        result.merge(LArElecCalibDbCfg(configFlags,requiredConditions))
    result.addCondAlgo(theADC2MeVCondAlg,primary=True)
    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    print ('--- data')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = LArADC2MeVSCCondAlgCfg (flags1)
    acc1.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    acc1.wasMerged()

    print ('--- mc')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.ESD
    flags2.lock()

    acc2 = LArADC2MeVSCCondAlgCfg (flags2)
    acc2.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc2.getService('IOVDbSvc').Folders)
    acc2.wasMerged()
