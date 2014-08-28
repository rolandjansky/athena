# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CaloClusterCorrection.CaloClusterCorrectionConf import CaloFillRectangularCluster as cfrc
from RecExConfig.RecFlags                            import rec

def CaloFillRect():
    from AthenaCommon.AppMgr import ToolSvc

    if rec.readESD():
        CellsSGKey = D3PDMakerFlags.CellsSGKey()
    else:
        CellsSGKey = "AODCellContainer"
        

    if not hasattr(ToolSvc, "PAU_CaloFillRectangularCluster"):
        caloFillRect = cfrc(name = "PAU_CaloFillRectangularCluster",
                            eta_size = 5,
                            phi_size = 7,
                            cells_name = CellsSGKey)
        ToolSvc += caloFillRect
        return caloFillRect
    else:
        return ToolSvc.PAU_CaloFillRectangularCluster
