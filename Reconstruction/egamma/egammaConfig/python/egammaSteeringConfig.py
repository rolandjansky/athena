# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the
          Things needed upstream the main egamma Reconstruction,
          EGamma Reconstruction,
          Output Item Lists,
          xAOD related Thinning
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def EGammaSteeringCfg(flags,
                      name="EGammaSteering"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma steering')

    acc = ComponentAccumulator()

    # Things upstream the main egamma reconstruction
    from egammaConfig.egammaUpstreamConfig import (
        egammaUpstreamCfg)
    acc.merge(egammaUpstreamCfg(flags))

    # e/gamma main Reconstruction
    from egammaConfig.egammaReconstructionConfig import (
        egammaReconstructionCfg)
    acc.merge(egammaReconstructionCfg(flags))

    if flags.Output.doWriteESD or flags.Output.doWriteAOD:
        # Add e/gamma related containers to the output stream
        from egammaConfig.egammaOutputConfig import (
            egammaOutputCfg)
        acc.merge(egammaOutputCfg(flags))

    if flags.Output.doWriteAOD:
        # Add e/gamma xAOD thinning
        from egammaConfig.egammaxAODThinningConfig import (
            egammaxAODThinningCfg)
        acc.merge(egammaxAODThinningCfg(flags))

    if flags.Detector.GeometryID and flags.InDet.doR3LargeD0:
        # LRT Reconstruction
        from egammaConfig.egammaLRTReconstructionConfig import (
            egammaLRTReconstructionCfg)
        acc.merge(egammaLRTReconstructionCfg(flags))

        # LRT output
        if flags.Output.doWriteESD or flags.Output.doWriteAOD:
            from egammaConfig.egammaLRTOutputConfig import (
                egammaLRTOutputCfg)
            acc.merge(egammaLRTOutputCfg(flags))

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
