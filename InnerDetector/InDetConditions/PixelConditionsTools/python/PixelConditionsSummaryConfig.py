"""Define a function to configure PixelConditionsSummaryCfg

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format

from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelDCSCondStateAlgCfg, PixelDCSCondStatusAlgCfg, PixelDeadMapCondAlgCfg
)
from PixelReadoutGeometry.PixelReadoutGeometryConfig import PixelReadoutManagerCfg

def PixelConditionsSummaryCfg(flags, name="PixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator with tool for Pixel Conditions"""
    acc = PixelReadoutManagerCfg(flags)
    acc.merge(PixelDCSCondStateAlgCfg(flags))
    acc.merge(PixelDCSCondStatusAlgCfg(flags))
    acc.merge(PixelDeadMapCondAlgCfg(flags))

    kwargs.setdefault("PixelReadoutManager", acc.getPrimary())
    kwargs.setdefault("UseByteStreamFEI4", not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.Input.Format is Format.BS)
    kwargs.setdefault("UseByteStreamFEI3", not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.Input.Format is Format.BS)

    if flags.InDet.usePixelDCS:
        kwargs.setdefault("IsActiveStates", [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ])
        kwargs.setdefault("IsActiveStatus", [ 'OK', 'WARNING', 'ERROR', 'FATAL' ])

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDetectorElementCondAlgCfg
    acc.merge(PixelDetectorElementCondAlgCfg(flags))
    kwargs.setdefault( "PixelDetEleCollKey", "PixelDetectorElementCollection")

    acc.setPrivateTools(CompFactory.PixelConditionsSummaryTool(name=name + "Tool", **kwargs))
    return acc

def PixelConditionsSummaryToolNoByteStreamErrorsActiveOnlyCfg(flags,name = "PixelConditionsSummaryToolNoByteStreamErrorsAcriveOnly", **kwargs) :
    kwargs.setdefault("ActiveOnly",        True)
    kwargs.setdefault("UseByteStreamFEI4", False)
    kwargs.setdefault("UseByteStreamFEI3", False)
    kwargs.setdefault("UseByteStreamRD53", False)
    return PixelConditionsSummaryCfg(flags, name, **kwargs)

def PixelActiveDetectorElementStatusToolCfg(flags,name = "PixelActiveDetectorElementStatusTool",**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("IsActiveStatus", [ 'OK', 'WARNING', 'ERROR', 'FATAL' ] )
    acc.setPrivateTools( CompFactory.PixelActiveDetectorElementStatusTool(name, **kwargs) )
    return acc

def PixelByteStreamErrorDetectorElementStatusToolCfg(flags, name = "PixelByteStreamErrorDetectorElementStatusTool",**kwargs) :
    acc = ComponentAccumulator()
    if not flags.Input.isMC and not flags.Overlay.DataOverlay and flags.Input.Format is Format.BS :
        kwargs.setdefault("PixelByteStreamErrs", "PixelByteStreamErrs")
        kwargs.setdefault("UseByteStreamFEI4",   True)
        kwargs.setdefault("UseByteStreamFEI3",   True)
        kwargs.setdefault("ActiveOnly",          False)
        acc.setPrivateTools( CompFactory.PixelByteStreamErrorDetectorElementStatusTool(name, **kwargs) )
    else :
        kwargs.pop("ActiveOnly",False)
        acc.setPrivateTools( CompFactory.PixelDetectorElementStatusCloneTool(name, **kwargs) )
    return acc

def PixelByteStreamErrorDetectorElementStatusToolActiveOnlyCfg(flags, name = "PixelByteStreamErrorDetectorElementStatusToolActiveOnly",**kwargs) :
    kwargs.setdefault("ActiveOnly", True)
    return PixelByteStreamErrorDetectorElementStatusToolCfg(flags, name, **kwargs )
