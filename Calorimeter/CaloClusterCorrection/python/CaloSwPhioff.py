# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwPhioff.py
# Created: Nov 2006, sss
# Purpose: Steering module for phi position corrections.
#
# Due to the accordion structure of the calorimeter, the reconstructed
# phi position may be slightly offset from the true position.  This bias
# depends on the average shower depth, and thus on the shower eta.
#
# The correction consists of a simple eta-dependent offset that is
# added to the phi position.  There's also a small energy dependence,
# so we also interpolate in energy.
#


from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_SW
from CaloClusterCorrection.common import makecorr

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.constants import sw_valid_keys as keys
cls = CompFactory.CaloSwPhioff_v2  # CaloClusterCorrection
CaloSwPhioff_versions = [

    # The original G3-based correction, translated from the
    # original fortran version.
    ['g3', cls, ['CaloSwPhioff_g3.CaloSwPhioff_g3_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # DC2 simulation and 8.x.0 reconstruction.
    # Electrons at 50, 100, and 200 GeV were used.
    # Derived for 5x5 clusters only.
    # Added in LArClusterRec-02-05-12, in 8.6.0.
    ['v2', cls, ['CaloSwPhioff_v2.CaloSwPhioff_v2_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # 11.0.41 simulation and 12.0 reconstruction.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # In CaloClusterCorrection-00-02-58, in 12.0.3.
    ['v3', cls, ['CaloSwPhioff_v3.CaloSwPhioff_v3_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # 12.0.3 simulation and reconstruction.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # In CaloClusterCorrection-xxx, in 12.0.4.
    ['v4', cls, ['CaloSwPhioff_v4.CaloSwPhioff_v4_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # Same as v4, but with the sign of the correction flipped in the barrel.
    # This because the orientation of the accordions is backwards in the MC.
    ['v4data', cls, ['CaloSwPhioff_v4data.CaloSwPhioff_v4data_parms',
                     'caloswcorr_pool', CALOCORR_COOL], keys],

    # mc12 simulation and reconstruction.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # In CaloClusterCorrection-xxx, in ??????.
    ['v5', cls, ['CaloSwPhioff_v5.CaloSwPhioff_v5_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # Same as v5, but with the sign of the correction flipped in the barrel.
    # This because the orientation of the accordions is backwards in the MC.
    ['v5data', cls, ['CaloSwPhioff_v5data.CaloSwPhioff_v5data_parms',
                     'caloswcorr_pool', CALOCORR_COOL], keys],
]


#
# Create a new tool instance.
#  FLAGS is the configuration flags instance.
#  SAMPLING is the calorimeter sampling (EMB1, EMB2, EME1, EME2) to which
#   this correction will be applied.
#  NAME is the base name for this tool.  If defaulted, a name will
#   be constructed from the name of the correction, the version, and the key.
#  If SUFFIX is not None, it will be added onto the end of the tool name.
#  VERSION is a string specifying which of several versions of the correction
#   to choose.  If defaulted, the latest version of the correction is chosen.
#  KEY is a string to specify the type of cluster to which the correction
#   applies.  The convention is to use `ele55' for 5x5 electron clusters,
#   `gam35' for 3x5 photon clusters, and so on.
#  SOURCE tells from where we should read the calibration constants.
#   See common.py for a description of the possibilities.
#   None means to use the default.
#  CONFCLASS gives the Configurable class to use to create the tool.
#   It may (and usually should) be defaulted.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def make_CaloSwPhioff (flags,
                       sampling,
                       name = None,
                       suffix = None,
                       version = None,
                       key = CALOCORR_DEFAULT_KEY,
                       source = None,
                       confclass = None,
                       **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwPhioff_versions,
                     name      = name,
                     basename  = 'phioff',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = sampling,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)

