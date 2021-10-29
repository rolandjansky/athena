# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the EGamma Reconstruction, Output Item Lists,
          and xAOD related Thinning
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def EGammaSteeringCfg(flags, name="EGammaSteering"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma steering')

    acc = ComponentAccumulator()

    # Reconstruction
    from egammaConfig.egammaReconstructionConfig import (
        egammaReconstructionCfg)
    acc.merge(egammaReconstructionCfg(flags))

    if flags.Output.doWriteESD or flags.Output.doWriteAOD:
        # Add e/gamma related containers to the output stream
        # we internally check if we need to fill one
        # or both ESD/AOD
        from egammaConfig.egammaOutputConfig import (
            egammaOutputCfg)
        acc.merge(egammaOutputCfg(flags))

    if flags.Output.doWriteAOD:
        from egammaConfig.egammaxAODThinningConfig import (
            egammaxAODThinningCfg)
        acc.merge(egammaxAODThinningCfg(flags))

    mlog.info("EGamma steering done")
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO
    flags.Output.doWriteESD = True  # To test the ESD parts
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(EGammaSteeringCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammasteeringconfig.pkl", "wb") as f:
        acc.store(f)
