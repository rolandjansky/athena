# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate egammaSelectedTrackCopy with default configuration"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaCaloTools.egammaCaloToolsConf import egammaCaloClusterSelector
from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCfg
from egammaAlgs.egammaAlgsConf import egammaSelectedTrackCopy


def egammaSelectedTrackCopyCfg(flags, name='egammaSelectedTrackCopy', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    if "egammaCaloClusterSelector" not in kwargs:
        egammaCaloClusterGSFSelector = egammaCaloClusterSelector(name='caloClusterGSFSelector',
                                                                 EMEtRanges = [2250.],
                                                                 EMFCuts = [0.75]
                                                                 )

        kwargs["egammaCaloClusterSelector"] = egammaCaloClusterGSFSelector

    if "ExtrapolationTool" not in kwargs:
        extraptool = EMExtrapolationToolsCfg(flags)
        kwargs["ExtrapolationTool"] = extraptool.popPrivateTools()
        acc.merge(extraptool)

    kwargs.setdefault("ClusterContainerName", flags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault("TrackParticleContainerName", flags.Egamma.Keys.Input.TrackParticles)
    kwargs.setdefault("narrowDeltaPhiBrem", 0.20)

    egseltrkcpAlg = egammaSelectedTrackCopy(name, **kwargs)

    acc.addEventAlgo(egseltrkcpAlg)
    return acc
