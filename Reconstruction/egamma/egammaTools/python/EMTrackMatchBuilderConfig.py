# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate EMTrackMatchBuilder with default configuration"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
EMTrackMatchBuilder=CompFactory.EMTrackMatchBuilder
from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCacheCfg


def EMTrackMatchBuilderCfg(flags, name='EMTrackMatchBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "ExtrapolationTool" not in kwargs:
        extrapcache = EMExtrapolationToolsCacheCfg(flags)
        kwargs["ExtrapolationTool"] = extrapcache.popPrivateTools()
        acc.merge(extrapcache)

    kwargs.setdefault("TrackParticlesName", flags.Egamma.Keys.Output.GSFTrackParticles)
    kwargs.setdefault("broadDeltaEta",      0.1)    # candidate match is done in 2 times this  so +- 0.2
    kwargs.setdefault("broadDeltaPhi",      0.15)   # candidate match is done in 2 times this  so +- 0.3
    kwargs.setdefault("useCandidateMatch",  True)
    kwargs.setdefault("useScoring",         True)
    kwargs.setdefault("SecondPassRescale",  True)
    kwargs.setdefault("UseRescaleMetric",   True)
    kwargs.setdefault("isCosmics",          flags.Beam.Type == "cosmics")

    tool = EMTrackMatchBuilder(name, **kwargs)

    acc.setPrivateTools(tool)
    return acc
