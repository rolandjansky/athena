# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwGap_v5.py
# Created: March 2012, SL
# Purpose: Gap correction, v5
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
#

from CaloClusterCorrection.constants import CALOCORR_COMBINED2

#############################################################################

CaloSwGap_v5 = [
         # escale  wgapSci  eoffset
[ 1.4125, 1.20193, 1.01119, 2722.8 ],
[ 1.4375, 1.21213, 1.10074, 3183.27 ],
[ 1.4625, 1.31096, 1.10145, 4988.24 ],
[ 1.4875, 1.21362, 1.15177, 8396.53 ],
[ 1.5125, 1.03755, 1.2089, 9984.47 ],
[ 1.5375, 0.995305, 1.00681, 4996.25 ],
[ 1.5625, 1.02523, 0.843151, 2694.72 ],
[ 1.5875, 1.0317, 0.841647, 2499.79 ],
]


#############################################################################


class CaloSwGap_v5_parms:
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
        correctionGoodPhi[_k] = CaloSwGap_v5
        correctionBadPhi[_k] = CaloSwGap_v5

