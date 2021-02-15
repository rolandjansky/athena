# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from egammaRec import egammaKeys
from egammaRec.egammaTruthAssociationAlg import egammaTruthAssociationAlg
from egammaAlgs.egammaSelectedTrackCopy import egammaSelectedTrackCopy
from egammaAlgs.EMBremCollectionBuilder import EMBremCollectionBuilder
from egammaAlgs.EMGSFCaloExtensionBuilder import EMGSFCaloExtensionBuilder
from egammaAlgs.egammaRecBuilder import egammaRecBuilder
from egammaAlgs.egammaSuperClusterBuilder import (
    electronSuperClusterBuilder, photonSuperClusterBuilder)
from egammaAlgs.topoEgammaBuilder import topoEgammaBuilder
from egammaTools.EMTrackMatchBuilder import EMTrackMatchBuilder
from egammaTrackTools.egammaTrackToolsFactories import (
    EMExtrapolationToolsCommonCache, EMExtrapolationToolsCache)
from InDetRecExample.InDetKeys import InDetKeys
from egammaTools.egammaToolsFactories import (
    EMClusterTool, EMFourMomBuilder,
    EMShowerBuilder, egammaOQFlagsBuilder, ElectronPIDBuilder)

LRTEMExtrapolationToolsCommonCache = EMExtrapolationToolsCommonCache.copy(
    name="LRTEMExtrapolationToolsCommonCache",
    LastCache='ParticleCaloExtension_LRT'
)

LRTegammaSelectedTrackCopy = egammaSelectedTrackCopy.copy(
    name='LRTegammaSelectedTrackCopy',
    OutputTrkPartContainerName="LRTegammaSelectedTrackParticles",
    TrackParticleContainerName=InDetKeys.xAODLargeD0TrackParticleContainer(),
    ExtrapolationToolCommonCache=LRTEMExtrapolationToolsCommonCache
)

LRTEMBremCollectionBuilder = EMBremCollectionBuilder.copy(
    name="LRTEMBremCollectionBuilder",
    TrackParticleContainerName=InDetKeys.xAODLargeD0TrackParticleContainer(),
    SelectedTrackParticleContainerName="LRTegammaSelectedTrackParticles",
    OutputTrkPartContainerName="LRT"+egammaKeys.outputTrackParticleKey(),
    OutputTrackContainerName="LRT"+egammaKeys.outputTrackKey(),
)

LRTEMGSFCaloExtensionBuilder = EMGSFCaloExtensionBuilder.copy(
    name='LRTEMGSFCaloExtensionBuilder',
    GSFPerigeeCache='LRTGSFPerigeeCaloExtension',
    GSFLastCache='LRTGSFLastCaloExtension',
    GFFTrkPartContainerName="LRT"+egammaKeys.outputTrackParticleKey())

LRTEMExtrapolationToolsCache = EMExtrapolationToolsCache.copy(
    name="LRTEMExtrapolationToolsCache",
    PerigeeCache='LRTGSFPerigeeCaloExtension',
    LastCache='LRTGSFLastCaloExtension',
    useCaching=True,
    useLastCaching=True
)

LRTEMTrackMatchBuilder = EMTrackMatchBuilder.copy(
    name="LRTEMTrackMatchBuilder",
    TrackParticlesName="LRT"+egammaKeys.outputTrackParticleKey(),
    ExtrapolationTool=LRTEMExtrapolationToolsCache
)

LRTegammaRecBuilder = egammaRecBuilder.copy(
    name='LRTegammaRecBuilder',
    egammaRecContainer="LRT"+egammaKeys.EgammaRecKey(),
    # Builder tools
    TrackMatchBuilderTool=LRTEMTrackMatchBuilder,
    doConversions=False
)

LRTelectronSuperClusterBuilder = electronSuperClusterBuilder.copy(
    name='LRTelectronSuperClusterBuilder',
    InputEgammaRecContainerName="LRT"+egammaKeys.EgammaRecKey(),
    SuperElectronRecCollectionName="LRT"+egammaKeys.ElectronSuperRecKey(),
    SuperClusterCollectionName='LRTElectronSuperClusters',
    TrackMatchBuilderTool=LRTEMTrackMatchBuilder
)

LRTphotonSuperClusterBuilder = photonSuperClusterBuilder.copy(
    name='LRTphotonSuperClusterBuilder',
    InputEgammaRecContainerName="LRT"+egammaKeys.EgammaRecKey(),
    SuperPhotonRecCollectionName="LRT"+egammaKeys.PhotonSuperRecKey(),
    SuperClusterCollectionName='LRTPhotonSuperClusters',
)

LRTEMClusterTool = EMClusterTool.copy(
    name='LRTEMClusterTool',
    OutputClusterContainerName="LRT"+egammaKeys.outputClusterKey(),
    OutputTopoSeededClusterContainerName='LRT' +
    egammaKeys.outputTopoSeededClusterKey(),
)

LRTtopoEgammaBuilder = topoEgammaBuilder.copy(
    name='LRTtopoEgammaBuilder',
    SuperElectronRecCollectionName="LRT"+egammaKeys.ElectronSuperRecKey(),
    ElectronOutputName="LRT"+egammaKeys.outputElectronKey(),
    SuperPhotonRecCollectionName="LRT"+egammaKeys.PhotonSuperRecKey(),
    PhotonOutputName="LRT"+egammaKeys.outputPhotonKey(),
    EMClusterTool=LRTEMClusterTool,
    EMShowerTool=EMShowerBuilder,
    ObjectQualityTool=egammaOQFlagsBuilder,
    egammaTools=[EMFourMomBuilder()],
    ElectronTools=[ElectronPIDBuilder()],
    doPhotons=False
)

LRTegammaTruthAssociationAlg = egammaTruthAssociationAlg.copy(
    name='LRTegammaTruthAssociationAlg',
    ElectronContainerName="LRT"+egammaKeys.outputElectronKey(),
    PhotonContainerName="LRT"+egammaKeys.outputPhotonKey(),
    EgammaTruthContainerName="LRT"+egammaKeys.outputTruthKey(),
    MatchPhotons=False,
    MatchForwardElectrons=False,
)
