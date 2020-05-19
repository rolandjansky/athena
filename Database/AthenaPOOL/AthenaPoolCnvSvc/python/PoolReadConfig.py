# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

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
    apcs.InputPoolAttributes += ["DatabaseName = '*'; ContainerName = 'CollectionTree'; TREE_CACHE = '-1'"]
    result.addService(apcs)
    result.addService(EvtPersistencySvc("EventPersistencySvc",CnvServices=[apcs.getFullJobOptName(),])) #No service handle yet???


    result.addService(StoreGateSvc("MetaDataStore"))

    if filenamesSecondary:
        # Create DoubleEventSelector (universal for any seconday input type)
        evSel = DoubleEventSelectorAthenaPool("DoubleEventSelector",
                                              InputCollections=filenames,
                                              SkipEvents=configFlags.Exec.SkipEvents)


        if configFlags.Overlay.DataOverlay:
            # We have to check if we're running data overlay - BS is needed in this case
            from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
            result.merge(ByteStreamReadCfg(configFlags))

            # We still have to add primary address provider
            apapsPrimary = AthenaPoolAddressProviderSvc("AthenaPoolAddressProviderSvcPrimary")
            apapsPrimary.DataHeaderKey = "EventSelector"
            result.addService(apapsPrimary)

            result.addService(ProxyProviderSvc(ProviderNames = [
                apapsPrimary.getFullJobOptName(),
            ])) #No service handle yet???
        else:
            # We have primary and secondary pool inputs, create two address providers
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

            secondarySel = EventSelectorAthenaPool("SecondaryEventSelector",
                                                   IsSecondary=True,
                                                   InputCollections=filenamesSecondary)
            result.addService(secondarySel)
    else:
        # We have only primary inputs
        apaps=AthenaPoolAddressProviderSvc()
        result.addService(apaps)
        result.addService(ProxyProviderSvc(ProviderNames=[apaps.getFullJobOptName(),])) #No service handle yet???

        evSel=EventSelectorAthenaPool("EventSelector", 
                                      InputCollections = filenames, 
                                      SkipEvents=configFlags.Exec.SkipEvents)

    result.addService(evSel)
    result.setAppProperty("EvtSel",evSel.getFullJobOptName())

    #(possibly) missing: MetaDataSvc

    return result
