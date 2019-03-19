"""Define methods to configure PixelDCSConditions

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg

def PixelDCSConditionsCfg(flags, name="PixelDCSCond", **kwargs):
    """Return configured ComponentAccumulator and tool for PixelDCSConditions"""
    acc = ComponentAccumulator()
    kwargs.setdefault("UseConditions", True)
    if flags.Common.isOnline:
        hvFolder = "/PIXEL/HLT/DCS/HV"
        tempFolder = "/PIXEL/HLT/DCS/TEMPERATURE"
        dbInstance = "PIXEL_ONL"
    else:
        hvFolder = "/PIXEL/DCS/HV"
        tempFolder = "/PIXEL/DCS/TEMPERATURE"
        dbInstance = "DCS_OFL"
    folders = [hvFolder, tempFolder]
    if not flags.Input.isMC:
        folders.extend(["/PIXEL/DCS/FSMSTATE", "/PIXEL/DCS/FSMSTATUS"])
        acc.addCondAlgo(PixelDCSCondStateAlg(name=name + "StateAlg"))
    acc.merge(addFolders(flags, folders, dbInstance, "CondAttrListCollection"))
    acc.addCondAlgo(PixelDCSCondHVAlg(name=name + "HVAlg", ReadKey=hvFolder))
    acc.addCondAlgo(PixelDCSCondTempAlg(name=name + "TempAlg", ReadKey=tempFolder))
    return acc

