# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloThinCellsBySamplingAlgConfig.py
# Created: Nov 2019, sss
# Purpose: Configure CaloThinCellsBySamplingAlg.

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def CaloThinCellsBySamplingAlgCfg (flags, streamName,
                                  samplings = [],
                                  cells = 'AllCalo'):
    result = ComponentAccumulator()
    
    CaloThinCellsBySamplingAlg=CompFactory.CaloThinCellsBySamplingAlg
    alg = CaloThinCellsBySamplingAlg ('CaloThinCellsBySamplingAlg_' + '_'.join (samplings),
                                     StreamName = streamName,
                                     SamplingCellsName = samplings,
                                     Cells = cells)
    result.addEventAlgo (alg)

    if "AOD" in streamName:
        #Add cell-container to AOD:
        from OutputStreamAthenaPool.OutputStreamConfig import addToAOD
        toAOD=[f'CaloCellContainer#{cells}',]
        result.merge(addToAOD(flags, toAOD))

    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    only = ['CaloThinCellsBySamplingAlg_TileGap3',
            ]
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = CaloThinCellsBySamplingAlgCfg (flags1, 'StreamAOD',
                                          ['TileGap3'])
    acc1.printConfig(summariseProps=True, onlyComponents=only)
    acc1.wasMerged()
