# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloThinCellsByClusterAlgConfig.py
# Created: Nov 2019, sss
# Purpose: Configure CaloThinCellsByClusterAlg.

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def CaloThinCellsByClusterAlgCfg(flags, streamName, clusters,
                                 samplings=[],
                                 cells='AllCalo'):
    result = ComponentAccumulator()

    CaloThinCellsByClusterAlg = CompFactory.CaloThinCellsByClusterAlg
    alg = CaloThinCellsByClusterAlg('CaloThinCellsByClusterAlg_' + clusters,
                                    StreamName=streamName,
                                    Clusters=clusters,
                                    SamplingCellsName=samplings,
                                    Cells=cells)
    result.addEventAlgo(alg)
    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    only = ['CaloThinCellsByClusterAlg_myclusters',
            ]
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = CaloThinCellsByClusterAlgCfg(flags1, 'StreamAOD',
                                        'myclusters', ['TileGap3'])
    acc1.printConfig(summariseProps=True, onlyComponents=only)
    acc1.wasMerged()
