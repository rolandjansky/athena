# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwRfac.py
# Created: Nov 2006, sss
# Purpose: Steering module for R-factor correction.
#
# Some corrections, such as the position correction, are energy-dependent,
# but they need to run before the energy correction is done.  This causes
# a problem: the energy-dependence of these corrections is handled
# by finding the correction at several different true cluster energies,
# and then interpolating.  The value that we'd want to use for the
# interpolation is the true cluster energy.  However, all we have
# is the uncorrected reconstructed energy, which is systematically off
# from the true energy by a significant amount.
#
# To deal with this circularity, we first (before the position corrections)
# apply a crude energy correction to get the scale approximately correct.
# This corrected energy is then used for the interpolation in the
# position corrections.  When the layer update pass is run, the correction
# we make here gets undone.
#
# This `R-factor' correction simply scales the total cluster energy
# by an eta-dependent constant.  This is typically derived from
# 100 GeV electrons/photons.
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
cls = CompFactory.CaloScaleCluster  # CaloClusterCorrection
CaloSwRfac_versions = [
    # From 11.0.41 simulation and 12.0.0 reconstruction.
    # In CaloClusterCorrection-00-02-38, 12.0.0.
    # This was used for the `DC3-02' corrections.
    ['v3', cls, ['CaloSwRfac_v3.CaloSwRfac_v3_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],
    
    # From 12.0.3 simulation and reconstruction,
    # with ideal (calib0) geometry.
    # In CaloClusterCorrection-00-02-61, 12.0.4.
    ['v4', cls, ['CaloSwRfac_v4.CaloSwRfac_v4_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],
    # Ewan edit
    # Not sure what version of athena this will be for (v20?).  November 2013 edits
    # Corrections made using mc11_7TeV
    # The differences come from an updated understanding of the upstream material (first update since data taking),
    # and a fix on the endcap, which were shifted.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # Updated version of CaloClusterCorrectionAnalysis corrects outliers removal
    ['v5', cls, ['CaloSwRfac_v5.CaloSwRfac_v5_parms',
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
def make_CaloSwRfac (flags,
                     name = None,
                     suffix = None,
                     version = None,
                     key = CALOCORR_DEFAULT_KEY,
                     source = None,
                     confclass = None,
                     **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwRfac_versions,
                     name      = name,
                     basename  = 'rfac',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)
