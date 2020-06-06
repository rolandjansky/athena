# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwDeadOTX_back.py 
# Created: March, 2010, L. Carminati
# Purpose: Steering module for dead OTX correction in the back
#

from CaloClusterCorrection import CaloClusterCorrectionConf
from CaloClusterCorrection.common import *

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.common import sw_valid_keys as keys
cls = CaloClusterCorrectionConf.CaloSwDeadOTX_back
CaloSwDeadOTX_back_versions = [
#
    ['v1', cls, ['CaloSwDeadOTX_back_v1.' +
                   'CaloSwDeadOTX_back_v1_parms',
                 'caloswcorr_pool', CALOCORR_COOL,
                 ], 
                 ['ele55',        'ele35',        'ele37',
                  'gam55_conv',   'gam35_conv',   'gam37_conv',
                  'gam55_unconv', 'gam35_unconv', 'gam37_unconv',
                  'ele33',        'ele57',        'ele77',
                  'gam33_conv',   'gam57_conv',   'gam77_conv',
                  'gam33_unconv', 'gam57_unconv', 'gam77_unconv'] ]
    ]

#
# Create a new tool instance.
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
def make_CaloSwDeadOTX_back(name = None,
                    suffix = None,
                    version = None,
                    key = CALOCORR_DEFAULT_KEY,
                    source = None,
                    confclass = None,
                    **kw):

    from CaloTools.CaloToolsConf import CaloAffectedTool
    theAffectedTool = CaloAffectedTool("CaloAffectedForDeadOTX_back")

    # Make the tool.
    return makecorr (versions  = CaloSwDeadOTX_back_versions,
                     name      = name,
                     basename  = 'deadOTXback',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     AffectedTool = theAffectedTool,
                     **kw)
