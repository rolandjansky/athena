# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the EGamma reconstruction.
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaReconstructionCfg(flags, name="egammaReconstruction"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma reconstruction configuration')

    acc = ComponentAccumulator()

    # Add e/gamma tracking algorithms
    if flags.Egamma.doTracking:
        from egammaAlgs.egammaSelectedTrackCopyConfig import (
            egammaSelectedTrackCopyCfg)
        acc.merge(egammaSelectedTrackCopyCfg(flags))

        from egammaAlgs.EMBremCollectionBuilderConfig import (
            EMBremCollectionBuilderCfg)
        acc.merge(EMBremCollectionBuilderCfg(flags))

    # Add e/gamma conversion finding
    if flags.Egamma.doConversionBuilding:
        from egammaAlgs.EMVertexBuilderConfig import (
            EMVertexBuilderCfg)
        acc.merge(EMVertexBuilderCfg(flags))

    # Add algorithms to produce
    # xAOD Electrons and Photons
    if flags.Egamma.doCentral:
        from egammaAlgs.egammaRecBuilderConfig import (
            egammaRecBuilderCfg)
        acc.merge(egammaRecBuilderCfg(flags))

        from egammaAlgs.egammaSuperClusterBuilderConfig import (
            electronSuperClusterBuilderCfg, photonSuperClusterBuilderCfg)
        acc.merge(electronSuperClusterBuilderCfg(flags))
        acc.merge(photonSuperClusterBuilderCfg(flags))

        from egammaAlgs.topoEgammaBuilderConfig import (
            topoEgammaBuilderCfg)
        acc.merge(topoEgammaBuilderCfg(flags))

        from egammaAlgs.egammaLargeClusterMakerAlgConfig import (
            egammaLargeClusterMakerAlgCfg)
        acc.merge(egammaLargeClusterMakerAlgCfg(flags))

    # Add calo seeded forward algorithms to produce
    # xAOD Forward Electrons
    if flags.Egamma.doForward:
        from egammaAlgs.egammaForwardBuilderConfig import (
            egammaForwardBuilderCfg)
        acc.merge(egammaForwardBuilderCfg(flags))

        from egammaAlgs.egammaLargeFWDClusterMakerAlgConfig import (
            egammaLargeFWDClusterMakerAlgCfg)
        acc.merge(egammaLargeFWDClusterMakerAlgCfg(flags))

    # Add truth association
    if flags.Egamma.doTruthAssociation:
        from egammaAlgs.egammaTruthAssociationConfig import (
            egammaTruthAssociationCfg)
        acc.merge(egammaTruthAssociationCfg(flags))

    # To use egamma CA within standard config
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
        for el in acc._allSequences:
            el.name = "TopAlg"

    mlog.info("EGamma reconstruction configured")

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
    acc.merge(egammaReconstructionCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammareconstructionconfig.pkl", "wb") as f:
        acc.store(f)
