# jobOptions file for LAr Cluster Reconstruction
# with full corrections.

# Backwards compatibility for old softe code.
from CaloClusterCorrection.CaloSwCorrections import CaloSwCorrections_compat
CaloSw_corrections = CaloSwCorrections_compat()

# Create 3x5 electron clusters
from LArClusterRec.LArClusterSwGetters import *
LArClusterSwEle35Getter()

# 7x11 clusters with no corrections.
# not built by default
#LArClusterSw711GetterNocorr()
