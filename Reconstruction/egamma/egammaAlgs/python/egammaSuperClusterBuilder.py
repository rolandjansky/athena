# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate the two supercluster builders with default configuration"
__author__ = "Jovan Mitrevski"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import AlgFactory
from egammaRec.egammaRecFlags import jobproperties # to set jobproperties.egammaRecFlags
from egammaRec import egammaKeys

from egammaTools.egammaToolsFactories import \
    EMTrackMatchBuilder, EMConversionBuilder, egammaSwTool, egammaMVATool

electronSuperClusterBuilder = AlgFactory( egammaAlgsConf.electronSuperClusterBuilder,
                                          name = 'electronSuperClusterBuilder',
                                          InputEgammaRecContainerName=egammaKeys.EgammaRecKey(),
                                          SuperElectronRecCollectionName=egammaKeys.ElectronSuperRecKey(),
                                          ClusterCorrectionTool=egammaSwTool,
                                          MVACalibTool=egammaMVATool,
                                          EtThresholdCut=1000, 
                                          TrackMatchBuilderTool = EMTrackMatchBuilder
                                         )

photonSuperClusterBuilder = AlgFactory( egammaAlgsConf.photonSuperClusterBuilder,
                                        name = 'photonSuperClusterBuilder',
                                        InputEgammaRecContainerName=egammaKeys.EgammaRecKey(),
                                        SuperPhotonRecCollectionName=egammaKeys.PhotonSuperRecKey(),
                                        ClusterCorrectionTool=egammaSwTool,
                                        MVACalibTool= egammaMVATool,
                                        ConversionBuilderTool = EMConversionBuilder
                                        )
