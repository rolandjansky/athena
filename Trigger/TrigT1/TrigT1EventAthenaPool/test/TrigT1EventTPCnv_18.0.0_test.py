#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-18.0.0.pool.root'
    keys = [
        #CMMCPHitsCollection_tlp1
        'CMMCPHits',

        #CMMEtSumsCollection_tlp1
        'CMMEtSums',

        #CMMJetHitsCollection_tlp1
        'CMMJetHits',

        #CMMRoI_p1
        'CMMRoIs',

        #CPMHitsCollection_tlp1
        'CPMHits',

        #CPMRoICollection_tlp1
        'CPMRoIs',

        #CPMTowerCollection_tlp1
        'CPMTowers',

        #CTP_RDO_p2
        'CTP_RDO',

        #JEMEtSumsCollection_tlp1
        'JEMEtSums',

        #JEMHitsCollection_tlp1
        'JEMHits',

        #JEMRoICollection_tlp1
        'JEMRoIs',

        #JetElementCollection_tlp1
        'JetElements',

        #MuCTPI_RDO_p1
        'MUCTPI_RDO',

        #RoIBResult_p1
        'RoIBResult',

        #TriggerTowerCollection_tlp1
        'TriggerTowers',
    ]

    TPCnvTest(infile, keys)
