# jobOptions file for LAr Cluster Reconstruction
#  This should be use only after LArCluster_jobOptions.txt
#  It copies the LArClusterContainer (LArClusterEM, defaultsize=5x5)
#    and redo the corrections with size 3x5 and 3x7. 

from LArClusterRec.LArClusterSwGetters import *
LArClusterSwEle35Getter()
LArClusterSwGam35Getter()
LArClusterSwEle37Getter()
LArClusterSwGam37Getter()


