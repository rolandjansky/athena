from AthenaConfiguration.ComponentFactory import CompFactory

if __name__ == "__main__":

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ["myRDO.pool.root",]
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

    cfg=MainServicesCfg(ConfigFlags)
    cfg.merge(PoolReadCfg(ConfigFlags))

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    cfg.merge(LArGMCfg(ConfigFlags))
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    cfg.merge(LArOnOffIdMappingCfg(ConfigFlags))
    cfg.addEventAlgo(CompFactory.DumpLArRawChannels(NtupStream="LARRC",OutputFileName="",ToLog=False))

    cfg.addService(CompFactory.THistSvc(Output = ["LARRC DATAFILE='LARRC.root', OPT='RECREATE'"]))

                   
    cfg.run(-1)

                    
