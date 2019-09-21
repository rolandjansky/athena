# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def PoolReadCfg(configFlags):
    """
    Creates a ComponentAccumulator instance containing the 
    athena services required for POOL file reading
    """

    filenames=configFlags.Input.Files
    filenamesSecondary=configFlags.Input.SecondaryFiles

    result=ComponentAccumulator()

    from PoolSvc.PoolSvcConf import PoolSvc
    from SGComps.SGCompsConf import ProxyProviderSvc
    from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import AthenaPoolAddressProviderSvc, EventSelectorAthenaPool, DoubleEventSelectorAthenaPool
    from GaudiSvc.GaudiSvcConf import EvtPersistencySvc
    
    from StoreGate.StoreGateConf import StoreGateSvc

    result.addService(PoolSvc(MaxFilesOpen=0))
    apcs=AthenaPoolCnvSvc()
    result.addService(apcs)
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[apcs.getFullJobOptName(),])) #No service handle yet???


    result.addService(StoreGateSvc("MetaDataStore"))


    if filenamesSecondary:
        # We have primary and secondary inputs, create two address providers
        apapsPrimary = AthenaPoolAddressProviderSvc("AthenaPoolAddressProviderSvcPrimary")
        apapsPrimary.DataHeaderKey = "EventSelector"
        apapsPrimary.AttributeListKey = "Input"
        result.addService(apapsPrimary)
        apapsSecondary = AthenaPoolAddressProviderSvc("AthenaPoolAddressProviderSvcSecondary")
        apapsSecondary.DataHeaderKey = "SecondaryEventSelector"
        result.addService(apapsSecondary)

        result.addService(ProxyProviderSvc(ProviderNames = [
            apapsPrimary.getFullJobOptName(),
            apapsSecondary.getFullJobOptName()
        ])) #No service handle yet???

        evSel=DoubleEventSelectorAthenaPool(PrimaryInputCollections = filenames,
                                            SecondaryaryInputCollections = filenamesSecondary)
    else:
        # We have only primary inputs
        apaps=AthenaPoolAddressProviderSvc()
        result.addService(apaps)
        result.addService(ProxyProviderSvc(ProviderNames=[apaps.getFullJobOptName(),])) #No service handle yet???

        evSel=EventSelectorAthenaPool(InputCollections = filenames)

    result.addService(evSel)
    result.setAppProperty("EvtSel",evSel.getFullJobOptName())

    #(possibly) missing: MetaDataSvc

    return result
