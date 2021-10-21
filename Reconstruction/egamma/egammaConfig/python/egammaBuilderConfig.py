# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the full EGamma reconstruction.
          egammaTopoClusterCopier is scheduled in TrackRecoConfig
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def EGammaReconstructionCfg(flags, name="EGammaReconstruction"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma reconstruction configuration')

    acc = ComponentAccumulator()

    from egammaAlgs.egammaSelectedTrackCopyConfig import (
        egammaSelectedTrackCopyCfg)
    acc.merge(egammaSelectedTrackCopyCfg(flags))
    from egammaAlgs.EMBremCollectionBuilderConfig import (
        EMBremCollectionBuilderCfg)
    acc.merge(EMBremCollectionBuilderCfg(flags))
    from egammaAlgs.EMGSFCaloExtensionBuilderConfig import (
        EMGSFCaloExtensionBuilderCfg)
    acc.merge(EMGSFCaloExtensionBuilderCfg(flags))
    from egammaAlgs.EMVertexBuilderConfig import EMVertexBuilderCfg
    acc.merge(EMVertexBuilderCfg(flags))
    from egammaAlgs.egammaRecBuilderConfig import egammaRecBuilderCfg
    acc.merge(egammaRecBuilderCfg(flags))
    from egammaAlgs.egammaSuperClusterBuilderConfig import (
        electronSuperClusterBuilderCfg, photonSuperClusterBuilderCfg)
    acc.merge(electronSuperClusterBuilderCfg(flags))
    acc.merge(photonSuperClusterBuilderCfg(flags))
    from egammaAlgs.topoEgammaBuilderConfig import topoEgammaBuilderCfg
    acc.merge(topoEgammaBuilderCfg(flags))

    mlog.info("EGamma reconstruction configured")

    return acc
