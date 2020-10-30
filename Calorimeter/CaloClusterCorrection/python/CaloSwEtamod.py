# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwEtamod.py
# Created: Nov 2006, sss
# Purpose: Steering module for eta modulation corrections.
#
# There is also a small dependence of the calorimeter energy response
# on the eta offset of the particle impact within a calorimeter cell.
# This is due at least partly to the fact that more energy leaks
# outside of the cluster window the impact is near the edge of the center cell
# than when it is near the center.
#
# The binning and the procedure for extracting the modulations is the same
# as for the phi modulation case.  Within each bin, we fit a quadratic.
# It is seen that if we restrict ourselves to regions of the calorimeter
# away from non-uniformities, then the extracted fit parameters
# are generally consistent.  Accordingly, to derive
# this correction, we combine together all the uniform bins and fit to
# that.
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
cls_g3 = CompFactory.CaloSwEtamod_g3 # CaloClusterCorrection
cls_v2 = CompFactory.CaloSwEtamod_v2 # CaloClusterCorrection
CaloSwEtamod_versions = [

    # The original G3-based correction, translated from the
    # original fortran version.
    ['g3', cls_g3, ['CaloSwEtamod_g3.CaloSwEtamod_g3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # DC2 simulation and 8.x.0 reconstruction.
    # Electrons at 50, 100, and 200 GeV were used.
    # Derived for 5x5 clusters only.
    # This was added in LArClusterRec-02-05-25, in 9.0.3.
    # This version runs before the gross energy corrections.
    ['v2', cls_v2, ['CaloSwEtamod_v2.CaloSwEtamod_v2_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # This is the same as v2, except that it runs after the
    # gross energy corrections.
    # It was added in CaloClusterCorrection-00-02-58, in 12.0.3.
    ['v3', cls_v2, ['CaloSwEtamod_v2.CaloSwEtamod_v3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # This was derived using 12.0.3 simulation and reconstruction,
    # for 5x5, 3x5, and 3x7 clusters, for both electrons and photons.
    # It was added in CaloClusterCorrection-00-02-64, in 12.0.4.
    ['v4', cls_v2, ['CaloSwEtamod_v4.CaloSwEtamod_v4_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],
    ]


#
# Create a new tool instance.
#  FLAGS is the configuration flags instance.
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
def make_CaloSwEtamod (flags,
                       name = None,
                       suffix = None,
                       version = None,
                       key = CALOCORR_DEFAULT_KEY,
                       source = None,
                       confclass = None,
                       **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwEtamod_versions,
                     name      = name,
                     basename  = 'etamod',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)


