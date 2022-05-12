# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of BCM_ZeroSuppression package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def BCM_ZeroSuppressionCfg(flags, name="InDetBCM_ZeroSuppression", **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")

    acc = ComponentAccumulator()
    kwargs.setdefault("BcmContainerName", "BCM_RDOs")
    algo = CompFactory.BCM_ZeroSuppression(name, **kwargs)
    acc.addEventAlgo(algo, primary = True)
    return acc
