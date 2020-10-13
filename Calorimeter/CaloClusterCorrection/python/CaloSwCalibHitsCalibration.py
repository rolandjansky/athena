# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwCalibHitsCalibration.py
# Created: Jan 2007, L. Carminati
# Purpose: Steering module for CalibHits based calibration of e/gamma 
#
# This module is used to set up the calibration hits based corrections
# for electrons and photons
#
# The entry point for this module is make_CaloSwCalibHitsCalibration, which
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
cls = CompFactory.CaloSwCalibHitsCalibration # CaloClusterCorrection
CaloSwCalibHitsCalibration_versions = [

    # 12.0.3 simulation and reconstruction, with ideal (calib0) geometry.
    ['v1', cls, ['CaloSwCalibHitsCalibration_v1.' +
                   'CaloSwCalibHitsCalibration_v1_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],
    ['v2', cls, ['CaloSwCalibHitsCalibration_v2.' +
                   'CaloSwCalibHitsCalibration_v2_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # ATLAS-CSC-05-00-00 (14.0.0) geometry simulation.
    ['v5', cls, ['CaloSwCalibHitsCalibration_v5.' +
                   'CaloSwCalibHitsCalibration_v5_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # ATLAS-CSC-05-00-00 (14.0.0) geometry simulation.
    # Some fixes for eta=0 and eta=0.8 .
    ['v5_1', cls, ['CaloSwCalibHitsCalibration_v5_1.' +
                   'CaloSwCalibHitsCalibration_v5_1_parms',
                 'caloswcorr_pool', CALOCORR_COOL], keys],

    # ATLAS-GEO-16-00-00 (15.6.X) geometry simulation + ad-hoc correction for data-MC leakage difference
    # Now have separate corrections for converted and unconverted photons.
    ['v8leakdata', cls, ['CaloSwCalibHitsCalibration_v8leakdata.' +
                    'CaloSwCalibHitsCalibration_v8leakdata_parms',
                  'caloswcorr_pool', CALOCORR_COOL], 
                 ['ele55',        'ele35',        'ele37',
                  'gam55_conv',   'gam35_conv',   'gam37_conv',
                  'gam55_unconv', 'gam35_unconv', 'gam37_unconv',
                  'ele33',        'ele57',        'ele77',
                  'gam33_conv',   'gam57_conv',   'gam77_conv',
                  'gam33_unconv', 'gam57_unconv', 'gam77_unconv'] ],

    # ATLAS-GEO-16-00-00 (15.6.X) geometry simulation.
    # Now have separate corrections for converted and unconverted photons.
    ['v8', cls, ['CaloSwCalibHitsCalibration_v8.' +
                   'CaloSwCalibHitsCalibration_v8_parms',
                 'caloswcorr_pool', CALOCORR_COOL], 
                 ['ele55',        'ele35',        'ele37',
                  'gam55_conv',   'gam35_conv',   'gam37_conv',
                  'gam55_unconv', 'gam35_unconv', 'gam37_unconv',
                  'ele33',        'ele57',        'ele77',
                  'gam33_conv',   'gam57_conv',   'gam77_conv',
                  'gam33_unconv', 'gam57_unconv', 'gam77_unconv'] ],

    # ATLAS-GEO-18-01-00 (mc11) geometry simulation + ad-hoc correction for data-MC leakage difference
    # Now have separate corrections for converted and unconverted photons.
    ['v9leakdata', cls, ['CaloSwCalibHitsCalibration_v9leakdata.' +
                     'CaloSwCalibHitsCalibration_v9leakdata_parms',
                   'caloswcorr_pool', CALOCORR_COOL], 
                 ['ele55',        'ele35',        'ele37',
                  'gam55_conv',   'gam35_conv',   'gam37_conv',
                  'gam55_unconv', 'gam35_unconv', 'gam37_unconv',
                  'ele33',        'ele57',        'ele77',
                  'gam33_conv',   'gam57_conv',   'gam77_conv',
                  'gam33_unconv', 'gam57_unconv', 'gam77_unconv'] ],

    # ATLAS-GEO-18-01-00 (mc11) geometry simulation.
    # Now have separate corrections for converted and unconverted photons.
    ['v9', cls, ['CaloSwCalibHitsCalibration_v9.' +
                    'CaloSwCalibHitsCalibration_v9_parms',
                  'caloswcorr_pool', CALOCORR_COOL],
                 ['ele55',        'ele35',        'ele37',
                  'gam55_conv',   'gam35_conv',   'gam37_conv',
                  'gam55_unconv', 'gam35_unconv', 'gam37_unconv',
                  'ele33',        'ele57',        'ele77',
                  'gam33_conv',   'gam57_conv',   'gam77_conv',
                  'gam33_unconv', 'gam57_unconv', 'gam77_unconv'] ],

    # ATLAS-CSC-05-00-00 (14.0.0) geometry simulation + ad-hoc correction for data-MC leakage difference
    # Now have separate corrections for converted and unconverted photons.
    ['v6leakdata', cls, ['CaloSwCalibHitsCalibration_v6leakdata.' +
                    'CaloSwCalibHitsCalibration_v6leakdata_parms',
                  'caloswcorr_pool', CALOCORR_COOL], 
                 ['ele55',        'ele35',        'ele37',
                  'gam55_conv',   'gam35_conv',   'gam37_conv',
                  'gam55_unconv', 'gam35_unconv', 'gam37_unconv',
                  'ele33',        'ele57',        'ele77',
                  'gam33_conv',   'gam57_conv',   'gam77_conv',
                  'gam33_unconv', 'gam57_unconv', 'gam77_unconv'] ],

    # ATLAS-CSC-05-00-00 (14.0.0) geometry simulation.
    # Now have separate corrections for converted and unconverted photons.
    ['v6', cls, ['CaloSwCalibHitsCalibration_v6.' +
                   'CaloSwCalibHitsCalibration_v6_parms',
                 'caloswcorr_pool', CALOCORR_COOL], 
                 ['ele55',        'ele35',        'ele37',
                  'gam55_conv',   'gam35_conv',   'gam37_conv',
                  'gam55_unconv', 'gam35_unconv', 'gam37_unconv',
                  'ele33',        'ele57',        'ele77',
                  'gam33_conv',   'gam57_conv',   'gam77_conv',
                  'gam33_unconv', 'gam57_unconv', 'gam77_unconv'] ]

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
def make_CaloSwCalibHitsCalibration (flags,
                                     name = None,
                                     suffix = None,
                                     version = None,
                                     key = CALOCORR_DEFAULT_KEY,
                                     source = None,
                                     confclass = None,
                                     **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwCalibHitsCalibration_versions,
                     name      = name,
                     basename  = 'calhits',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)
