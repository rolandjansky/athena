"""Configuration for POOL file writing

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc


def PoolWriteCfg(configFlags, **kwargs):
    """Return ComponentAccumulator configured to Write POOL files"""
    # based on WriteAthenaPool._configureWriteAthenaPool
    acc = ComponentAccumulator()

    PoolAttributes = []
    # Switch off splitting by setting default SplitLevel to 0
    PoolAttributes += ["DEFAULT_SPLITLEVEL ='0'"]
    # Set as default the member-wise streaming, ROOT default
    PoolAttributes += ["STREAM_MEMBER_WISE = '1'"]

    # Increase default BasketSize to 32K, ROOT default (but overwritten by POOL)
    PoolAttributes += ["DEFAULT_BUFFERSIZE = '32000'"]

    # Turn off auto_flush for DataHeader container to avoid basket optimization
    PoolAttributes += ["ContainerName = 'POOLContainer(DataHeader)'; BRANCH_BASKET_SIZE = '256000'"]
    PoolAttributes += ["ContainerName = 'POOLContainerForm(DataHeaderForm)'; BRANCH_BASKET_SIZE = '1024000'"]
    PoolAttributes += ["ContainerName = 'TTree=POOLContainerForm(DataHeaderForm)'; CONTAINER_SPLITLEVEL = '99'"]

    kwargs.setdefault("PoolAttributes", PoolAttributes)

    acc.addService(AthenaPoolCnvSvc(**kwargs))
    return acc
