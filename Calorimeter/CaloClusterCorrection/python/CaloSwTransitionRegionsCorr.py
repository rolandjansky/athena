# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwTransitionRegionsCorr.py
# Created: Dec, 2007, L. Carminati
# Purpose: Steering module for transition region correction.
#
# This module is used to set up the transition region correction.
#
# The entry point for this module is make_CaloSwTransitionRegionsCorr, which
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
cls = CompFactory.CaloSwTransitionRegionsCorr  # CaloClusterCorrection
CaloSwTransitionRegionsCorr_versions = [

    # 12.0.3 simulation and reconstruction, with ideal (calib0) geometry.
    ['v1', cls, ['CaloSwTransitionRegionsCorr_v1.' +
                   'CaloSwTransitionRegionsCorr_v1_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # ATLAS-CSC-05-00-00 (14.0.0) geometry simulation.
    ['v5', cls, ['CaloSwTransitionRegionsCorr_v5.' +
                   'CaloSwTransitionRegionsCorr_v5_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # ATLAS-CSC-05-00-00 (14.0.0) geometry simulation.
    # Some fixes for eta=0 and eta=0.8 .
    ['v5_1', cls, ['CaloSwTransitionRegionsCorr_v5_1.'+
                   'CaloSwTransitionRegionsCorr_v5_1_parms',
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
#  CONFCLASS gives the Configurable class to use to create the tool.
#   It may (and usually should) be defaulted.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def make_CaloSwTransitionRegionsCorr (flags,
                                      name = None,
                                      suffix = None,
                                      version = None,
                                      key = CALOCORR_DEFAULT_KEY,
                                      source = None,
                                      confclass = None,
                                      **kw):

    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwTransitionRegionsCorr_versions,
                     name      = name,
                     basename  = 'trcorr',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)
