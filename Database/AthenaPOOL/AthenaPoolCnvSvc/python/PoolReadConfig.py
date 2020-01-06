# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PoolReadCfg(configFlags):
    """
    Creates a ComponentAccumulator instance containing the 
    athena services required for POOL file reading
    """

    filenames=configFlags.Input.Files
    filenamesSecondary=configFlags.Input.SecondaryFiles

    result=ComponentAccumulator()

    PoolSvc=CompFactory.PoolSvc
    ProxyProviderSvc=CompFactory.ProxyProviderSvc
    AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc
    AthenaPoolAddressProviderSvc, EventSelectorAthenaPool, DoubleEventSelectorAthenaPool=CompFactory.getComps("AthenaPoolAddressProviderSvc","EventSelectorAthenaPool","DoubleEventSelectorAthenaPool",)
    EvtPersistencySvc=CompFactory.EvtPersistencySvc
    
    StoreGateSvc=CompFactory.StoreGateSvc

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

        evSel=DoubleEventSelectorAthenaPool("DoubleEventSelector",
                                            PrimaryInputCollections = filenames,
                                            SecondaryaryInputCollections = filenamesSecondary)
    else:
        # We have only primary inputs
        apaps=AthenaPoolAddressProviderSvc()
        result.addService(apaps)
        result.addService(ProxyProviderSvc(ProviderNames=[apaps.getFullJobOptName(),])) #No service handle yet???

        evSel=EventSelectorAthenaPool("EventSelector", InputCollections = filenames)

    result.addService(evSel)
    result.setAppProperty("EvtSel",evSel.getFullJobOptName())

    #(possibly) missing: MetaDataSvc

    return result
