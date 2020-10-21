# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwEtaoff.py
# Created: Nov 2006, sss
# Purpose: Steering module for eta position (s-shape) corrections.
#
# Due to the cell structure of the calorimeter, measurements in eta
# are biased towards the cell center.  There will also be other biases
# near edges and cracks.  This is sometimes called the S-shape correction,
# after how the bias looks when plotted.  This correction attempts
# to remove these biases.
#
# In general, this is done by fitting an empirical function that evolves
# as a function of eta.  The details of how this is done has changed
# significantly with time.  For more details, see the C++ header files
# for the individual versions.
#


from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_SW, \
     EMB1, EME1, EMB2, EME2
from CaloClusterCorrection.common import makecorr

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.constants import sw_valid_keys as keys
cls_g3 = {EMB1 : CompFactory.CaloSwEta1b_g3,  # CaloClusterCorrection
          EMB2 : CompFactory.CaloSwEta2b_g3,  # CaloClusterCorrection
          EME1 : CompFactory.CaloSwEta1e_g3,  # CaloClusterCorrection
          EME2 : CompFactory.CaloSwEta2e_g3}  # CaloClusterCorrection
cls_v2 = CompFactory.CaloSwEtaoff_v2  # CaloClusterCorrection
cls_v3 = CompFactory.CaloSwEtaoff_v3  # CaloClusterCorrection
CaloSwEtaoff_versions = [

    # The original G3-based correction, translated from the
    # original fortran version.
    ['g3', cls_g3, ['CaloSwEtaoff_g3.CaloSwEtaoff_g3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # DC2 simulation and 8.x.0 reconstruction.
    # Electrons at 50, 100, and 200 GeV were used.
    # Derived for 5x5 clusters only.
    # Added in LArClusterRec-02-05-12, in 8.6.0.
    ['v2', cls_v2, ['CaloSwEtaoff_v2.CaloSwEtaoff_v2_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # 11.0.41 simulation and 12.0 reconstruction.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # In CaloClusterCorrection-00-02-58, in 12.0.3.
    ['v3', cls_v3, ['CaloSwEtaoff_v3.CaloSwEtaoff_v3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # 12.0.3 simulation and reconstruction.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # In CaloClusterCorrection-00-02-61, in 12.0.4.
    ['v4', cls_v3, ['CaloSwEtaoff_v4.CaloSwEtaoff_v4_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # 12.0.3 simulation and reconstruction.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # In CaloClusterCorrection-00-02-74, in 12.X.0.
    # The difference from v4 is that this is derived with a different
    # depth function, which was derived by requiring that the eta position
    # resolution be optimal, and the use of large-statistics samples
    # for the 50 and 100 GeV energy points.
    ['v4_1', cls_v3, ['CaloSwEtaoff_v4_1.CaloSwEtaoff_v4_1_parms',
                      'caloswcorr_pool', CALOCORR_COOL], keys],

    # Ewan edit
    # Not sure what version of athena this will be for (v20?).  November 2013 edits
    # Corrections made using mc11_7TeV
    # The differences come from an updated understanding of the upstream material (first update since data taking),
    # and a fix on the endcap, which were shifted.
    # Both electrons and photons, 5x5, 3x5, 3x7 clusters.
    # Updated version of CaloClusterCorrectionAnalysis corrects outliers removal
    ['v5', cls_v3, ['CaloSwEtaoff_v5.CaloSwEtaoff_v5_parms',
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
def make_CaloSwEtaoff (flags,
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
                     versions  = CaloSwEtaoff_versions,
                     name      = name,
                     basename  = 'etaoff',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = sampling,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                      **kw)

