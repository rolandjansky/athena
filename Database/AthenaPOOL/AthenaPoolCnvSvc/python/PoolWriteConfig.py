"""Configuration for POOL file writing

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc


def PoolWriteCfg(flags, forceTreeAutoFlush=-1):
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
        comp_alg = 1 if flags.Output.EVNT_TRFileName.endswith('_000') or flags.Output.EVNT_TRFileName.startswith('tmp.') else 2
        auto_flush = forceTreeAutoFlush if forceTreeAutoFlush != -1 else 1
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.EVNT_TRFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.EVNT_TRFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 1 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.EVNT_TRFileName, "CollectionTree", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.EVNT_TRFileName, "POOLContainer", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.EVNT_TRFileName, "POOLContainerForm", auto_flush ) ]

    if flags.Output.HITSFileName:
        # Use LZMA w/ Level 1
        comp_alg = 1 if flags.Output.HITSFileName.endswith('_000') or flags.Output.HITSFileName.startswith('tmp.') else 2
        auto_flush = forceTreeAutoFlush if forceTreeAutoFlush != -1 else 10
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.HITSFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.HITSFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 1 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.HITSFileName, "CollectionTree", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.HITSFileName, "POOLContainer", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.HITSFileName, "POOLContainerForm", auto_flush ) ]

    if flags.Output.RDOFileName:
        # Use LZMA w/ Level 1
        comp_alg = 1 if flags.Output.RDOFileName.endswith('_000') or flags.Output.RDOFileName.startswith('tmp.') else 2
        auto_flush = forceTreeAutoFlush if forceTreeAutoFlush != -1 else 10
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.RDOFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.RDOFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 10 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.RDOFileName, "CollectionTree", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.RDOFileName, "POOLContainer", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.RDOFileName, "POOLContainerForm", auto_flush ) ]

    if flags.Output.ESDFileName:
        # Use LZMA w/ Level 1
        comp_alg = 1 if flags.Output.ESDFileName.endswith('_000') or flags.Output.ESDFileName.startswith('tmp.') else 2
        auto_flush = forceTreeAutoFlush if forceTreeAutoFlush != -1 else 10
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.ESDFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.ESDFileName, 1 ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 10 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.ESDFileName, "CollectionTree", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.ESDFileName, "POOLContainer", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.ESDFileName, "POOLContainerForm", auto_flush ) ]

    if flags.Output.AODFileName:
        # Use LZMA w/ Level 1
        comp_alg = 1 if flags.Output.AODFileName.endswith('_000') or flags.Output.AODFileName.startswith('tmp.') else 2
        auto_flush = forceTreeAutoFlush if forceTreeAutoFlush != -1 else 100
        PoolAttributes += [ pah.setFileCompAlg( flags.Output.AODFileName, comp_alg ) ]
        PoolAttributes += [ pah.setFileCompLvl( flags.Output.AODFileName, 1 ) ]
        # By default use a maximum basket buffer size of 128k and minimum buffer entries of 10
        PoolAttributes += [ pah.setMaxBufferSize( flags.Output.AODFileName, "131072" ) ]
        PoolAttributes += [ pah.setMinBufferEntries( flags.Output.AODFileName, "10" ) ]
        # Flush the CollectionTree, POOLContainer, and POOLContainerForm to disk at every 100 events
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.AODFileName, "CollectionTree", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.AODFileName, "POOLContainer", auto_flush ) ]
        PoolAttributes += [ pah.setTreeAutoFlush( flags.Output.AODFileName, "POOLContainerForm", auto_flush ) ]

    # Derivation framework output settings    
    for flag in [key for key in flags._flagdict.keys() if ("Output.DAOD_" in key and "FileName" in key)]:
        # Since there may be several outputs, this has to be done in a loop 
        FileName = flags._flagdict[flag]._value 
        # Use ZSTD w/ Level 5 for DAODs
        PoolAttributes += [ pah.setFileCompAlg( FileName, "5" ) ]
        PoolAttributes += [ pah.setFileCompLvl( FileName, "5" ) ]
        # By default use a maximum basket buffer size of 128k and minimum buffer entries of 10
        PoolAttributes += [ pah.setMaxBufferSize( FileName, "131072" ) ]
        PoolAttributes += [ pah.setMinBufferEntries( FileName, "10" ) ]
        # By default use 20 MB AutoFlush (or 500 events for SharedWriter w/ parallel compession)
        # for event data except for a number of select formats (see below)
        TREE_AUTO_FLUSH = -20000000
        if flags.MP.UseSharedWriter and flags.MP.UseParallelCompression:
            TREE_AUTO_FLUSH = 500
        # By default use split-level 0 except for DAOD_PHYSLITE which is maximally split
        CONTAINER_SPLITLEVEL = 0
        if "DAOD_PHYSVAL" in FileName:
            TREE_AUTO_FLUSH = 100
        if "DAOD_PHYS" in FileName:
            TREE_AUTO_FLUSH = 500
        if "DAOD_PHYSLITE" in FileName:
            TREE_AUTO_FLUSH = 1000
            CONTAINER_SPLITLEVEL = 99
        PoolAttributes += [ pah.setTreeAutoFlush( FileName, "CollectionTree", str(TREE_AUTO_FLUSH) ) ]
        PoolAttributes += [ pah.setContainerSplitLevel( FileName, "CollectionTree", str(CONTAINER_SPLITLEVEL) ) ]
        PoolAttributes += [ pah.setContainerSplitLevel( FileName, "Aux.", str(CONTAINER_SPLITLEVEL) ) ]

    acc.addService(AthenaPoolCnvSvc(PoolAttributes = PoolAttributes))
    acc.addService(CompFactory.EvtPersistencySvc("EventPersistencySvc",CnvServices=["AthenaPoolCnvSvc"]))
    return acc
