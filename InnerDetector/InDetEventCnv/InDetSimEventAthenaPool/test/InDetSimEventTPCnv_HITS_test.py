#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'rtt:valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.simul.HITS.e3099_s2578_tid04919495_00/HITS.04919495._000416.pool.root.1'

    keys = [
        #SiHitCollection_p2
        'BLMHits',
        'BCMHits',
        'PixelHits@100',
        'SCT_Hits@100',
        #TRT_HitCollection_p3
        'TRTUncompressedHits@100',
    ]

    TPCnvTest(infile, keys)
