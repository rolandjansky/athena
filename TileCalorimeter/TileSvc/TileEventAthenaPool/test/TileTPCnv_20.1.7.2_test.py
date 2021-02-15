#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-20.1.7.2.pool.root'

    keys = [
        #TileDigitsContainer_p1
        'TileDigitsFlt',

        #TileL2Container_p2
        'TileL2Cnt',

        #TileMuContainer_p1
        'TileMuObj',

        #TileTTL1Container_p1
        'TileTTL1MBTS',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True, doTile=True)


