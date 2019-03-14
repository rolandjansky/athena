# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArHVScaleCfg(configFlags):
    result=ComponentAccumulator()

    if configFlags.Input.isMC:
        result.merge(addFolders(configFlags,["/LAR/IdentifierOfl/HVLineToElectrodeMap<tag>LARHVLineToElectrodeMap-001</tag>"], "LAR_OFL", className="AthenaAttributeList"))

    elif not configFlags.Common.isOnline:
        result.merge(addFolders(configFlags,["/LAR/DCS/HV/BARREl/I16"], "DCS_OFL", className="CondAttrListCollection"))
        result.merge(addFolders(configFlags,["/LAR/DCS/HV/BARREL/I8"],  "DCS_OFL", className="CondAttrListCollection"))

        result.merge(addFolders(configFlags,["/LAR/IdentifierOfl/HVLineToElectrodeMap"], "LAR_OFL", className="AthenaAttributeList"))
        result.merge(addFolders(configFlags,["/LAR/HVPathologiesOfl/Pathologies"], "LAR_OFL", className="AthenaAttributeList"))

        result.merge(addFolders(configFlags,["/LAR/ElecCalibFlat/HVScaleCorr"],"LAR_ONL", className="CondAttrListCollection"))

        from LArBadChannelTool.LArBadChannelConfig import LArBadChannelCfg, LArBadFebCfg
        result.merge(LArBadChannelCfg(configFlags))
        result.merge(LArBadFebCfg(configFlags))

        from LArRecUtils.LArRecUtilsConf import LArHVIdMappingAlg
        hvmapalg = LArHVIdMappingAlg(ReadKey="/LAR/IdentifierOfl/HVLineToElectrodeMap",WriteKey="LArHVIdMap")
        result.addCondAlgo(hvmapalg)

        from LArRecUtils.LArRecUtilsConf import LArHVPathologyDbCondAlg
        hvpath = LArHVPathologyDbCondAlg(PathologyFolder="/LAR/HVPathologiesOfl/Pathologies",
                                         HVMappingKey="LArHVIdMap",
                                         HVPAthologyKey="LArHVPathology")
        result.addCondAlgo(hvpath)

        from LArRecUtils.LArRecUtilsConf import LArHVCondAlg
        hvcond = LArHVCondAlg("LArHVPathologyAlg",HVPathologies="LArHVPathology",OutputHVData="LArHVData")
        result.addCondAlgo(hvcond)

        from LArRecUtils.LArRecUtilsConf import LArFlatConditionsAlg_LArHVScaleCorrFlat_ as LArHVScaleCorrFlat
        hvscaleflat = LArHVScaleCorrFlat("LArHVScaleCorrFlat",
                                         ReadKey="/LAR/ElecCalibFlat/HVScaleCorr",
                                         WriteKey='LArHVScaleCorr')
        result.addCondAlgo(hvscaleflat)

        from LArRecUtils.LArRecUtilsConf import LArHVScaleCorrCondAlg
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

    from AthenaConfiguration.MainServicesConfig import MainServicesSerialCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg=MainServicesSerialCfg()
    cfg.merge(PoolReadCfg(ConfigFlags))

    cfg.merge( LArHVScaleCfg(ConfigFlags) )

    cfg.run(10)
