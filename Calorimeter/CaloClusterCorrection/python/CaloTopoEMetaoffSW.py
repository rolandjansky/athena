# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMetaoffSW.py
# Created: Apr, 2009, sss
# Purpose: Steering module + corrections for eta offset corrections
# for TopoEM clusters.
#
# TopoEM uses an old version of the SW corrections.
# We need to get a copy of it under the topoem hierarchy.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_EMTOPO
from CaloClusterCorrection.common import makecorr

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.constants import topoem_valid_keys as keys
cls_v2 = CompFactory.CaloSwEtaoff_v2   # CaloClusterCorrection
CaloTopoEMetaoffSW_versions = [

    # Copied from SW.
    # DC2 simulation and 8.x.0 reconstruction.
    # Electrons at 50, 100, and 200 GeV were used.
    # Derived for 5x5 clusters only.
    # Added in LArClusterRec-02-05-12, in 8.6.0.
    ['v2', cls_v2, ['CaloSwEtaoff_v2.CaloSwEtaoff_v2_parms',
                    'calotopoemcorr_pool', CALOCORR_COOL], keys],
    ]


#
# Create a new tool instance.
#  FLAGS is the configuration flags instance.
#  SAMPLING is the calorimeter sampling (EMB1, EMB2, EME1, EME2) to which
#   this correction will be applied, or None.
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
def make_CaloTopoEMetaoffSW (flags,
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
                     versions  = CaloTopoEMetaoffSW_versions,
                     name      = name,
                     basename  = 'topoetaoffsw',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = sampling,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)
