"""Define functions to configure Pixel conditions algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelITkOfflineCalibCondAlg


def PixelITkOfflineCalibCondAlgCfg(flags, name="PixelITkOfflineCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelITkOfflineCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/PIXEL/ITkClusterError", "PIXEL_OFL", className="CondAttrListCollection"))
    kwargs.setdefault("ReadKey", "/PIXEL/ITkClusterError")
    kwargs.setdefault("WriteKey", "PixelITkOfflineCalibData")
    kwargs.setdefault("InputSource", 2)
    acc.addCondAlgo(PixelITkOfflineCalibCondAlg(name, **kwargs))
    return acc
