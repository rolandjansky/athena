# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwPhioff_g3.py
# Created: Nov 2006, sss
# Purpose: Phi position corrections, original G3 version.
#
# This is the original G3-based phi offset correction, translated from the
# original fortran version.
#


from CaloClusterCorrection.constants import \
     CALOCORR_EMB2, CALOCORR_EME2, EMB2, EME2


#############################################################################
# barrel phi position correction for sampling 2
#
class CaloSwPhioff_g3_b_parms:
    region = CALOCORR_EMB2
    correction_coef = 0.001
    interp_barriers = [0.8]
    degree = 2
    flip_phi = 0
    energies = [0]
    energy_degree = 1
    correction = [
        [
        [0.05, 0.4424],
        [0.15, 0.4058],
        [0.25, 0.4753],
        [0.35, 0.7823],
        [0.45, 0.8636],
        [0.55, 0.9798],
        [0.65, 0.8224],
        [0.75, 0.7925],

        [0.85, -0.2889],
        [0.95, -0.3849],
        [1.05, -0.6478],
        [1.15, -0.9297],
        [1.25, -1.1424],
        [1.35, -1.1506],
        ]
        ]


#############################################################################
# endcap phi position correction for sampling 2
#
class CaloSwPhioff_g3_e_parms:
    region = CALOCORR_EME2
    correction_coef = 0.001
    interp_barriers = [2.3]
    degree = 2
    flip_phi = 0
    energies = [0]
    energy_degree = 1
    correction = [
        [
        [1.45, -2.02],
        [1.55, -0.57],
        [1.65, -0.5],
        [1.75, -0.37],
        [1.85, -0.36],
        [1.95, -0.23],
        [2.05, -0.53],
        [2.15, -0.46],
        [2.25, -0.54],

        [2.35,  0.18],
        [2.45,  0.35],
        ]
        ]




CaloSwPhioff_g3_parms = { EMB2 : CaloSwPhioff_g3_b_parms,
                          EME2 : CaloSwPhioff_g3_e_parms,
                          }


