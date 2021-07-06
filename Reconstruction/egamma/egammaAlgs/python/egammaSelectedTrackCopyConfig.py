# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate egammaSelectedTrackCopy with default configuration"

from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCfg
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaCaloTools.egammaCaloToolsFactories import egammaCheckEnergyDepositTool
egammaCaloClusterSelector = CompFactory.egammaCaloClusterSelector
egammaSelectedTrackCopy = CompFactory.egammaSelectedTrackCopy


def egammaSelectedTrackCopyCfg(
        flags,
        name='egammaSelectedTrackCopy',
        **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    if "egammaCaloClusterSelector" not in kwargs:
        egammaCaloClusterGSFSelector = egammaCaloClusterSelector(
            name='caloClusterGSFSelector',
            EMEtCut=2250.,
            EMEtSplittingFraction=0.7,
            EMFCut=0.5
        )

        kwargs["egammaCaloClusterSelector"] = egammaCaloClusterGSFSelector
        kwargs.setdefault("egammaCheckEnergyDepositTool",
                          egammaCheckEnergyDepositTool())

    if "ExtrapolationTool" not in kwargs:
        extraptool = EMExtrapolationToolsCfg(flags)
        kwargs["ExtrapolationTool"] = extraptool.popPrivateTools()
        acc.merge(extraptool)

    kwargs.setdefault(
        "ClusterContainerName",
        flags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault(
        "TrackParticleContainerName",
        flags.Egamma.Keys.Input.TrackParticles)

    egseltrkcpAlg = egammaSelectedTrackCopy(name, **kwargs)

    acc.addEventAlgo(egseltrkcpAlg)
    return acc
