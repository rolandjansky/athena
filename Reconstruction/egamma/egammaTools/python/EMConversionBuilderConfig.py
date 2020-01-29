# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure the electron and photon selectors."

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
EMConversionBuilder=CompFactory.EMConversionBuilder
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
