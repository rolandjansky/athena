#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'rtt:valid1.110401.PowhegPythia_P2012_ttbar_nonallhad.simul.HITS.e3099_s2578_tid04919495_00/HITS.04919495._000416.pool.root.1'

    keys = [
        #LArHitContainer_p2
        'LArHitHEC@100',
        'LArHitFCAL@100',
        'LArHitEMB@100',
        'LArHitEMEC@100',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True)
