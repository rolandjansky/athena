# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate the
two supercluster builders with default configuration"""
__author__ = "Jovan Mitrevski"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaRec import egammaKeys

from egammaTools.egammaToolsFactories import (
    EMTrackMatchBuilder, EMConversionBuilder, egammaSwTool, egammaMVASvc)

from egammaCaloTools.egammaCaloToolsFactories import (
    egammaCheckEnergyDepositTool)

electronSuperClusterBuilder = AlgFactory(
    egammaAlgsConf.electronSuperClusterBuilder,
    name='electronSuperClusterBuilder',
    InputEgammaRecContainerName=egammaKeys.EgammaRecKey(),
    SuperElectronRecCollectionName=egammaKeys.ElectronSuperRecKey(),
    ClusterCorrectionTool=egammaSwTool,
    egammaCheckEnergyDepositTool=egammaCheckEnergyDepositTool,
    MVACalibSvc=egammaMVASvc,
    EtThresholdCut=1000,
    TrackMatchBuilderTool=EMTrackMatchBuilder
)

photonSuperClusterBuilder = AlgFactory(
    egammaAlgsConf.photonSuperClusterBuilder,
    name='photonSuperClusterBuilder',
    InputEgammaRecContainerName=egammaKeys.EgammaRecKey(),
    SuperPhotonRecCollectionName=egammaKeys.PhotonSuperRecKey(),
    ClusterCorrectionTool=egammaSwTool,
    egammaCheckEnergyDepositTool=egammaCheckEnergyDepositTool,
    MVACalibSvc=egammaMVASvc,
    ConversionBuilderTool=EMConversionBuilder
)
