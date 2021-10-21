# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the full EGamma reconstruction.
          Note that
          egammaTopoClusterCopier is scheduled in TrackRecoConfig
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def EGammaReconstructionCfg(flags, name="EGammaReconstruction"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma reconstruction configuration')

    acc = ComponentAccumulator()

    # Depending on what is availabe we might want
    # to disable certain things

    # without calo no egamma
    if not flags.Detector.EnableCalo:
        flags.Egamma.enabled = False
        flags.Egamma.doCaloSeeded = False
        flags.Egamma.doForwardSeeded = False

    # Tracking
    if not flags.Detector.EnableID:
        flags.Egamma.doGSF = False
        flags.Egamma.doConversionBuilding = False

    # MC Truth
    if not flags.Input.isMC:
        flags.Egamma.doTruthAssociation = False

    # if not enabled add nothing
    if not flags.Egamma.enabled:
        return acc

    # Add algorithms
    if flags.Egamma.doGSF:

        from egammaAlgs.egammaSelectedTrackCopyConfig import (
            egammaSelectedTrackCopyCfg)
        acc.merge(egammaSelectedTrackCopyCfg(flags))

        from egammaAlgs.EMBremCollectionBuilderConfig import (
            EMBremCollectionBuilderCfg)
        acc.merge(EMBremCollectionBuilderCfg(flags))

        from egammaAlgs.EMGSFCaloExtensionBuilderConfig import (
            EMGSFCaloExtensionBuilderCfg)
        acc.merge(EMGSFCaloExtensionBuilderCfg(flags))

    if flags.Egamma.doConversionBuilding:
        from egammaAlgs.EMVertexBuilderConfig import EMVertexBuilderCfg
        acc.merge(EMVertexBuilderCfg(flags))

    if flags.Egamma.doCaloSeeded:
        from egammaAlgs.egammaRecBuilderConfig import egammaRecBuilderCfg
        acc.merge(egammaRecBuilderCfg(flags))
        from egammaAlgs.egammaSuperClusterBuilderConfig import (
            electronSuperClusterBuilderCfg, photonSuperClusterBuilderCfg)
        acc.merge(electronSuperClusterBuilderCfg(flags))
        acc.merge(photonSuperClusterBuilderCfg(flags))
        from egammaAlgs.topoEgammaBuilderConfig import topoEgammaBuilderCfg
        acc.merge(topoEgammaBuilderCfg(flags))

    if flags.Egamma.doForwardSeeded:
        from egammaAlgs.egammaForwardBuilderConfig import (
            egammaForwardBuilderCfg)
        acc.merge(egammaForwardBuilderCfg(flags))

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

    acc = MainServicesCfg(flags)
    acc.merge(EGammaReconstructionCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammabuilderconfig.pkl", "wb") as f:
        acc.store(f)
