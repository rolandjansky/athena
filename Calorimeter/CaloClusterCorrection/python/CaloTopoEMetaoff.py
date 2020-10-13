# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMetaoff.py
# Created: Mar 2007, sss, from earlier job options.
# Purpose: Steering module + corrections for eta offset corrections
# for TopoEM clusters.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_EMTOPO, CALOCORR_COMBINED2
from CaloClusterCorrection.common import makecorr

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.constants import topoem_valid_keys as keys
cls = CompFactory.CaloTopoEMsshape  # CaloClusterCorrection
CaloTopoEMetaoff_versions = [

    # v1 --- from the original job options file.
    ['v1', cls, ['CaloTopoEMetaoff.CaloTopoEMetaoff_v1_parms',
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
def make_CaloTopoEMetaoff (flags,
                           name = None,
                           suffix = None,
                           version = None,
                           key = CALOCORR_DEFAULT_KEY,
                           source = None,
                           confclass = None,
                           **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloTopoEMetaoff_versions,
                     name      = name,
                     basename  = 'topoetaoff',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)



# ... s-shapes : barrel + endcap, S2
# I think this was derived for 6-3-3 clusters.
# Currently applied for all emtopo clusters.
class CaloTopoEMetaoff_v1_parms:
    region = CALOCORR_COMBINED2
    Granularity = 0.1
    EtaFrontier = [ 1.4, 1.5, 2.5 ]
    P0 = [
        -0.9892, -0.9713, -0.9438, -0.9250, -0.9403, 
        -0.9298, -0.9410, -0.9926, -0.9758, -0.9662, 
        -0.9866, -1.0061, -0.9970, -0.9739, -1.0000,
        -0.8058, -0.9600, -0.9551, -1.0209, -1.1195, 
        -1.2160, -1.2755, -1.4796, -1.2666, -1.7747
        ]
    P1 = [
        5.8053, 5.9395, 6.0614, 6.1413, 6.0782, 
        6.1650, 5.9474, 5.3771, 5.7169, 5.4273, 
        5.2548, 5.0040, 4.9485, 5.1636, 5.0000,
        6.0134, 4.9725, 5.0041, 4.5225, 4.1565, 
        3.7802, 3.4394, 3.0471, 3.4170, 2.8284
        ]
    P2 = [
        2.4509, 2.4091, 2.3431, 2.3011, 2.3374, 
        2.3280, 2.3262, 2.4251, 2.4053, 2.3446, 
        2.3905, 2.4099, 2.3752, 2.3787, 2.0000,
        1.9980, 2.2829, 2.2696, 2.3523, 2.5219, 
        2.6509, 2.6739, 2.9549, 2.6459, 3.4399
        ]
    P3 = [
        0.197,  0.184,  0.291,  0.493,  0.687, 
        0.977,  1.335,  1.721,  1.110,  1.668, 
        2.295,  2.761,  2.847,  3.797,  4.000,
        -0.446, -0.443, -0.379, -0.261, -0.025, 
        -0.169, -0.065, -0.090, -0.227, -3.554
        ]
    P4 = [
        -0.115, -0.094, -0.143, -0.245, -0.343, 
        -0.470, -0.678, -0.835, -0.494, -0.898, 
        -1.232, -1.531, -1.666, -2.224, -2.000,
        0.502,  0.442,  0.299,  0.205,  0.139, 
        0.192,  0.123,  0.199,  0.179,  0.773
        ]
