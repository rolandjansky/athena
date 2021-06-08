"""Define a function to configure PixelConditionsSummaryCfg

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory

from PixelCabling.PixelCablingConfigNew import PixelCablingSvcCfg
from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg, PixelDeadMapCondAlgCfg
)

def PixelConditionsSummaryCfg(flags, name="PixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator with tool for Pixel Conditions"""
    acc = PixelCablingSvcCfg(flags)
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
    acc.merge(PixelDeadMapCondAlgCfg(flags))

    kwargs.setdefault("PixelCablingSvc", acc.getPrimary())
    kwargs.setdefault("UseByteStreamFEI4", not flags.Input.isMC and not flags.Overlay.DataOverlay)
    kwargs.setdefault("UseByteStreamFEI3", not flags.Input.isMC and not flags.Overlay.DataOverlay)

    if flags.InDet.usePixelDCS:
        kwargs.setdefault("IsActiveStates", [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ])
        kwargs.setdefault("IsActiveStatus", [ 'OK', 'WARNING', 'ERROR', 'FATAL' ])

    acc.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name=name + "Tool", **kwargs))
    return acc
