# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate EMTrackMatchBuilder with default configuration"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTrackTools.egammaTrackToolsConfig import (
    EMExtrapolationToolsCfg)


def EMTrackMatchBuilderCfg(flags, name='EMTrackMatchBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "ExtrapolationTool" not in kwargs:
        extrap = EMExtrapolationToolsCfg(flags)
        kwargs["ExtrapolationTool"] = acc.popToolsAndMerge(extrap)

    kwargs.setdefault("TrackParticlesName",
                      flags.Egamma.Keys.Output.GSFTrackParticles)
    # candidate match is done in 2 times this  so +- 0.2
    kwargs.setdefault("broadDeltaEta",      0.1)
    # candidate match is done in 2 times this  so +- 0.3
    kwargs.setdefault("broadDeltaPhi",      0.15)
    kwargs.setdefault("useCandidateMatch",  True)
    kwargs.setdefault("useScoring",         True)
    kwargs.setdefault("SecondPassRescale",  True)
    kwargs.setdefault("UseRescaleMetric",   True)
    kwargs.setdefault("isCosmics",          flags.Beam.Type == "cosmics")

    tool = CompFactory.EMTrackMatchBuilder(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc


if __name__ == "__main__":

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaCommon.Configurable import Configurable
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    Configurable.configurableRun3Behavior = True

    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.fillFromArgs()
    ConfigFlags.lock()
    ConfigFlags.dump()

    cfg = ComponentAccumulator()
    mlog = logging.getLogger("EMTrackMatchBuilderConfigTest")
    mlog.info("Configuring  EMTrackMatchBuilder: ")
    printProperties(mlog, cfg.popToolsAndMerge(
        EMTrackMatchBuilderCfg(ConfigFlags)),
        nestLevel=1,
        printDefaults=True)

    f = open("emtrackmatchbuilder.pkl", "wb")
    cfg.store(f)
    f.close()
