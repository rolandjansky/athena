# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

def MetaDataSvcCfg(flags, toolNames=[], tools=[]):
    # should cover most use cases (POOL, RAW) when called with appropriate toolNames argument
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from six import string_types

    result = ComponentAccumulator()

    result.addService( CompFactory.StoreGateSvc("MetaDataStore") )
    result.addService( CompFactory.StoreGateSvc("InputMetaDataStore") )

    svc = CompFactory.MetaDataSvc("MetaDataSvc")
    svc.MetaDataContainer = "MetaDataHdr"
    result.addService(svc)
    result.addService(CompFactory.ProxyProviderSvc(ProviderNames=["MetaDataSvc"]))

    for tool in tools:
        result.addPublicTool(tool)
        svc.MetaDataTools += [tool]

    for name in toolNames:
        if not isinstance(name, string_types):
            from AthenaCommon.Logging import logging
            log = logging.getLogger("MetaDataSvcConfig")
            log.error('Attempted to pass a non-string argument as a metadata tool name')
            continue
        thistool = CompFactory.getComps(name)[0]()
        result.addPublicTool(thistool)
        svc.MetaDataTools += [thistool]

    return result
