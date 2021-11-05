# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
        Instantiate things needed upstream the main egamma Reconstruction,
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaUpstreamCfg(flags, name="EGammaUpstream"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma Upstream configuration')

    acc = ComponentAccumulator()

    # egamma selected/copied topo clusters
    if flags.Detector.EnableCalo:
        from egammaAlgs.egammaTopoClusterCopierConfig import (
            egammaTopoClusterCopierCfg)
        acc.merge(egammaTopoClusterCopierCfg(flags))

    # This is not a direct dependency
    if not flags.Input.isMC:
        from LumiBlockComps.LumiBlockMuWriterConfig import LumiBlockMuWriterCfg
        acc.merge(LumiBlockMuWriterCfg(flags))

    mlog.info("EGamma upstream configured")

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO
    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge(egammaUpstreamCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammareconstructionconfig.pkl", "wb") as f:
        acc.store(f)
