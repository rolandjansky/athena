# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate egammaRecBuilder with default configuration
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTools.EMTrackMatchBuilderConfig import EMTrackMatchBuilderCfg
from egammaTools.EMConversionBuilderConfig import EMConversionBuilderCfg
egammaRecBuilder = CompFactory.egammaRecBuilder


def egammaRecBuilderCfg(
        flags,
        name='egammaRecBuilder',
        **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()
    if "TrackMatchBuilderTool" not in kwargs:
        emtrkmatch = EMTrackMatchBuilderCfg(flags)
        kwargs["TrackMatchBuilderTool"] = emtrkmatch.popPrivateTools()
        acc.merge(emtrkmatch)

    if "ConversionBuilderTool" not in kwargs:
        emcnv = EMConversionBuilderCfg(flags)
        kwargs["ConversionBuilderTool"] = emcnv.popPrivateTools()
        acc.merge(emcnv)

    kwargs.setdefault(
        "egammaRecContainer",
        flags.Egamma.Keys.Internal.EgammaRecs)
    kwargs.setdefault(
        "InputTopoClusterContainerName",
        flags.Egamma.Keys.Internal.EgammaTopoClusters)

    egrecAlg = egammaRecBuilder(name, **kwargs)

    acc.addEventAlgo(egrecAlg)
    return acc
