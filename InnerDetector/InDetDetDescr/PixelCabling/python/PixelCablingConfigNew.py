"""Define a method to construct configured PixelCablingSvc

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def PixelCablingSvcCfg(flags, name="PixelCablingSvc", **kwargs):
    """Return a ComponentAccumulator with configured PixelCablingSvc"""
    acc = ComponentAccumulator()
    acc.addService(CompFactory.PixelCablingSvc(name, **kwargs))
    return acc
