# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def LArADC2MeVCondAlgCfg(configFlags):
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg 
    from LArRecUtils.LArRecUtilsConf import LArADC2MeVCondAlg 
    from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    
    result=ComponentAccumulator()
    result.merge(LArOnOffIdMappingCfg(configFlags))
    result.merge(LArGMCfg(configFlags)) #Needed for identifier helpers

    theADC2MeVCondAlg=LArADC2MeVCondAlg()

    isMC=configFlags.Input.isMC
    
    if isMC:
        requiredConditons=["Ramp","DAC2uA","uA2MeV"]
        if not configFlags.LAr.hasMphys():
            theADC2MeVCondAlg.LArMphysOverMcalKey="" #No MphysOVerMcal
        else:
            theADC2MeVCondAlg.LArMphysOverMcalKey="LArMphysOverMcalSym"
            requiredConditons+=["MphysOverMcal",]

        if not larCondFlags.hasHVCorr():
            theADC2MeVCondAlg.LArHVScaleCorrKey=""
        else:
            requiredConditons+=["HVScale",]
        theADC2MeVCondAlg.LAruA2MeVKey="LAruA2MeVSym"
        theADC2MeVCondAlg.LArDAC2uAKey="LArDAC2uASym"
        theADC2MeVCondAlg.LArRampKey="LArRampSym"


        theADC2MeVCondAlg.UseFEBGainTresholds=False
    else: # not MC:
        requiredConditons=["Ramp","DAC2uA","uA2MeV","MphysOverMcal","HVScaleCorr"]
        from LArRecUtils.LArFebConfigCondAlgConfig import LArFebConfigCondAlgCfg
        result.merge(LArFebConfigCondAlgCfg(configFlags))

    result.merge(LArElecCalibDbCfg(configFlags,requiredConditons))
    result.addCondAlgo(theADC2MeVCondAlg,primary=True)
    return result


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

    cfg=ComponentAccumulator()
    from ByteStreamCnvSvc.ByteStreamConfig import TrigBSReadCfg
    cfg.merge( TrigBSReadCfg(ConfigFlags) )

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
 

    cfg.merge(LArADC2MeVCondAlgCfg(ConfigFlags))
    f=open("LArADC2MeVCondAlgo.pkl","w")
    cfg.store(f)
    f.close()
