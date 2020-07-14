"""Define a function to configure PixelConditionsSummaryCfg

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg
# NEW FOR RUN3 , PixelDeadMapCondAlgCfg
)

def PixelConditionsSummaryCfg(flags, name="PixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator with tool for Pixel Conditions"""
    acc = ComponentAccumulator()
    kwargs.setdefault("UseByteStream", not flags.Input.isMC)
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
# NEW FOR RUN3    acc.merge(PixelDeadMapCondAlgCfg(flags))
    acc.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name=name + "Tool", **kwargs))
    return acc

