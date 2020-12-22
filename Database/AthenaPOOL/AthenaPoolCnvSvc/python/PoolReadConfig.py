# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg

def EventSelectorAthenaPoolCfg(configFlags):
    result=ComponentAccumulator()
    EventSelectorAthenaPool=CompFactory.EventSelectorAthenaPool
    evSel=EventSelectorAthenaPool("EventSelector",
                                  InputCollections = configFlags.Input.Files,
                                  SkipEvents=configFlags.Exec.SkipEvents)
    if configFlags.Input.OverrideRunNumber:
        if not configFlags.Input.RunAndLumiOverrideList:
            DataRunNumber = -1
            FirstLB = 1
            InitialTimeStamp = 1
            OldRunNumber = -1
            if configFlags.Digitization.DataRunNumber>0:
                # Behaviour for Digitization jobs using DataRunNumber
                DataRunNumber = configFlags.Digitization.DataRunNumber
                FirstLB = 1
                InitialTimeStamp = configFlags.IOVDb.RunToTimestampDict.get(DataRunNumber, 1) # TODO fix repeated configuration
                if not configFlags.Sim.DoFullChain:
                    OldRunNumber = configFlags.Input.RunNumber[0] # CHECK this should be the Run Number from the HITS file
            elif configFlags.Input.RunNumber:
                # Behaviour for Simulation jobs
                DataRunNumber = configFlags.Input.RunNumber[0]
                FirstLB = configFlags.Input.LumiBlockNumber[0]
                InitialTimeStamp = configFlags.Input.TimeStamp[0]
            assert DataRunNumber >= 0, (
                "configFlags.Input.OverrideRunNumber was True, but provided DataRunNumber (%d) is negative. "
                "Use a real run number from data." % DataRunNumber)
            evSel.OverrideRunNumber = configFlags.Input.OverrideRunNumber
            evSel.RunNumber = DataRunNumber
            evSel.FirstLB = FirstLB
            evSel.InitialTimeStamp = InitialTimeStamp # Necessary to avoid a crash
            if hasattr(evSel, "OverrideRunNumberFromInput"):
                evSel.OverrideRunNumberFromInput = configFlags.Input.OverrideRunNumber
            if OldRunNumber > 0:
                evSel.OldRunNumber = OldRunNumber
        else:
            # Behaviour for Digitization jobs using RunAndLumiOverrideList
            pass
        result.merge(EvtIdModifierSvcCfg(configFlags))
    result.addService(evSel)
    return result


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
        result.addService(evSel)
    else:
        # We have only primary inputs
        apaps=AthenaPoolAddressProviderSvc()
        result.addService(apaps)
        result.addService(ProxyProviderSvc(ProviderNames=[apaps.getFullJobOptName(),])) #No service handle yet???
        result.merge(EventSelectorAthenaPoolCfg(configFlags))
        evSel = result.getService("EventSelector")

    result.setAppProperty("EvtSel",evSel.getFullJobOptName())

    #(possibly) missing: MetaDataSvc

    return result
