# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate egammaSelectedTrackCopy with default configuration"

from egammaTrackTools.egammaTrackToolsConfig import (
    EMExtrapolationToolsCfg, EMExtrapolationToolsCommonCacheCfg)
from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def egammaSelectedTrackCopyCfg(
        flags,
        name='egammaSelectedTrackCopy',
        **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    if "egammaCaloClusterSelector" not in kwargs:
        egammaCaloClusterGSFSelector = CompFactory.egammaCaloClusterSelector(
            name='caloClusterGSFSelector',
            EMEtCut=2250.,
            EMEtSplittingFraction=0.7,
            EMFCut=0.5
        )
        kwargs["egammaCaloClusterSelector"] = egammaCaloClusterGSFSelector

    if "ExtrapolationTool" not in kwargs:
        extraptool = EMExtrapolationToolsCfg(
            flags, name="EMExtrapolationTools")
        kwargs["ExtrapolationTool"] = acc.popToolsAndMerge(extraptool)

    if "ExtrapolationToolCommonCache" not in kwargs:
        kwargs["ExtrapolationToolCommonCache"] = acc.popToolsAndMerge(
            EMExtrapolationToolsCommonCacheCfg(flags))

    kwargs.setdefault(
        "ClusterContainerName",
        flags.Egamma.Keys.Internal.EgammaTopoClusters)
    kwargs.setdefault(
        "TrackParticleContainerName",
        flags.Egamma.Keys.Input.TrackParticles)

    egseltrkcpAlg = CompFactory.egammaSelectedTrackCopy(name, **kwargs)

    acc.addEventAlgo(egseltrkcpAlg)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO
    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge(egammaSelectedTrackCopyCfg(flags))
    mlog = logging.getLogger("egammaSelectedTrackCopyConfigTest")
    mlog.info("Configuring  egammaSelectedTrackCopy: ")
    printProperties(mlog,
                    acc.getEventAlgo("egammaSelectedTrackCopy"),
                    nestLevel=1,
                    printDefaults=True)
    with open("egammaselectedtrackCopy.pkl", "wb") as f:
        acc.store(f)
