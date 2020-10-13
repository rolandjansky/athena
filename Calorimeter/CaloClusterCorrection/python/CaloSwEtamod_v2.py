# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwEtamod_v2.py
# Created: Nov 2006, sss
# Purpose: Eta modulation corrections, v2.
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


etamod_v3_correction = [
        [0.9988, 0.3031, -12.0],
        [0.9984, 0.356,  -13.55],
        [0.9987, 0.2368, -7.566]
    ]


# Version which goes before the gross energy corrections.
class CaloSwEtamod_v2_parms:
    region = CALOCORR_COMBINED2
    energies = CaloSw_sample_energies
    energy_degree = 2
    rfac = CaloSwEtaphimod_v2_rfac
    rfac_degree = 2
    correction = etamod_v3_correction


# Version which goes after the gross energy corrections.
class CaloSwEtamod_v3_parms:
    region = CALOCORR_COMBINED2
    energies = CaloSw_sample_energies
    energy_degree = 2
    rfac = [ [0, 1], [3, 1] ]
    rfac_degree = 1
    correction = etamod_v3_correction
