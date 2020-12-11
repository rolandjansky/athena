#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-18.0.0.pool.root'

    keys = [
        #CaloCellLinkContainer_p2
        'LArClusterEMFrwd_Link',
        'LArClusterEM_Link',
        'egClusterCollection_Link',
        'EMTopoSW35_Link',
        'MuonClusterCollection_Link',
        'LArClusterEM7_11Nocorr_Link',
        'LArClusterEMSofte_Link',
        'Tau1P3PCellEM012ClusterContainer_Link',
        'Tau1P3PCellCluster_Link',
        'TauPi0BonnClusterContainer_Link',
        'HLT_TrigCaloClusterMaker_topo_Link',
        'CaloCalTopoCluster_Link',

        #CaloClusterContainer_p7
        'HLT@10',
        'LArClusterEMFrwd@10',
        'MuonClusterCollection@10',
        'Tau1P3PCellEM012ClusterContainer@10',
        'Tau1P3PCellCluster@10',
        'HLT_TrigT2Calo@10',
        'HLT_TrigCaloClusterMaker_slw@10',
        'LArClusterEMSofte@10',
        'LArClusterEM7_11Nocorr@10',
        'LArClusterEM@10',
        'egClusterCollection@10',
        'TauPi0BonnClusterContainer@10',
        'EMTopoSW35@10',
        'HLT_TrigCaloClusterMaker_topo@10',
        'HLT_TrigCaloClusterMaker@10',
        'CaloCalTopoCluster@10',

        #CaloShowerContainer_p2
        'HLT@10',
        'MuonClusterCollection_Data@10',
        'LArClusterEM_Data@10',
        'LArClusterEM7_11Nocorr_Data@10',
        'egClusterCollection_Data@10',
        'Tau1P3PCellEM012ClusterContainer_Data@10',
        'LArClusterEMSofte_Data@10',
        'EMTopoSW35_Data@10',
        'Tau1P3PCellCluster_Data@10',
        'TauPi0BonnClusterContainer_Data@10',
        'CaloCalTopoCluster_Data@10',

        #CaloTopoTowerContainer_p1
        'TopoTower',

        #CaloTowerContainer_p1
        'CombinedTower',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doLAr=True, doTile=True)
