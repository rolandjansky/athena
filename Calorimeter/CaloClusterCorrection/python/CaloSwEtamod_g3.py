# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloSwEtamod_g3.py,v 1.2 2006-11-16 07:07:45 ssnyder Exp $
#
# File: CaloClusterCorrection/python/CaloSwEtamod_g3.py
# Created: Nov 2006, sss
# Purpose: Eta modulation corrections, original G3 version.
#
# This is the original G3-based eta modulation correction, translated from the
# original fortran version.
#


class CaloSwEtamod_g3_parms:
    eta_size = 3
    correction = [
        [ [1.0052, -1.39, 56.0],     # barrel
          [1.0005, -0.19,  7.0],
          [1.0001, -0.137, 6.4] ],

        [ [1.011,  -2.2,  75.0],     # endcap
          [1.0045, -0.7,  22.0],
          [1.0027, -0.37,  9.0] ]
        ]
