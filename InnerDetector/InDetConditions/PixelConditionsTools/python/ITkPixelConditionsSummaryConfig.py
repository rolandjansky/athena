"""Define a function to configure ITkPixelConditionsSummaryCfg

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory

from PixelConditionsAlgorithms.ITkPixelConditionsConfig import (
    ITkPixelDCSCondStateAlgCfg, ITkPixelDCSCondStatusAlgCfg, ITkPixelDeadMapCondAlgCfg
)
from PixelReadoutGeometry.PixelReadoutGeometryConfig import ITkPixelReadoutManagerCfg

def ITkPixelConditionsSummaryCfg(flags, name="ITkPixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator with tool for ITk Pixel Conditions"""
    acc = ITkPixelReadoutManagerCfg(flags)
    acc.merge(ITkPixelDCSCondStateAlgCfg(flags))
    acc.merge(ITkPixelDCSCondStatusAlgCfg(flags))
    acc.merge(ITkPixelDeadMapCondAlgCfg(flags))

    kwargs.setdefault("PixelReadoutManager", acc.getPrimary())
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("PixelDCSStateCondData", "ITkPixelDCSStateCondData")
    kwargs.setdefault("PixelDCSStatusCondData", "ITkPixelDCSStatusCondData")
    kwargs.setdefault("PixelDeadMapCondData", "ITkPixelDeadMapCondData")
    kwargs.setdefault("UseByteStreamFEI4", not flags.Input.isMC)
    kwargs.setdefault("UseByteStreamFEI3", not flags.Input.isMC)
    kwargs.setdefault("UseByteStreamRD53", False) # Turned off until BS format is defined

    acc.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name=name + "Tool", **kwargs))
    return acc
