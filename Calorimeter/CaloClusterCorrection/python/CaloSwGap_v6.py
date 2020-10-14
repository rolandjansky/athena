# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwGap_v6.py
# Created: March 2012, SL
# Purpose: Gap correction, v6
#
# New gap corrections
# - derived from rel17 single electron MC samples
# - crack definition now matching with calibration hits definition (i.e.
#   use eta in sampling 2 in calo frame and use 1.425<eta(s2-calo)<1.55)
# - correction is applied as a function of cluster eta in the ATLAS frame:
#   due to misalignments, the corrections are provided for a larger
#   crack definition than mentioned above
# - use a single correction for all cluster sizes
# - for now, use the electron corrections for the photons as well
# - make two phi binning: "good phi" regions without TG3 cells missing
#                         "bad phi" regions with TG3 cells missing
#

from CaloClusterCorrection.constants import CALOCORR_COMBINED2

#############################################################################

CaloSwGap_v6_goodphi = [
         # escale  wgapSci  eoffset
[ 1.4125, 1.19207, 1.11653, 2909.22 ],
[ 1.4375, 1.19698, 1.16545, 3374.8 ],
[ 1.4625, 1.28962, 1.15927, 4881.07 ],
[ 1.4875, 1.15101, 1.33242, 8031.85 ],
[ 1.5125, 0.952019, 1.57746, 8727.48 ],
[ 1.5375, 0.992051, 1.03874, 4212.3 ],
[ 1.5625, 1.02, 0.91491, 2140.95 ],
[ 1.5875, 1.02813, 0.90897, 1993.07 ]
]

CaloSwGap_v6_badphi = [
         # escale  wgapSci  eoffset
[ 1.4125, 1.22272, 2.07331, 2056.31 ],
[ 1.4375, 1.26355, 1.62687, 2318.94 ],
[ 1.4625, 1.61749, 1.4683, 2376.19 ],
[ 1.4875, 1.74078, 0.875144, 5229.63 ],
[ 1.5125, 1.28008, 1.70207, 9373.12 ],
[ 1.5375, 1.06362, 1.50708, 7612.6 ],
[ 1.5625, 1.04044, 1.1965, 4072.75 ],
[ 1.5875, 1.04594, 1.22732, 3503.04 ]
]


#############################################################################


class CaloSwGap_v6_parms:
    _all_keys = ['ele55', 'ele35', 'ele37', 'ele33', 'ele57', 'ele77',
                 'gam55', 'gam35', 'gam37', 'gam33', 'gam57', 'gam77']

    correctionGoodPhi = {}
    correctionBadPhi = {}

    etamin_crack = 1.425
    etamax_crack = 1.55
    degree = 3
    use_raw_eta = False
    use_raw_eta_boundaries = True
    region = CALOCORR_COMBINED2

    # Use new constants, same for all cluster sizes, same for electrons and photons.
    # Take position from layer 2; use raw eta boundaries.
    for _k in _all_keys:
        correctionGoodPhi[_k] = CaloSwGap_v6_goodphi
        correctionBadPhi[_k] = CaloSwGap_v6_badphi

