# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the Negative Calo Cluster Thinning
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def ThinNegativeEnergyCaloClustersCfg(flags,
                                      name="ThinNegativeEnergyCaloClustersAlg",
                                      **kwargs):

    acc = ComponentAccumulator()
    kwargs.setdefault("StreamName", "StreamAOD")
    kwargs.setdefault("ThinNegativeEnergyCaloClusters", True)
    kwargs.setdefault("CaloClustersKey", "CaloCalTopoClusters")
    acc.addEventAlgo(
        CompFactory.ThinNegativeEnergyCaloClustersAlg(name, **kwargs)
    )
    return acc


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(ThinNegativeEnergyCaloClustersCfg(flags))
    mlog = logging.getLogger("ThinNegativeEnergyCaloClustersConfigTest")
    printProperties(
        mlog,
        acc.getEventAlgo("ThinNegativeEnergyCaloClustersAlg"),
        nestLevel=1,
        printDefaults=True,
    )

    with open("thinnegativeenergycaloclusterscfg.pkl", "wb") as f:
        acc.store(f)
