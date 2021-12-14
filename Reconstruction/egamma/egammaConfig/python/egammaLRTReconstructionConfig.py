# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the EGamma LRT reconstruction.
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTrackTools.egammaTrackToolsConfig import (
    EMExtrapolationToolsCfg)


def egammaLRTReconstructionCfg(flags, name="egammaLRTReconstruction"):

    mlog = logging.getLogger(name)
    mlog.info('Starting EGamma LRT reconstruction configuration')

    acc = ComponentAccumulator()

    # Add e/gamma tracking algorithms
    if flags.Egamma.doTracking:
        from egammaAlgs.egammaSelectedTrackCopyConfig import (
            egammaSelectedTrackCopyCfg)
        emextLRT = acc.popToolsAndMerge(
            EMExtrapolationToolsCfg(flags))
        acc.merge(egammaSelectedTrackCopyCfg(
            flags,
            name="LRTegammaSelectedTrackCopy",
            TrackParticleContainerName="InDetLargeD0TrackParticles",
            OutputTrkPartContainerName="LRTegammaSelectedTrackParticles",
            ExtrapolationTool=emextLRT)
        )

        from egammaAlgs.EMBremCollectionBuilderConfig import (
            EMBremCollectionBuilderCfg)
        acc.merge(EMBremCollectionBuilderCfg(
            flags,
            name='LRTEMBremCollectionBuilder',
            TrackParticleContainerName='InDetLargeD0TrackParticles',
            SelectedTrackParticleContainerName='LRTegammaSelectedTrackParticles',
            OutputTrkPartContainerName='LRT'+flags.Egamma.Keys.Output.GSFTrackParticles,
            OutputTrackContainerName='LRT'+flags.Egamma.Keys.Output.GSFTracks)
        )

    # Add calo seeded central algorithms
    if flags.Egamma.doCentral:
        from egammaAlgs.egammaRecBuilderConfig import (
            egammaRecBuilderCfg)
        from egammaTools.EMTrackMatchBuilderConfig import (
            EMTrackMatchBuilderCfg)
        emextLRT = acc.popToolsAndMerge(
            EMExtrapolationToolsCfg(flags))
        lrtemtrackmatch = acc.popToolsAndMerge(EMTrackMatchBuilderCfg(
            flags,
            name='LRTEMTrackMatchBuilder',
            TrackParticlesName='LRT'+flags.Egamma.Keys.Output.GSFTrackParticles,
            ExtrapolationTool=emextLRT)
        )
        acc.merge(egammaRecBuilderCfg(
            flags,
            name='LRTegammaRecBuilder',
            egammaRecContainer="LRT"+flags.Egamma.Keys.Internal.EgammaRecs,
            TrackMatchBuilderTool=lrtemtrackmatch,
            doConversions=False)
        )

        from egammaAlgs.egammaSuperClusterBuilderConfig import (
            electronSuperClusterBuilderCfg)
        acc.merge(electronSuperClusterBuilderCfg(
            flags,
            name='LRTelectronSuperClusterBuilder',
            InputEgammaRecContainerName='LRT'+flags.Egamma.Keys.Internal.EgammaRecs,
            SuperElectronRecCollectionName='LRT' +
            flags.Egamma.Keys.Internal.ElectronSuperRecs,
            SuperClusterCollectionName='LRTElectronSuperClusters',
            TrackMatchBuilderTool=lrtemtrackmatch)
        )

        from egammaAlgs.topoEgammaBuilderConfig import (
            topoEgammaBuilderCfg)
        from egammaTools.EMClusterToolConfig import (
            EMClusterToolCfg)
        LRTEMClusterTool = acc.popToolsAndMerge(EMClusterToolCfg(
            flags,
            name='LRTEMClusterTool',
            OutputClusterContainerName='LRT'+flags.Egamma.Keys.Output.CaloClusters)
        )
        acc.merge(topoEgammaBuilderCfg(
            flags,
            name='LRTtopoEgammaBuilder',
            InputElectronRecCollectionName='LRT' +
            flags.Egamma.Keys.Internal.ElectronSuperRecs,
            ElectronOutputName='LRT'+flags.Egamma.Keys.Output.Electrons,
            PhotonOutputName="LRT"+flags.Egamma.Keys.Output.Photons,
            EMClusterTool=LRTEMClusterTool,
            doPhotons=False)
        )

    # Add truth association
    if flags.Egamma.doTruthAssociation:
        from egammaAlgs.egammaTruthAssociationConfig import (
            egammaTruthAssociationCfg)
        acc.merge(egammaTruthAssociationCfg(
            flags,
            name='LRTegammaTruthAssociationAlg',
            ElectronContainerName='LRT'+flags.Egamma.Keys.Output.Electrons,
            EgammaTruthContainerName='LRT'+flags.Egamma.Keys.Output.TruthParticles,
            MatchPhotons=False,
            MatchForwardElectrons=False)
        )

    # To use egamma CA within standard config
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
        for el in acc._allSequences:
            el.name = "TopAlg"

    mlog.info("EGamma LRT reconstruction configured")

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
    acc.merge(egammaLRTReconstructionCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)

    with open("egammalrtbuilderconfig.pkl", "wb") as f:
        acc.store(f)
