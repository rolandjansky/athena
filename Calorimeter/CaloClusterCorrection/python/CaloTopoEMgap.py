# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMgap.py
# Created: Mar 2007, sss, from earlier job options.
# Purpose: Steering module + corrections for gap correction
# for TopoEM clusters.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_EMTOPO, CALOCORR_CLUSTER
from CaloClusterCorrection.common import makecorr

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.constants import topoem_valid_keys as keys
cls = CompFactory.CaloTopoEMGap   # CaloClusterCorrection
CaloTopoEMgap_versions = [

    # v1 --- from the original DC3 job options file.
    ['v1', cls, ['CaloTopoEMgap.CaloTopoEMgap_v1_parms',
                 'calotopoemcorr_pool', CALOCORR_COOL], keys],

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
def make_CaloTopoEMgap (flags,
                        name = None,
                        suffix = None,
                        version = None,
                        key = CALOCORR_DEFAULT_KEY,
                        source = None,
                        confclass = None,
                        **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloTopoEMgap_versions,
                     name      = name,
                     basename  = 'topogap',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)


# Gap correction, from 12.0.3?
# I think this was derived for 6-3-3 clusters.
# Currently applied for all emtopo clusters.
GapCorr_el_1203 = [
[1.4375, 881.659, 1.51111, 1.09507, 893.54],
[1.4625, 924.435, 1.60056, 1.14361, 2136.88],
[1.4875, 1738.92, 1.35488, 1.06781, 2691.53],
[1.5125, 1418.49, 1.52461, 0.864585, 3665.26],
[1.5375, 404.816, 1.14325, 0.914854, 1577.98],
]

GapCorr_gam_1203 = [
[1.4375, 707.923, 1.0929,   1.12359,  0.0],
[1.4625, 2385.57, 1.35795,  1.10472,  0.0],
[1.4875, 11680,   1.98237,  0.732961, 0.0],
[1.5125, 4506.57, 1.77194,  0.749464, 0.0],
[1.5375, 589.303, 0.850365, 0.955104, 0.0],
]


class CaloTopoEMgap_v1_parms:
    _default_keys = ['ele633', 'gam633']

    region = CALOCORR_CLUSTER
    # Disable this correction for ele420 clusters.
    _etamin_crack = 1.425
    etamin_crack = {'ele633' : _etamin_crack,
                    'gam633' : _etamin_crack,
                    'ele420' : 999999999,
                    'ele430' : 999999999,
                    }
    etamax_crack = 1.55
    degree = 3
    use_raw_eta = False
    correction = {'ele633' : GapCorr_el_1203,
                  'gam633' : GapCorr_gam_1203}

