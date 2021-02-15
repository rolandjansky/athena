#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-15.6.4.pool.root'

    keys = [
        #TileL2Container_p1
        'TileL2Cnt',

        #TileMuContainer_p1
        'TileMuObj',

        #TileTTL1Container_p1
        'TileTTL1MBTS',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True)


