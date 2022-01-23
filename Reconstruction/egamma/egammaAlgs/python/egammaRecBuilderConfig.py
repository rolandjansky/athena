# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate egammaRecBuilder with default configuration
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTools.EMTrackMatchBuilderConfig import EMTrackMatchBuilderCfg
from egammaTools.EMConversionBuilderConfig import EMConversionBuilderCfg


def egammaRecBuilderCfg(
        flags,
        name='egammaRecBuilder',
        sequenceName = None,
        **kwargs):

    seqkw = {'sequence': sequenceName} if sequenceName else {}
    acc = ComponentAccumulator (**seqkw)

    if "TrackMatchBuilderTool" not in kwargs:
        emtrkmatch = EMTrackMatchBuilderCfg(flags)
        kwargs["TrackMatchBuilderTool"] = acc.popToolsAndMerge(emtrkmatch)

    if "ConversionBuilderTool" not in kwargs:
        emcnv = EMConversionBuilderCfg(flags)
        kwargs["ConversionBuilderTool"] = acc.popToolsAndMerge(emcnv)

    kwargs.setdefault(
        "egammaRecContainer",
        flags.Egamma.Keys.Internal.EgammaRecs)
    kwargs.setdefault(
        "InputClusterContainerName",
        flags.Egamma.Keys.Internal.EgammaTopoClusters)

    egrecAlg = CompFactory.egammaRecBuilder(name, **kwargs)

    acc.addEventAlgo(egrecAlg)
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
    acc.merge(egammaRecBuilderCfg(flags))
    mlog = logging.getLogger("egammaRecBuilderConfigTest")
    mlog.info("Configuring  egammaRecBuilder: ")
    printProperties(mlog,
                    acc.getEventAlgo("egammaRecBuilder"),
                    nestLevel=1,
                    printDefaults=True)
    with open("egammarecbuilder.pkl", "wb") as f:
        acc.store(f)
