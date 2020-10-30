# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwPhioff_v2.py
# Created: Nov 2006, sss
# Purpose: Phi position corrections, v2.
#
# These corrections were derived using DC2 MC files reconstructed
# with 8.x.0.  Electrons at 50, 100, and 200 GeV were used.
# Derived for 5x5 clusters only.
#
# This was added in LArClusterRec-02-05-12, in 8.6.0.
#

from CaloClusterCorrection.constants import \
     CALOCORR_EMB2, CALOCORR_EME2, EMB2, EME2
from AthenaCommon.SystemOfUnits import GeV
import math


# Energies at which corrections were tabulated.
CaloSw_sample_energies = [50*GeV, 100*GeV, 200*GeV]


#############################################################################
# barrel phi position correction for sampling 2
#
class CaloSwPhioff_v2_b_parms:
    region = CALOCORR_EMB2
    correction_coef = 0.001
    interp_barriers = [0.8]
    degree = 3
    flip_phi = 1
    energies = CaloSw_sample_energies
    energy_degree = 2
    correction = [
        [
        [ 0.05,   6.368485e-01],
        [ 0.15,   6.650752e-01],
        [ 0.25,   7.123238e-01],
        [ 0.35,   8.099820e-01],
        [ 0.45,   9.070935e-01],
        [ 0.55,   9.354902e-01],
        [ 0.65,   9.148351e-01],
        [ 0.75,   9.467127e-01],
        [ 0.85,  -5.690570e-01],
        [ 0.95,  -6.839550e-01],
        [ 1.05,  -8.904216e-01],
        [ 1.15,  -1.011007e+00],
        [ 1.25,  -1.295527e+00],
        [ 1.35,  -1.119071e+00],
        [ 1.45,  -1.491369e+00],
        ],

        [
        [ 0.05,   5.108382e-01],
        [ 0.15,   5.200147e-01],
        [ 0.25,   5.378089e-01],
        [ 0.35,   6.997061e-01],
        [ 0.45,   7.876812e-01],
        [ 0.55,   8.096306e-01],
        [ 0.65,   8.216495e-01],
        [ 0.75,   8.864841e-01],
        [ 0.85,  -4.780463e-01],
        [ 0.95,  -5.635048e-01],
        [ 1.05,  -7.141002e-01],
        [ 1.15,  -8.910146e-01],
        [ 1.25,  -1.158082e+00],
        [ 1.35,  -1.123030e+00],
        [ 1.45,  -1.179254e+00],
        ],

        [
        [ 0.05,   4.046394e-01],
        [ 0.15,   4.084462e-01],
        [ 0.25,   4.279229e-01],
        [ 0.35,   5.190943e-01],
        [ 0.45,   6.200826e-01],
        [ 0.55,   6.732781e-01],
        [ 0.65,   7.164665e-01],
        [ 0.75,   7.926999e-01],
        [ 0.85,  -3.220841e-01],
        [ 0.95,  -4.310168e-01],
        [ 1.05,  -5.525186e-01],
        [ 1.15,  -7.620540e-01],
        [ 1.25,  -1.023037e+00],
        [ 1.35,  -1.033757e+00],
        [ 1.45,  -2.017682e+00],
        ],
        ]


#############################################################################
# endcap phi position correction for sampling 2
#
class CaloSwPhioff_v2_e_parms:
    region = CALOCORR_EME2
    correction_coef = 0.001
    interp_barriers = [2.3]
    degree = 3
    flip_phi = 1
    energies = CaloSw_sample_energies
    energy_degree = 2
    # Note: previous geometry had an erroneous 4.1 mrad offset in phi in ECEM
    # between generator and reco.  This has been fixed since this correction
    # was last derived; fixed up for now by shifting all by 4.1 mrad.
    _phi_offset = 2*math.pi/256/6*1000
    correction = [
        [
        [ 1.45,  -2.390531+_phi_offset],
        [ 1.55,  -4.592782+_phi_offset],
        [ 1.65,  -4.707693+_phi_offset],
        [ 1.75,  -4.613757+_phi_offset],
        [ 1.85,  -4.714106+_phi_offset],
        [ 1.95,  -4.612117+_phi_offset],
        [ 2.05,  -4.719658+_phi_offset],
        [ 2.15,  -4.770102+_phi_offset],
        [ 2.25,  -4.969137+_phi_offset],
        [ 2.35,  -3.498540+_phi_offset],
        [ 2.45,  -3.330409+_phi_offset],
        ],
    
        [
        [ 1.45,  -3.832055+_phi_offset],
        [ 1.55,  -4.514931+_phi_offset],
        [ 1.65,  -4.553897+_phi_offset],
        [ 1.75,  -4.531036+_phi_offset],
        [ 1.85,  -4.642853+_phi_offset],
        [ 1.95,  -4.626890+_phi_offset],
        [ 2.05,  -4.679615+_phi_offset],
        [ 2.15,  -4.700794+_phi_offset],
        [ 2.25,  -4.765519+_phi_offset],
        [ 2.35,  -3.645206+_phi_offset],
        [ 2.45,  -3.462443+_phi_offset],
        ],
    
        [
        [ 1.45,  -3.510859+_phi_offset],
        [ 1.55,  -4.492770+_phi_offset],
        [ 1.65,  -4.534856+_phi_offset],
        [ 1.75,  -4.529166+_phi_offset],
        [ 1.85,  -4.555026+_phi_offset],
        [ 1.95,  -4.560039+_phi_offset],
        [ 2.05,  -4.625474+_phi_offset],
        [ 2.15,  -4.673238+_phi_offset],
        [ 2.25,  -4.724827+_phi_offset],
        [ 2.35,  -3.778890+_phi_offset],
        [ 2.45,  -3.618776+_phi_offset],
        ],
        ]




CaloSwPhioff_v2_parms = { EMB2 : CaloSwPhioff_v2_b_parms,
                          EME2 : CaloSwPhioff_v2_e_parms,
                          }


