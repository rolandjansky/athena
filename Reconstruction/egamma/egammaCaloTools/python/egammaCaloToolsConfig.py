# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
Tool configuration to instantiate
all egammaCaloTools with default configuration.
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CaloFillRectangularClusterCfg(flags):
    result = ComponentAccumulator()

    CaloFillRectangularCluster = CompFactory.CaloFillRectangularCluster
    caloFillRectangularCluster = CaloFillRectangularCluster(
        eta_size=5,
        phi_size=7,
        cells_name=flags.Egamma.Keys.Input.CaloCells)

    result.setPrivateTools(caloFillRectangularCluster)
    return result
