# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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
                      name="EGammaSteering",
                      doAllUpstream=False):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma Steering')

    acc = ComponentAccumulator()

    # upstream the main egamma reconstruction
    from egammaConfig.egammaUpstreamConfig import (
        egammaUpstreamCfg)
    acc.merge(egammaUpstreamCfg(flags,
                                doAll=doAllUpstream))

    # e/gamma main Reconstruction
    from egammaConfig.egammaReconstructionConfig import (
        egammaReconstructionCfg)
    acc.merge(egammaReconstructionCfg(flags))

    # Add e/gamma related containers to the output stream
    if flags.Output.doWriteESD or flags.Output.doWriteAOD:
        from egammaConfig.egammaOutputConfig import (
            egammaOutputCfg)
        acc.merge(egammaOutputCfg(flags))

    # Add e/gamma xAOD thinning
    if flags.Output.doWriteAOD:
        from egammaConfig.egammaxAODThinningConfig import (
            egammaxAODThinningCfg)
        acc.merge(egammaxAODThinningCfg(flags))

    # LRT Reconstruction
    if flags.Detector.GeometryID and flags.InDet.Tracking.doR3LargeD0:
        from egammaConfig.egammaLRTReconstructionConfig import (
            egammaLRTReconstructionCfg)
        acc.merge(egammaLRTReconstructionCfg(flags))

        # LRT output
        if flags.Output.doWriteESD or flags.Output.doWriteAOD:
            from egammaConfig.egammaLRTOutputConfig import (
                egammaLRTOutputCfg)
            acc.merge(egammaLRTOutputCfg(flags))

    mlog.info("EGamma Steering done")
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO_RUN2
    flags.Output.doWriteESD = True  # To test the ESD parts
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()
    acc = MainServicesCfg(flags)
    acc.merge(EGammaSteeringCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammasteeringconfig.pkl", "wb") as f:
        acc.store(f)
