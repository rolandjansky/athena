# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMphimod.py
# Created: Mar 2007, sss, from earlier job options.
# Purpose: Steering module + corrections for phi modulation corrections
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
cls = CompFactory.CaloTopoEMphimod   # CaloClusterCorrection
CaloTopoEMphimod_versions = [

    # v1 --- from the original job options file.
    ['v1', cls, ['CaloTopoEMphimod.CaloTopoEMphimod_v1_parms',
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
def make_CaloTopoEMphimod (flags,
                           name = None,
                           suffix = None,
                           version = None,
                           key = CALOCORR_DEFAULT_KEY,
                           source = None,
                           confclass = None,
                           **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloTopoEMphimod_versions,
                     name      = name,
                     basename  = 'topophimod',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)



# ... phi modulation : barrel + endcap
# I think this was derived for 6-3-3 clusters.
# Currently applied for all emtopo clusters.
class CaloTopoEMphimod_v1_parms:
    region = CALOCORR_COMBINED2
    BarrelGranularity = 0.1
    EndcapGranularity = 0.05
    EtaFrontier = [ 1.5, 1.55, 2.5 ]

    P1b = [
        -0.003018, -0.002832, -0.002661, -0.002509, -0.002378, 
        -0.002391, -0.002121, -0.002167, -0.001456, -0.000904, 
        -0.000721, -0.000666, -0.000912, -0.000395,  0.
        ]
    P2b = [
        0.000377, 0.000661,  0.000140, 0.000425, 0.000476, 
        0.000457, 0.000255,  0.000059, 0.000083, 0.000792, 
        0.000427, 0.000153, -0.000525, 0.000428, 0.
        ]
    P3b = [
        -0.000209, -0.000571, -0.000416, -0.000549, -0.000696, 
        -0.000422, -0.000422, -0.000040, -0.000167,  0.000329, 
        0.000545,  0.000434,  0.000337,  0.000854,  0.
        ]
    P4b = [
        -0.000063, -0.000020, -0.000080, -0.000158, -0.000154, 
        -0.000040,  0.000273,  0.000152,  0.000067, -0.000015, 
        0.000401, -0.000601, -0.000302,  0.000845,  0.
        ]
    P1e = [
        -0.002073, -0.002342,  0.000042, -0.000401,  0.000412, 
        0.001082,  0.001728,  0.002083,  0.002554,  0.003267, 
        0.003507,  0.003587,  0.003455,  0.002587,  0.002392, 
        0.001015, -0.000090, -0.002045, -0.004660
        ]
    P2e = [
        0.000259,  0.000438,  0.000008,  0.000805,  0.000863, 
        -0.001590, -0.000420, -0.000060, -0.000674,  0.000052, 
        -0.000436,  0.000024, -0.000162,  0.000214,  0.000182, 
        0.000829,  0.000245,  0.000749,  0.001217
        ]
    P3e = [
        0.001139,  0.001065, -0.000462, -0.000382, -0.000938, 
        -0.000569, -0.001691, -0.002208, -0.002703, -0.002319, 
        -0.002547, -0.001932, -0.002138, -0.002233, -0.000784, 
        -0.000421,  0.000124,  0.000936,  0.001676
        ]
    P4e = [
        -0.000249, -0.001185, -0.001608, -0.000078, -0.000446, 
        -0.001075, -0.000420, -0.000101, -0.000349,  0.000215, 
        -0.000475, -0.000355,  0.000067, -0.000357, -0.000062, 
        -0.000185, -0.000613, -0.000351, -0.000796
        ]
