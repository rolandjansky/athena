"""Define functions to configure Pixel conditions algorithms

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelITkOfflineCalibCondAlg


def PixelITkOfflineCalibCondAlgCfg(flags, name="PixelITkOfflineCalibCondAlg", **kwargs):
    """Return a ComponentAccumulator with configured PixelITkOfflineCalibCondAlg"""
    acc = ComponentAccumulator()
    acc.merge(addFolders(flags, "/PIXEL/PixReco", "PIXEL_OFL", className="DetCondCFloat"))
    kwargs.setdefault("ReadKey", "/PIXEL/PixReco")
    kwargs.setdefault("InputSource", 2)
    acc.addCondAlgo(PixelITkOfflineCalibCondAlg(name, **kwargs))
    return acc
