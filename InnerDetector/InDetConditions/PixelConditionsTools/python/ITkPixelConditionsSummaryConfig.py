"""Define a function to configure ITkPixelConditionsSummaryCfg

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory

from PixelCabling.PixelCablingConfigNew import ITkPixelCablingSvcCfg
from PixelConditionsAlgorithms.ITkPixelConditionsConfig import (
    ITkPixelDCSCondStateAlgCfg, ITkPixelDCSCondStatusAlgCfg, ITkPixelDeadMapCondAlgCfg
)

def ITkPixelConditionsSummaryCfg(flags, name="ITkPixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator with tool for ITk Pixel Conditions"""
    acc = ITkPixelCablingSvcCfg(flags)
    acc.merge(ITkPixelDCSCondStateAlgCfg(flags))
    acc.merge(ITkPixelDCSCondStatusAlgCfg(flags))
    acc.merge(ITkPixelDeadMapCondAlgCfg(flags))

    kwargs.setdefault("PixelCablingSvc", acc.getPrimary())
    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("PixelDCSStateCondData", "ITkPixelDCSStateCondData")
    kwargs.setdefault("PixelDCSStatusCondData", "ITkPixelDCSStatusCondData")
    kwargs.setdefault("PixelDeadMapCondData", "ITkPixelDeadMapCondData")
    kwargs.setdefault("UseByteStreamFEI4", not flags.Input.isMC)
    kwargs.setdefault("UseByteStreamFEI3", not flags.Input.isMC)

    if flags.InDet.usePixelDCS:
        kwargs.setdefault("IsActiveStates", [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ])
        kwargs.setdefault("IsActiveStatus", [ 'OK', 'WARNING', 'ERROR', 'FATAL' ])

    acc.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name=name + "Tool", **kwargs))
    return acc
