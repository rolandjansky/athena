from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def PoolReadCfg(configFlags):
    """
    Creates a ComponentAccumulator instance containing the 
    athena services required for POOL file reading
    """

    filenames=configFlags.Input.Files

    result=ComponentAccumulator()

    from PoolSvc.PoolSvcConf import PoolSvc
    from SGComps.SGCompsConf import ProxyProviderSvc
    from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import AthenaPoolAddressProviderSvc, EventSelectorAthenaPool
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    
    from StoreGate.StoreGateConf import StoreGateSvc

    result.addService(PoolSvc(MaxFilesOpen=0))
    apcs=AthenaPoolCnvSvc()
    result.addService(apcs)
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[apcs.getFullJobOptName(),])) #No service handle yet???


    result.addService(StoreGateSvc("MetaDataStore"))

    apaps=AthenaPoolAddressProviderSvc()
    result.addService(apaps)
    result.addService(ProxyProviderSvc(ProviderNames=[apaps.getFullJobOptName(),])) #No service handle yet???


    evSel=EventSelectorAthenaPool(InputCollections = filenames)

    result.addService(evSel)
    result.setAppProperty("EvtSel",evSel.getFullJobOptName())

    #(possibly) missing: MetaDataSvc, AddressRemappingSvc

    return result
