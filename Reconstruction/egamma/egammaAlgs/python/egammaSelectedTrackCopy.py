# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

__doc__ = "ToolFactory to instantiate egammaSelectedTrackCopy with default configuration"
__author__ = "Christos"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import ToolFactory, AlgFactory
# to set jobproperties.egammaRecFlags
from egammaRec.egammaRecFlags import jobproperties
from InDetRecExample.InDetKeys import InDetKeys
from egammaTrackTools.egammaTrackToolsFactories import EMExtrapolationTools, EMExtrapolationToolsCommonCache
from egammaCaloTools import egammaCaloToolsConf

egammaCaloClusterGSFSelector = ToolFactory(egammaCaloToolsConf.egammaCaloClusterSelector,
                                           name='caloClusterGSFSelector',
                                           EMEtRanges=[2300.],
                                           EMFCuts=[0.6],
                                           # 3x7/7x7=0.429. Below this
                                           # there is more energy outside the
                                           # 3x7 core than inside
                                           RetaCut=[0.4]
                                           )

egammaSelectedTrackCopy = AlgFactory(egammaAlgsConf.egammaSelectedTrackCopy,
                                     name='egammaSelectedTrackCopy',
                                     ExtrapolationTool=EMExtrapolationTools,
                                     ExtrapolationToolCommonCache=EMExtrapolationToolsCommonCache,
                                     ClusterContainerName=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
                                     TrackParticleContainerName=InDetKeys.xAODTrackParticleContainer(),
                                     egammaCaloClusterSelector=egammaCaloClusterGSFSelector
                                     )
