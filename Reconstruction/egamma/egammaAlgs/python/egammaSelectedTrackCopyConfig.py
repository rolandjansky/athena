# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate egammaSelectedTrackCopy with default configuration"

from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCfg
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
egammaCaloClusterSelector = CompFactory.egammaCaloClusterSelector
egammaSelectedTrackCopy = CompFactory.egammaSelectedTrackCopy


def egammaSelectedTrackCopyCfg(flags, name='egammaSelectedTrackCopy', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    if "egammaCaloClusterSelector" not in kwargs:
        egammaCaloClusterGSFSelector = egammaCaloClusterSelector(name='caloClusterGSFSelector',
                                                                 EMEtRanges=[2200.],
                                                                 EMFCuts=[0.55],
                                                                 # 3x7/7x7=0.429. Below this
                                                                 # there is more energy outside the
                                                                 # 3x7 core than inside
                                                                 RetaCut=[0.4]
                                                                 )

        kwargs["egammaCaloClusterSelector"] = egammaCaloClusterGSFSelector

    if "ExtrapolationTool" not in kwargs:
        extraptool = EMExtrapolationToolsCfg(flags)
        kwargs["ExtrapolationTool"] = extraptool.popPrivateTools()
        acc.merge(extraptool)

    kwargs.setdefault("ClusterContainerName",
                      flags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault("TrackParticleContainerName",
                      flags.Egamma.Keys.Input.TrackParticles)

    egseltrkcpAlg = egammaSelectedTrackCopy(name, **kwargs)

    acc.addEventAlgo(egseltrkcpAlg)
    return acc
