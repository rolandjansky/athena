"""Define a function to configure PixelConditionsSummaryCfg

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline, addFolders
PixelConditionsSummaryTool=CompFactory.PixelConditionsSummaryTool
PixelTDAQCondAlg=CompFactory.PixelTDAQCondAlg
from PixelConditionsTools.PixelDCSConditionsConfig import PixelDCSConditionsCfg
from PixelConditionsAlgorithms.PixelConditionsConfig import PixelConfigCondAlgCfg

def PixelConditionsSummaryCfg(flags, name="PixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator with tool for Pixel Conditions"""
    acc = ComponentAccumulator()
    kwargs.setdefault("UseDCSState", False)
    kwargs.setdefault("UseByteStream", False)
    kwargs.setdefault("UseTDAQ", False)
    kwargs.setdefault("UseDeadMap", True)
    PixelDeadMapFolder = "/PIXEL/PixMapOverlay"
    if kwargs["UseDCSState"]:
        acc.merge(PixelDCSConditionsCfg(flags))
    if kwargs["UseTDAQ"]:
        PixelTDAQFolder = "/TDAQ/Resources/ATLAS/PIXEL/Modules"
        acc.merge(addFolders(flags, PixelTDAQFolder, "TDAQ_ONL", "CondAttrListCollection"))
        acc.addCondAlgo(PixelTDAQCondAlg(name="PixelTDAQCondAlg", ReadKey=PixelTDAQFolder))
    if kwargs["UseDeadMap"]:
        acc.merge(addFolders(flags, PixelDeadMapFolder, "PIXEL_OFL","CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixMapOverlay",
                                        "/PIXEL/PixMapOverlay", "CondAttrListCollection"))
    acc.merge(PixelConfigCondAlgCfg(flags, UseDeadMap=kwargs["UseDeadMap"], ReadDeadMapKey=PixelDeadMapFolder))
    acc.setPrivateTools(PixelConditionsSummaryTool(name=name + "Tool", **kwargs))
    return acc

