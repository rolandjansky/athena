#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-20.1.7.2.pool.root'

    keys = [
        #CaloClusterCellLinkContainer
        'InDetTrackParticlesAssociatedClusters_links@10',
        'MuonClusterCollection_links@10',
        'LArClusterEM_links@10',
        'egammaClusters_links@10',
        'LArClusterEM7_11Nocorr_links@10',
        'CaloCalTopoClusters_links@10',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doLAr=True, doTile=True)
