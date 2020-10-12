# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwLongWeights.py
# Created: Nov 2006, sss
# Purpose: Steering module for longitudinal weights corrections.
#
# This module is used to set up the longitudinal weight correction.
# This correction scales the energies of layers 0 and 3 by constants,
# then applies a linear transformation (scale plus offset) to the resulting
# energy.  All four of these constants are eta-dependent
# (but not energy-dependent).
#
# Note that for any given eta, we should apply either the gap or the LW
# correction, but not both.  For some versions of the corrections, that
# wasn't true.  See the comments in the individual versions for more
# about hacks that were used to deal with this.
#
# The entry point for this module is make_CaloSwLongWeights, which
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
cls = CompFactory.CaloSwLongWeights # CaloClusterCorrection
CaloSwLongWeights_versions = [

    # From SP and KL.  The original version.
    # Only one correction, derived from 5X5 DC2 electrons.
    # In LArClusterRec-02-05-10 in 8.5.0;
    # revised in LArClusterRec-02-05-25 in 9.0.3.
    # This was used for the `DC2' corrections.
    ['old', cls, ['CaloSwLongWeights_old.CaloSwLongWeights_old_parms',
                  'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  Derived from 9.0.4 MC for 3X5, 3X7, and 5X5 clusters
    # (electrons only).  This version is not applied in the gap region.
    # This was used for the `DC2new' corrections.
    ['904', cls, ['CaloSwLongWeights_904.CaloSwLongWeights_904_parms',
                  'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  Derived from 9.0.4 MC for 3X5, 3X7, and 5X5 clusters
    # (electrons only).  This version is applied in the gap region.
    # This was used for the `Rome' corrections.
    ['904gap', cls, ['CaloSwLongWeights_904.CaloSwLongWeights_904gap_parms',
                  'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  11.0.41 simulation, 12.0.0 reconstruction.
    # No separate photon correction.
    # In CaloClusterCorrection-00-02-38, 12.0.0.
    # This was used for the `DC3-02' corrections.
    ['v3', cls, ['CaloSwLongWeights_v3.CaloSwLongWeights_v3_parms',
                  'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  11.0.42 simulation, 12.0.2 reconstruction.
    # Separate electron and photon corrections.
    # In CaloClusterCorrection-00-02-59, 12.0.3.
    ['v3_1', cls, ['CaloSwLongWeights_v3.CaloSwLongWeights_v3_1_parms',
                  'caloswcorr_pool', CALOCORR_COOL], keys],

    # From SP.  12.0.3 simulation and reconstruction,
    # with ideal (calib0) geometry.
    # In CaloClusterCorrection-00-02-61, 12.0.4.
    ['v4', cls, ['CaloSwLongWeights_v4.CaloSwLongWeights_v4_parms',
                  'caloswcorr_pool', CALOCORR_COOL], keys],

    # From Stathes and Nicolas.
    # For ATLAS-CSC-05-00-00 geometry (14.0.0).
    ['v5', cls, ['CaloSwLongWeights_v5.CaloSwLongWeights_v5_parms',
                  'caloswcorr_pool', CALOCORR_COOL], keys],

    ['pi0_v1',cls,
     ['CaloSwLongWeights_pi0_v1.CaloSwLongWeights_pi0_v1_parms'],keys],

    # From Stathes and Nicolas.
    # For ATLAS-CSC-05-00-00 geometry.
    ['v6', cls, ['CaloSwLongWeights_v6.CaloSwLongWeights_v6_parms',
                 'caloswcorr_pool', CALOCORR_COOL],
     keys + ['gam55_conv', 'gam35_conv', 'gam37_conv']],
    
    # From Stathes and Nicolas.
    # For Atlfast2 simulation with ATLAS-CSC-05-00-00 geometry (14.0.0).
    ['atlfast_v1', cls,
     ['CaloSwLongWeights_atlfast_v1.CaloSwLongWeights_atlfast_v1_parms',
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
def make_CaloSwLongWeights (flags,
                            name = None,
                            suffix = None,
                            version = None,
                            key = CALOCORR_DEFAULT_KEY,
                            source = None,
                            confclass = None,
                            **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwLongWeights_versions,
                     name      = name,
                     basename  = 'lwc',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)
