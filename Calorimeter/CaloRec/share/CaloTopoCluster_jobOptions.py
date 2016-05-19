#
# $Id: CaloTopoCluster_jobOptions.py,v 1.48 2008-09-12 09:08:56 menke Exp $
#
# File: CaloRec/share/CaloTopoCluster_jobOptions.py
# Created: June 2007, S.Menke
# Purpose: Define default calibrated topo cluster algo and corrections
#

try:
    from CaloRec.CaloClusterTopoGetter import CaloClusterTopoGetter
    CaloClusterTopoGetter()
    #from CaloRec.CaloCell2ClusterMapperGetters import CaloCell2TopoClusterMapperGetter
    #CaloCell2TopoClusterMapperGetter()
except Exception:
    from AthenaCommon.Resilience import treatException
    treatException("Problem with CaloTopoCluster. Switched off")

