# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArADC2MeVCondAlgCfg(configFlags):
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
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSym"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASym"
        theADC2MeVCondAlg.LArRampKey="LArRampSym"
        theADC2MeVCondAlg.LArMphysOverMcalKey="LArMphysOverMcalSym"
        theADC2MeVCondAlg.LArHVScaleCorrKey="LArHVScaleCorr"
        theADC2MeVCondAlg.UseFEBGainTresholds=False
    else: # not MC:
        requiredConditions=["Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr"]
        from LArRecUtils.LArFebConfigCondAlgConfig import LArFebConfigCondAlgCfg
        if 'COMP200' in configFlags.IOVDb.DatabaseInstance: # Run1 case
            theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSym"
            theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASym"
        result.merge(LArFebConfigCondAlgCfg(configFlags))

    result.merge(LArElecCalibDbCfg(configFlags,requiredConditions))
    result.addCondAlgo(theADC2MeVCondAlg,primary=True)
    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    print ('--- data')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = LArADC2MeVCondAlgCfg (flags1)
    acc1.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc1.getService('IOVDbSvc').Folders)
    acc1.wasMerged()

    print ('--- mc')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = defaultTestFiles.ESD
    flags2.lock()
    acc2 = LArADC2MeVCondAlgCfg (flags2)
    acc2.printCondAlgs(summariseProps=True)
    print ('IOVDbSvc:', acc2.getService('IOVDbSvc').Folders)
    acc2.wasMerged()
