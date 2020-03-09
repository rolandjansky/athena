# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloCellContainerAliasAlgConfig.py
# Created: Nov 2019, sss
# Purpose: Configure CaloCellContainerAliasAlg.

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def CaloCellContainerAliasAlgCfg (flags, alias, cells = 'AllCalo'):
    result = ComponentAccumulator()

    from CaloRec.CaloRecConf import CaloCellContainerAliasAlg
    alg = CaloCellContainerAliasAlg ('CaloCellContainerAliasAlg_' + alias,
                                     Alias = alias,
                                     Cells = cells)
    result.addEventAlgo (alg)

    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    only = ['CaloCellContainerAliasAlg_myalias',
            ]
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = CaloCellContainerAliasAlgCfg (flags1, 'myalias')
    acc1.printConfig(summariseProps=True, onlyComponents=only)
    acc1.wasMerged()
