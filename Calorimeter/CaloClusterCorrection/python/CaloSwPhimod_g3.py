# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: CaloSwPhimod_g3.py,v 1.1 2006-11-14 14:51:28 ssnyder Exp $
#
# File: CaloClusterCorrection/python/CaloSwPhimod_g3.py
# Created: Nov 2006, sss
# Purpose: Phi modulation corrections, original G3 version.
#
# This is the original G3-based phi modulation correction, translated from the
# original fortran version.
#


class CaloSwPhimod_g3_parms:
    correction = [
        [0.1, 0.9998,  0.0049,  0.0004,  0.0004,  0.0021],
        [0.3, 0.9942,  0.0060, -0.0001, -0.0001,  0.0026],
        [0.5, 0.9923,  0.0058, -0.0010, -0.0005,  0.0041],
        [0.7, 0.9942,  0.0055, -0.0005, -0.0006,  0.0026],
        [0.9, 0.9939,  0.0044,  0.0006,  0.0000, -0.0012],
        [1.2, 0.9878,  0.0026,  0.0000,  0.0003, -0.0021],
        [1.5, 1.0,     0.0,     0.0,     0.0,     0.0],
        [1.7, 0.9992, -0.0005, -0.0032,  0.0038, -0.0002],
        [1.8, 0.9966, -0.0038,  0.0004,  0.0033,  0.0001],
        [1.9, 0.9955, -0.0082,  0.0000,  0.0022,  0.0016],
        [2.0, 0.9935, -0.0115,  0.0026,  0.0003,  0.0015],
        [2.1, 0.9934, -0.0107,  0.0018,  0.0008,  0.0017],
        [2.2, 0.9938, -0.0095,  0.0003,  0.0010,  0.0009],
        [2.3, 0.9954, -0.0043,  0.0,     0.0013,  0.0005],
        [2.5, 1.0,     0.0,     0.0,     0.0,     0.0],
        ]
