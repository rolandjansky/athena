"""Define a method to construct configured PixelCablingSvc

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

from PixelConditionsAlgorithms.PixelConditionsConfig import (
    PixelCablingCondAlgCfg, PixelHitDiscCnfgAlgCfg, PixelReadoutSpeedAlgCfg
)

def PixelCablingSvcCfg(flags, name="PixelCablingSvc", **kwargs):
    """Return a ComponentAccumulator with configured PixelCablingSvc"""
    acc = ComponentAccumulator()
    acc.merge(PixelHitDiscCnfgAlgCfg(flags))
    acc.merge(PixelReadoutSpeedAlgCfg(flags))
    acc.merge(PixelCablingCondAlgCfg(flags))
    acc.addService(CompFactory.PixelCablingSvc(name, **kwargs))
    return acc
