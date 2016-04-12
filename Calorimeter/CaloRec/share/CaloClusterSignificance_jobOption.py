# File: CaloRec/share/CaloClusterSignificance_jobOption.py
# Created: October 2012, S.Menke 
# Purpose: Python fragment to enable non-standard moments 

# enable cluster moments which are off by default used for rapidity gap anlyses
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence.CaloTopoCluster.TopoMoments.MomentsNames += ["SIGNIFICANCE","CELL_SIGNIFICANCE","CELL_SIG_SAMPLING"]
