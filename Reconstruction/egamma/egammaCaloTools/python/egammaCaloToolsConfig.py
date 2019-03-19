# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Tool configuration to instantiate all egammaCaloTools with default configuration."

#---------------------------------------
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CaloFillRectangularClusterCfg(flags):
    result=ComponentAccumulator()

    from CaloClusterCorrection.CaloClusterCorrectionConf import CaloFillRectangularCluster
    caloFillRectangularCluster = CaloFillRectangularCluster(eta_size = 5,
                                                            phi_size = 7,
                                                            cells_name = flags.Egamma.Keys.Input.Cells)

    result.setPrivateTools(caloFillRectangularCluster)
    return result

#---------------------------------------

