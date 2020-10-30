# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwPhimod_v2.py
# Created: Nov 2006, sss
# Purpose: Phi modulation corrections, v2 and v3.
#
# These corrections were derived using DC2 MC files reconstructed
# with 8.x.0.  Electrons at 50, 100, and 200 GeV were used.
# Derived for 5x5 clusters only.
#
# This was added in LArClusterRec-02-05-25, in 9.0.3.
#
# Two versions of this correction are exported.
# The original (v2) version of this correction ran before the
# gross energy correction.  Therefore, when the energy interpolation
# was performed, the energy was scaled with a crude eta-dependent
# factor.  For v3 of the corrections, the modulation corrections
# were moved to after the gross energy corrections.  Accordingly,
# this crude energy correction was turned off (by setting the
# correction factor to unity).
#
# v3 of the correction was in CaloClusterCorrection-00-02-58, in 12.0.3.
#


from CaloClusterCorrection.constants import CALOCORR_COMBINED2
from AthenaCommon.SystemOfUnits import GeV
from CaloClusterCorrection.CaloSwEtaphimod_v2_rfac import CaloSwEtaphimod_v2_rfac

# Energies at which corrections were tabulated.
CaloSw_sample_energies = [50*GeV, 100*GeV, 200*GeV]


phimod_v2_correction = [
    [
    [0.025,   2.664719e+00,  -1.977650e+00,  -3.860338e-01,  -2.407169e-01],
    [0.125,   1.628531e+00,  -3.030055e+00,  -9.809708e-02,   1.100109e-01],
    [  0.3,   1.070846e+00,  -3.488764e+00,  -3.307523e-02,  -4.116306e-01],
    [  0.5,   1.274397e+00,  -3.145406e+00,   3.300825e-01,  -2.691256e-01],
    [0.675,   1.172382e+00,  -2.858006e+00,  -6.314742e-01,  -3.864474e-01],
    [0.775,   1.550877e+00,  -4.643438e-01,  -6.363973e-01,   1.945537e+00],
    [0.825,   3.621207e+00,   8.322764e-01,   3.552365e-01,   1.096855e+00],
    [0.925,   2.283281e+00,  -3.008672e-02,  -7.406917e-01,  -6.440943e-01],
    [  1.1,   1.720161e+00,  -3.120961e+00,  -1.994254e-01,  -2.832045e+00],
    [ 1.25,   1.034721e+00,  -1.757163e+00,   7.509211e-01,  -3.867221e+00],
    [ 1.35,   4.329657e+00,  -2.672754e+00,  -4.513718e-01,  -9.722191e-01],
    [ 1.55,   3.450384e+00,  -9.897880e-01,  -6.906309e-01,   4.891501e-01],
    [ 1.65,   5.234329e+00,  -6.979198e-01,   1.177628e-01,   1.328640e+00],
    [ 1.75,   1.716329e+00,  -1.553104e+00,  -3.667783e+00,   9.834792e-01],
    [  1.9,   1.443694e+00,  -3.952973e+00,   2.176944e-01,  -4.774538e-01],
    [  2.1,   1.138740e+00,  -4.082930e+00,   5.088291e-02,   1.270251e+00],
    [  2.3,   1.456033e+00,  -2.812821e+00,   2.295804e-01,   3.475075e-01],
    [ 2.45,   4.889527e+00,  -4.555045e-01,  -7.241840e-02,  -3.709562e-01],
    ],

    [
    [0.025,   1.971881e+00,  -2.108978e+00,  -3.812373e-03,   6.974836e-01],
    [0.125,   1.634289e+00,  -4.802683e+00,  -2.846689e+00,  -3.716865e-01],
    [  0.3,   1.735753e+00,  -2.476407e+00,  -9.856527e-01,  -3.294847e-01],
    [  0.5,   1.274812e+00,  -3.450919e+00,  -3.290327e-01,   1.603592e-01],
    [0.675,   1.893247e+00,  -3.282441e+00,   6.283741e-01,   7.038055e-02],
    [0.775,   1.988917e+00,  -7.072864e-01,  -2.910389e-01,  -2.737981e+00],
    [0.825,   4.251496e+00,  -3.454414e+00,  -8.634848e-02,  -3.290181e-01],
    [0.925,   2.258372e+00,  -3.853058e+00,   1.114267e+00,   2.894483e-01],
    [  1.1,   1.289441e+00,  -2.040407e+00,  -2.569965e-01,  -1.781131e+00],
    [ 1.25,   2.095391e+00,   1.017069e+00,  -2.391279e-01,  -3.097476e-01],
    [ 1.35,   1.660031e+00,   2.256689e+00,   3.140057e-02,  -1.770399e+00],
    [ 1.55,   5.021190e+00,  -8.969750e-01,   5.353170e-01,  -1.477288e+00],
    [ 1.65,   6.578303e+00,  -1.292578e-01,   1.996386e+00,  -9.525331e-01],
    [ 1.75,   3.873153e+00,   1.039520e-01,   7.117752e-01,  -3.212314e+00],
    [  1.9,   1.118655e+00,  -1.946691e+00,   8.969255e-01,  -3.606561e+00],
    [  2.1,   1.920745e+00,  -2.770727e+00,   7.815196e-01,  -8.528697e-01],
    [  2.3,   9.550326e-01,  -1.962016e+00,   1.737935e-01,   7.070875e-01],
    [ 2.45,   5.344424e+00,  -6.362199e-02,   8.217396e-01,   1.365029e+00],
    ],

    [
    [0.025,   3.859011e+00,   2.620800e+00,  -4.940401e-01,   3.226650e-01],
    [0.125,   2.525576e+00,   3.333473e+00,  -1.383994e+00,   5.264736e-02],
    [  0.3,   2.648994e+00,   3.443790e+00,  -1.914856e+00,  -7.568537e-04],
    [  0.5,   2.355716e+00,   3.254760e+00,  -7.351603e-01,  -3.398604e-01],
    [0.675,   1.923320e+00,   3.301835e+00,  -6.642117e-02,  -5.247854e-01],
    [0.775,   2.437411e+00,   4.264660e+00,  -4.325367e-01,   1.267180e+00],
    [0.825,   1.852553e+00,   3.640325e+00,   3.757975e-01,   3.053511e-01],
    [0.925,   9.542042e-01,   2.658316e+00,  -2.140545e-01,  -5.922291e-01],
    [  1.1,   8.494119e-01,   2.408077e+00,   1.095697e+00,  -1.959227e+00],
    [ 1.25,   2.773621e+00,   2.491694e+00,   3.822403e-01,  -5.126685e-01],
    [ 1.35,   1.301051e+00,   2.690086e+00,   2.505080e+00,   2.026869e+00],
    [ 1.55,   3.878504e+00,  -1.331661e-01,   3.038069e-01,  -2.261385e+00],
    [ 1.65,   3.883578e+00,   1.900214e-01,   5.729534e-01,   3.521348e-01],
    [ 1.75,   2.804249e+00,   2.053093e-01,  -8.386892e-01,  -1.587206e-02],
    [  1.9,   1.895993e+00,   2.493088e+00,   1.597364e-01,  -1.215671e+00],
    [  2.1,   3.079865e+00,   3.069515e+00,   1.932418e+00,  -3.497717e-01],
    [  2.3,   1.353233e+00,   5.788015e+00,  -9.021981e-01,  -2.239381e-02],
    [ 2.45,   6.917990e+00,   6.746056e+00,   9.872459e-01,  -4.001087e-01],
    ]
    ]


# Version which goes before the gross energy corrections.
class CaloSwPhimod_v2_parms:
    region = CALOCORR_COMBINED2
    correction_coef = 0.001
    interp_barriers = [1.5]
    degree = 3
    energies = CaloSw_sample_energies
    energy_degree = 2
    rfac = CaloSwEtaphimod_v2_rfac
    rfac_degree = 2
    use_raw_eta = False
    correction = phimod_v2_correction


# Version which goes after the gross energy corrections.
class CaloSwPhimod_v3_parms:
    region = CALOCORR_COMBINED2
    correction_coef = 0.001
    interp_barriers = [1.5]
    degree = 3
    energies = CaloSw_sample_energies
    energy_degree = 2
    rfac = [ [0, 1], [3, 1] ]
    rfac_degree = 1
    use_raw_eta = False
    correction = phimod_v2_correction

