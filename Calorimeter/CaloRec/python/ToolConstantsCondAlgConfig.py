# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/ToolConstantsCondAlgConfig.py
# Created: Jun 2020, sss
# Purpose: Configure ToolConstantsCondAlg.

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def ToolConstantsCondAlgCfg (flags, key, DetStoreKey='', COOLFolder=''):
    """Configure a conditions algorithm to convert inline COOL data or detector store data to ToolConstants.
    KEY is also the key of the output conditions object.
    For reading from COOL inline data, COOLFOLDER gives the name
    of the COOL folder; the dta are given by KEY within the folder.
    The caller should register the folder with IOVDbSvc.
    For copying a ToolConstants object from the detector store,
    set DETSTOREKEY to the key of the object to copy."""

    if ((DetStoreKey == '' and COOLFolder == '') or
        (DetStoreKey != '' and COOLFolder != '')):
        raise RuntimeError ("ToolConstantsCondAlgCfg: For key " + key + ", exactly one of DetStoreKey or COOLFolder must be specified")

    result = ComponentAccumulator()

    name = 'ToolConstantsCondAlg_' + key

    ToolConstantsCondAlg = CompFactory.ToolConstantsCondAlg # CaloRec
    alg = ToolConstantsCondAlg (name,
                                COOLFolderKey = COOLFolder,
                                ToolConstantsKey = key,
                                DetStoreKey = DetStoreKey)
    result.addCondAlgo (alg)

    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.loadAllDynamicFlags()

    flags = ConfigFlags.clone()
    flags.lock()

    print ('--- detstore')
    acc1 = ToolConstantsCondAlgCfg (flags, 'key1', DetStoreKey='ds1')
    acc1.printConfig(summariseProps=True)
    acc1.wasMerged()

    print ('--- cool')
    acc2 = ToolConstantsCondAlgCfg (flags, 'key2', COOLFolder='folder2')
    acc2.printConfig(summariseProps=True)
    acc2.wasMerged()

    print ('--- error')
    try:
        acc3 = ToolConstantsCondAlgCfg (flags, 'key3', COOLFolder='folder3', DetStoreKey='ds3')
    except RuntimeError:
        pass
    else:
        assert 0, 'not caught'
