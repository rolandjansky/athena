# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure the electron and photon selectors."

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTools.egammaToolsConf import EMConversionBuilder
from egammaTrackTools.egammaTrackToolsConfig import EMExtrapolationToolsCfg


def EMConversionBuilderCfg(flags, name='EMConversionBuilder', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    if "ExtrapolationTool" not in kwargs:
        extraptool = EMExtrapolationToolsCfg(flags)
        kwargs["ExtrapolationTool"] = extraptool.popPrivateTools()
        acc.merge(extraptool)
    kwargs.setdefault("ConversionContainerName", flags.Egamma.Keys.Output.ConversionVertices)

    emconv = EMConversionBuilder(name, **kwargs)

    acc.setPrivateTools(emconv)
    return acc
