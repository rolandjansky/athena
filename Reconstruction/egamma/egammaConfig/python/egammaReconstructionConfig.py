# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the full EGamma reconstruction.
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaReconstructionCfg(flags, name="EGammaReconstruction"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma reconstruction configuration')

    acc = ComponentAccumulator()

    # egamma selected/copied topo clusters
    if flags.Detector.EnableCalo:
        from egammaAlgs.egammaTopoClusterCopierConfig import (
            egammaTopoClusterCopierCfg)
        acc.merge(egammaTopoClusterCopierCfg(flags))

    # Add e/gamma tracking algorithms
    if flags.Egamma.doTracking:
        from egammaAlgs.egammaSelectedTrackCopyConfig import (
            egammaSelectedTrackCopyCfg)
        acc.merge(egammaSelectedTrackCopyCfg(flags))

        from egammaAlgs.EMBremCollectionBuilderConfig import (
            EMBremCollectionBuilderCfg)
        acc.merge(EMBremCollectionBuilderCfg(flags))

        from egammaAlgs.EMGSFCaloExtensionBuilderConfig import (
            EMGSFCaloExtensionBuilderCfg)
        acc.merge(EMGSFCaloExtensionBuilderCfg(flags))

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

    mlog.info("EGamma reconstruction configured")

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
    acc.merge(egammaReconstructionCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammareconstructionconfig.pkl", "wb") as f:
        acc.store(f)
