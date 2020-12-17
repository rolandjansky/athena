#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'rtt:valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.simul.HITS.e3099_s2578_tid04919495_00/HITS.04919495._000416.pool.root.1'

    keys = [
        #Muon::CSCSimHitCollection_p2
        'CSC_Hits',

        #Muon::MDTSimHitCollection_p2
        'MDT_Hits',

        #Muon::RPCSimHitCollection_p2
        'RPC_Hits',

        #Muon::TGCSimHitCollection_p3
        'TGC_Hits',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doMuon=True)
