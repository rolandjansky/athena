# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloThinCellsBySamplingAlgConfig.py
# Created: Nov 2019, sss
# Purpose: Configure CaloThinCellsBySamplingAlg.

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def CaloThinCellsBySamplingAlgCfg (flags, streamName,
                                  samplings = [],
                                  cells = 'AllCalo'):
    result = ComponentAccumulator()
    
    from CaloRec.CaloRecConf import CaloThinCellsBySamplingAlg
    alg = CaloThinCellsBySamplingAlg ('CaloThinCellsBySamplingAlg_' + '_'.join (samplings),
                                     StreamName = streamName,
                                     SamplingCellsName = samplings,
                                     Cells = cells)
    result.addEventAlgo (alg)

    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    only = ['CaloThinCellsBySamplingAlg_TileGap3',
            ]
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = CaloThinCellsBySamplingAlgCfg (flags1, 'StreamAOD',
                                          ['TileGap3'])
    acc1.printConfig(summariseProps=True, onlyComponents=only)
    acc1.wasMerged()
