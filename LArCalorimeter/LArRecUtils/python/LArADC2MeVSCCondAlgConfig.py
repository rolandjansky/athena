# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArADC2MeVSCCondAlgCfg(configFlags):
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    
    result=ComponentAccumulator()
    result.merge(LArOnOffIdMappingCfg(configFlags))
    result.merge(LArGMCfg(configFlags)) #Needed for identifier helpers

    theADC2MeVCondAlg=CompFactory.LArADC2MeVCondAlg(LArADC2MeVKey = 'LArADC2MeV')

    isMC=configFlags.Input.isMC
    
    if isMC:
        requiredConditions=["Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr"]
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSC"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASC"
        theADC2MeVCondAlg.LArRampKey="LArRampSC"
        theADC2MeVCondAlg.LArMphysOverMcalKey="LArMphysOverMcalSC"
        theADC2MeVCondAlg.LArHVScaleCorrKey="LArHVScaleCorr"
        theADC2MeVCondAlg.UseFEBGainTresholds=False
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
