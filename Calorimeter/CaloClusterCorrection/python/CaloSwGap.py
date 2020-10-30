# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwGap.py
# Created: Nov 2006, sss
# Purpose: Steering module for gap correction.
#
# This module is used to set up the gap correction.
#
# In the gap, the energy is corrected using a weighted sum of the calorimeter
# and scintillator energies.  In later versions, an energy offset is added
# as well.  These constants are all eta-dependent.
#
# The entry point for this module is make_CaloSwGap, which
# returns a new correction tool.  The available versions are given
# in the table below.
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
cls_g3 = CompFactory.CaloSwGap_g3  # CaloClusterCorrection
cls_v2 = CompFactory.CaloSwGap_v2  # CaloClusterCorrection
cls_v3 = CompFactory.CaloSwGap_v3  # CaloClusterCorrection
CaloSwGap_versions = [
    # The original G3-based gap correction, translated from the
    # original fortran version.
    ['g3', cls_g3, ['CaloSwGap_g3.CaloSwGap_g3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # From sss.  DC2 simulation, 8.6.0 reconstruction.
    # Done only for 5x5 electrons, using 50, 100, 200 GeV electrons.
    # In LArClusterRec-02-05-27, 9.4.0.
    ['v2', cls_v2, ['CaloSwGap_v2.CaloSwGap_v2_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  11.0.41 simulation, 12.0.0 reconstruction.
    # No separate photon correction.
    # In CaloClusterCorrection-00-02-38, 12.0.0.
    # This was used for the `DC3-02' corrections.
    ['v3', cls_v2, ['CaloSwGap_v3.CaloSwGap_v3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  11.0.42 simulation, 12.0.2 reconstruction.
    # Separate electron and photon corrections.
    # In CaloClusterCorrection-00-02-59, 12.0.3.
    ['v3_1', cls_v2, ['CaloSwGap_v3.CaloSwGap_v3_1_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # In CaloClusterCorrection-00-02-61, 12.0.4.
    ['v4', cls_v2, ['CaloSwGap_v4.CaloSwGap_v4_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  12.0.3 simulation and reconstruction,
    # with ideal (calib0) geometry.
    # From SL. corrections from rel17 - no phi binning
    ['v5', cls_v3, ['CaloSwGap_v5.CaloSwGap_v5_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SL. corrections from rel17 - with phi binning
    ['v6', cls_v3, ['CaloSwGap_v6.CaloSwGap_v6_parms',
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
# The following arguments set tool parameters:
#  CELLS_NAME specifies the SG key to use to find the scintillator cells.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def make_CaloSwGap (flags,
                    name = None,
                    suffix = None,
                    version = None,
                    key = CALOCORR_DEFAULT_KEY,
                    source = None,
                    confclass = None,
                    cells_name = None,
                    **kw):

    if cells_name is not None:
        kw['cells_name'] = cells_name

    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwGap_versions,
                     name      = name,
                     basename  = 'gap',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)
