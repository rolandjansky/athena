#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":
    infile = 'esd/ESD-15.6.7-data.pool.root'

    keys = [
        #LArDigitContainer_p1
        'LArDigitContainer_EMClust',
        'LArDigitContainer_Thinned',

        #LArNoisyROSummary_p1
        'LArNoisyROSummary',
    ]

    TPCnvTest(infile, keys, useGeoModelSvc=True)
