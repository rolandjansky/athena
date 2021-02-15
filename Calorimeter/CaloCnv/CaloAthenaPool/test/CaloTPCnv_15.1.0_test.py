#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'aod/AOD-15.1.0/AOD-15.1.0-full.pool.root'

    keys = [
        #CaloCellLinkContainer_p2
        'MuonClusterCollection_Link',
        'egClusterCollection_Link',

        #CaloClusterContainer_p5
        'HLT@10',
        'Tau1P3PCellEM012ClusterContainer@10',
        'Tau1P3PCellCluster@10',
        'Tau1P3PPi0ClusterContainer@10',
        'LArClusterEMFrwd@10',
        'MuonClusterCollection@10',
        'egClusterCollection@10',
        'LArClusterEMSofte@10',
        'EMTopoCluster430@10',
        'HLT_TrigCaloClusterMaker@10',
        'CaloCalTopoCluster@10',

        #CaloShowerContainer_p2
        'HLT',

        #CaloCompactCellContainer
        #'AODCellContainer',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True)
