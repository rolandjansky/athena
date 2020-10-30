# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwClcon.py
# Created: Nov 2006, sss
# Purpose: Steering module for cluster containment corrections.
#
# The cluster containment corrections scale the overall cluster energy
# by a constant which is both eta end energy dependent.
#
# The original definition of this correction was to include only the energy
# lost due to the spread of the cluster and the shower outside of the fixed
# window.  Later, this was used as a closure correction: it was set to get
# the cluster energy back to the true value, thus including any effects
# not otherwise accounted for.  In the current version of the corrections,
# this has been absorbed into the layer weights correction, and is no
# longer used as a separate correction pass.
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
cls_g3 = CompFactory.CaloSwClcon_g3 # CaloClusterCorrection
cls_v2 = CompFactory.CaloSwClcon_v2 # CaloClusterCorrection
CaloSwClcon_versions = [

    # The original G3-based correction, translated from the
    # original fortran version.
    ['g3', cls_g3, ['CaloSwClcon_g3.CaloSwClcon_g3_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # DC2 simulation.
    # To be used with the `old' layer weights corrections.
    # Electrons at 50, 100, and 200 GeV were used.
    # Added in LArClusterRec-02-05-25, in 9.0.3.
    ['dc2', cls_v2, ['CaloSwClcon_dc2.CaloSwClcon_dc2_parms',
                    'caloswcorr_pool', CALOCORR_COOL], keys],

    # DC2 simulation.
    # To be used with the `904' LW corrections with DC2 data.
    # Electrons at 20, 50, 100, 200, 500, and 1000 GeV were used.
    # Added in LArClusterRec-02-05-32, in 10.0.1.
    ['dc2new', cls_v2, ['CaloSwClcon_dc2new.CaloSwClcon_dc2new_parms',
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
def make_CaloSwClcon (flags,
                      name = None,
                      suffix = None,
                      version = None,
                      key = CALOCORR_DEFAULT_KEY,
                      source = None,
                      confclass = None,
                      **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloSwClcon_versions,
                     name      = name,
                     basename  = 'clcon',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)

