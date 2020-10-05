# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def LArHVScaleCfg(configFlags):
    result=ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    #result.merge(IOVDbSvcCfg(configFlags))

    if configFlags.Input.isMC:
        result.merge(addFolders(configFlags,["/LAR/Identifier/HVLineToElectrodeMap<tag>LARHVLineToElectrodeMap-001</tag>"], "LAR_OFL", className="AthenaAttributeList"))

        LArHVIdMappingAlg=CompFactory.LArHVIdMappingAlg
        hvmapalg = LArHVIdMappingAlg(ReadKey="/LAR/Identifier/HVLineToElectrodeMap",WriteKey="LArHVIdMap")
        result.addCondAlgo(hvmapalg)


    elif not configFlags.Common.isOnline:
        result.merge(addFolders(configFlags,["/LAR/DCS/HV/BARREl/I16"], "DCS_OFL", className="CondAttrListCollection"))
        result.merge(addFolders(configFlags,["/LAR/DCS/HV/BARREL/I8"],  "DCS_OFL", className="CondAttrListCollection"))

        result.merge(addFolders(configFlags,["/LAR/IdentifierOfl/HVLineToElectrodeMap"], "LAR_OFL", className="AthenaAttributeList"))
        result.merge(addFolders(configFlags,["/LAR/HVPathologiesOfl/Pathologies"], "LAR_OFL", className="AthenaAttributeList"))

        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg, LArBadFebCfg
        result.merge(LArBadChannelCfg(configFlags))
        result.merge(LArBadFebCfg(configFlags))

        LArHVIdMappingAlg=CompFactory.LArHVIdMappingAlg
        hvmapalg = LArHVIdMappingAlg(ReadKey="/LAR/IdentifierOfl/HVLineToElectrodeMap",WriteKey="LArHVIdMap")
        result.addCondAlgo(hvmapalg)

        LArHVPathologyDbCondAlg=CompFactory.LArHVPathologyDbCondAlg
        hvpath = LArHVPathologyDbCondAlg(PathologyFolder="/LAR/HVPathologiesOfl/Pathologies",
                                         HVMappingKey="LArHVIdMap",
                                         HVPAthologyKey="LArHVPathology")
        result.addCondAlgo(hvpath)

        LArHVCondAlg=CompFactory.LArHVCondAlg
        hvcond = LArHVCondAlg(HVPathologies="LArHVPathology",OutputHVData="LArHVData")
        result.addCondAlgo(hvcond)

        from LArConfiguration.LArElecCalibDBConfig import LArElecCalibDbCfg
        result.merge(LArElecCalibDbCfg(configFlags,["HVScaleCorr",]))

        LArHVScaleCorrCondAlg=CompFactory.LArHVScaleCorrCondAlg
        hvscalecorrkey = "LArHVScaleCorrRecomputed"
        if configFlags.Input.isMC:
            hvscalecorrkey = "LArHVScaleCorr"

        hvscale = LArHVScaleCorrCondAlg(keyHVdata="LArHVData",keyOutputCorr=hvscalecorrkey)
        hvscale.UndoOnlineHVCorr=True
        result.addCondAlgo(hvscale)

    return result

if __name__=="__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    nThreads=1
    ConfigFlags.Concurrency.NumThreads = nThreads
    if nThreads>0:
        ConfigFlags.Scheduler.ShowDataDeps = True
        ConfigFlags.Scheduler.ShowDataFlow = True
        ConfigFlags.Scheduler.ShowControlFlow = True
        ConfigFlags.Concurrency.NumConcurrentEvents = nThreads

    ConfigFlags.Input.Files = ["myESD-data.pool.root"]
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( LArHVScaleCfg(ConfigFlags) )

    cfg.run(10)
