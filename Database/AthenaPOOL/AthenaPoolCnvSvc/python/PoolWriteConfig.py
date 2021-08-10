"""Configuration for POOL file writing

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc


def PoolWriteCfg(flags):
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

    # Set POOLContainerForm(DataHeaderForm) split level to 0
    PoolAttributes += ["ContainerName = 'TTree=POOLContainerForm(DataHeaderForm)'; CONTAINER_SPLITLEVEL = '0'"]
    PoolAttributes += ["TREE_BRANCH_OFFSETTAB_LEN ='100'"]

    # Kept in sync with RecoUtils.py
    from AthenaPoolCnvSvc import PoolAttributeHelper as pah

    if flags.Output.EVNT_TRFileName:
        # Use LZMA w/ Level 1
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.EVNT_TRFileName, 2 ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.EVNT_TRFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 1 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.EVNT_TRFileName, "CollectionTree", 1 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.EVNT_TRFileName, "POOLContainer", 1 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.EVNT_TRFileName, "POOLContainerForm", 1 ) ]

    if flags.Output.HITSFileName:
        # Use LZMA w/ Level 1
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.HITSFileName, 2 ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.HITSFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 1 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.HITSFileName, "CollectionTree", 1 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.HITSFileName, "POOLContainer", 1 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.HITSFileName, "POOLContainerForm", 1 ) ]

    if flags.Output.RDOFileName:
        # Use LZMA w/ Level 1
        comp_alg = 1 if '_000' in flags.Output.RDOFileName or 'tmp.' in flags.Output.RDOFileName else 2
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.RDOFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.RDOFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 10 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.RDOFileName, "CollectionTree", 10 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.RDOFileName, "POOLContainer", 10 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.RDOFileName, "POOLContainerForm", 10 ) ]

    if flags.Output.ESDFileName:
        # Use LZMA w/ Level 1
        comp_alg = 1 if '_000' in flags.Output.ESDFileName or 'tmp.' in flags.Output.ESDFileName else 2
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.ESDFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.ESDFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 10 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.ESDFileName, "CollectionTree", 10 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.ESDFileName, "POOLContainer", 10 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.ESDFileName, "POOLContainerForm", 10 ) ]

    if flags.Output.AODFileName:
        # Use LZMA w/ Level 1
        comp_alg = 1 if '_000' in flags.Output.AODFileName or 'tmp.' in flags.Output.AODFileName else 2
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.AODFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.AODFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 100 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.AODFileName, "CollectionTree", 100 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.AODFileName, "POOLContainer", 100 ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.AODFileName, "POOLContainerForm", 100 ) ]

    acc.addService(AthenaPoolCnvSvc(PoolAttributes = PoolAttributes))
    acc.addService(CompFactory.EvtPersistencySvc(CnvServices=["AthenaPoolCnvSvc"]))
    return acc
