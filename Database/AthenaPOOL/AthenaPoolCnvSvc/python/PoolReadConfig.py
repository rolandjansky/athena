
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def PoolReadCfg(inputFlags):
    """
    creates A ComponentAccumulator instance containing the 
    athena services required for POOL file reading
    """

    filenames=inputFlags.get("AthenaConfiguration.GlobalFlags.InputFiles")

    result=ComponentAccumulator()

    from PoolSvc.PoolSvcConf import PoolSvc
    from SGComps.SGCompsConf import ProxyProviderSvc
    from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import AthenaPoolAddressProviderSvc, EventSelectorAthenaPool
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    


    result.addService(PoolSvc(MaxFilesOpen=0))
    apcs=AthenaPoolCnvSvc()
    result.addService(apcs)
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[apcs.getFullJobOptName(),])) #No service handle yet???

    apaps=AthenaPoolAddressProviderSvc()
    result.addService(apaps)
    result.addService(ProxyProviderSvc(ProviderNames=[apaps.getFullJobOptName(),])) #No service handle yet???


    evSel=EventSelectorAthenaPool(InputCollections = filenames)

    result.addService(evSel)
    result.setAppProperty("EvtSel",evSel.getFullJobOptName())

    #(possibly) missing: MetaDataSvc, AddressRemappingSvc

    return result
