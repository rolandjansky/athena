# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

__doc__ = """ToolFactory to instantiate
egammaSelectedTrackCopy with default configuration"""
__author__ = "Christos"

from egammaAlgs import egammaAlgsConf
from egammaRec.Factories import ToolFactory, AlgFactory
# to set jobproperties.egammaRecFlags
from egammaRec.egammaRecFlags import jobproperties
from InDetRecExample.InDetKeys import InDetKeys
from egammaTrackTools.egammaTrackToolsFactories import (
    EMExtrapolationTools, EMExtrapolationToolsCommonCache)
from egammaCaloTools import egammaCaloToolsConf

egammaCaloClusterGSFSelector = ToolFactory(
    egammaCaloToolsConf.egammaCaloClusterSelector,
    name='caloClusterGSFSelector',
    EMEtCut=2250.,
    EMEtSplittingFraction=0.7,
    EMFCut=0.5
)

egammaSelectedTrackCopy = AlgFactory(
    egammaAlgsConf.egammaSelectedTrackCopy,
    name='egammaSelectedTrackCopy',
    ExtrapolationTool=EMExtrapolationTools,
    ExtrapolationToolCommonCache=EMExtrapolationToolsCommonCache,
    ClusterContainerName=jobproperties.egammaRecFlags.egammaTopoClusterCollection(),
    TrackParticleContainerName=InDetKeys.xAODTrackParticleContainer(),
    egammaCaloClusterSelector=egammaCaloClusterGSFSelector
)
