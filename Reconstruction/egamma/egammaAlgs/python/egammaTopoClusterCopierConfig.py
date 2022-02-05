# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate egammaTopoClusterCopier with default configuration"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaTopoClusterCopierCfg(
        flags,
        name='egammaTopoClusterCopier',
        **kwargs):

    acc = ComponentAccumulator()
    kwargs.setdefault(
        "InputTopoCollection",
        flags.Egamma.Keys.Input.TopoClusters)

    egtopocluster = flags.Egamma.Keys.Internal.EgammaTopoClusters
    kwargs.setdefault(
        "OutputTopoCollection",
        egtopocluster)
    kwargs.setdefault(
        "OutputTopoCollectionShallow",
        "tmp_"+egtopocluster)

    egcopierAlg = CompFactory.egammaTopoClusterCopier(name, **kwargs)

    acc.addEventAlgo(egcopierAlg)

    # To use within standard config
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
        for el in acc._allSequences:
            el.name = "TopAlg"

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.lock()
    acc = MainServicesCfg(flags)
    mlog = logging.getLogger("egammaTopoClusterCopierConfigTest")
    mlog.info("Configuring  egammaTopoClusterCopier: ")
    acc.merge(egammaTopoClusterCopierCfg(flags))
    printProperties(mlog,
                    acc.getEventAlgo("egammaTopoClusterCopier"),
                    nestLevel=1,
                    printDefaults=True)
    with open("egammatopoclustercopier.pkl", "wb") as f:
        acc.store(f)
