# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the full Egamma xAOD Thinning
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaxAODThinningCfg(flags, name="EGammaxAODThinning"):
    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma xAOD Thinning configuration')

    acc = ComponentAccumulator()
    # Add e/gamma track thinning
    # (although we call the Alg slimming)
    if flags.Egamma.doTrackThinning:
        from egammaAlgs.egammaTrackSlimmerConfig import (
            egammaTrackSlimmerCfg)
        acc.merge(egammaTrackSlimmerCfg(flags))

    # Need to add the Cell Thinning for
    # egamma and egammaLargeClusters

    mlog.info("EGamma xAOD Thinning configured")
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.ESD
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(egammaxAODThinningCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammaxaodthinningconfig.pkl", "wb") as f:
        acc.store(f)
