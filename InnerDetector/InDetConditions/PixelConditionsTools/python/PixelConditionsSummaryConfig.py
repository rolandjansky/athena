"""Define methods to configure PixelConditionsSummary

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline
from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelTDAQCondAlg
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
from PixelConditionsTools.PixelDCSConditionsConfig import PixelDCSConditionsCfg

def PixelConditionsSummaryCfg(flags, name="PixelConditionsSummary", **kwargs):
    """Return configured ComponentAccumulator and tool for PixelDCSConditions"""
    acc = ComponentAccumulator()
    kwargs.setdefault("UseDCSState", False)
    kwargs.setdefault("UseByteStream", False)
    kwargs.setdefault("UseTDAQ", False)
    kwargs.setdefault("UseDeadMap", True)
    PixelDeadMapFolder = "/PIXEL/PixMapOverlay"
    if kwargs["UseDCSState"]:
        acc.merge(PixelDCSConditionsCfg(flags, DCSConditionsTool=DCSTool))
    if kwargs["UseTDAQ"]:
        PixelTDAQFolder = "/TDAQ/Resources/ATLAS/PIXEL/Modules"
        acc.merge(addFolders(flags, PixelTDAQFolder, "TDAQ_ONL", "CondAttrListCollection"))
        acc.addCondAlgo(PixelTDAQCondAlg(name="PixelTDAQCondAlg", ReadKey=PixelTDAQFolder))
    if kwargs["UseDeadMap"]:
        acc.merge(addFolders(flags, PixelDeadMapFolder, "PIXEL_OFL","CondAttrListCollection"))
        acc.merge(addFoldersSplitOnline(flags, "PIXEL", "/PIXEL/Onl/PixMapOverlay",
                                        "/PIXEL/PixMapOverlay", "CondAttrListCollection"))
    acc.addCondAlgo(PixelConfigCondAlg(name=name + "CondAlg", 
                                       UseDeadMap=kwargs["UseDeadMap"],
                                       ReadDeadMapKey=PixelDeadMapFolder))
    tool = PixelConditionsSummaryTool(name=name + "Tool", **kwargs)
    return acc, tool

