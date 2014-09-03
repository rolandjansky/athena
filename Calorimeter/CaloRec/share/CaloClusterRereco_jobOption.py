# File: CaloRec/share/CaloClusterRereco_jobOption.py
# Created: October 2012, S.Menke 
# Purpose: Python fragment to turn on re-reconstruction of topo
# clusters on LC scale. To be run on ESDs

include("RecExCond/AllDet_detDescr.py")

## configure object key store to recognize calo cells
from RecExConfig.ObjKeyStore import ObjKeyStore, objKeyStore
oks = objKeyStore
oks.addStreamESD('CaloCellContainer', ['AllCalo'] )

## re-do topo clusters on LC scale
from CaloRec.CaloTopoClusterFlags import jobproperties
jobproperties.CaloTopoClusterFlags.doTopoClusterLocalCalib = True
from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter
CaloClusterTopoGetter()
