# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMphioff.py
# Created: Mar 2007, sss, from earlier job options.
# Purpose: Steering module + corrections for phi offset corrections
# for TopoEM clusters.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_EMTOPO, CALOCORR_COMBINED2
from CaloClusterCorrection.common import makecorr
import math

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
from CaloClusterCorrection.constants import topoem_valid_keys as keys
cls = CompFactory.CaloTopoEMphioff  # CaloClusterCorrection
CaloTopoEMphioff_versions = [

    # v1 --- from the original job options file.
    ['v1', cls, ['CaloTopoEMphioff.CaloTopoEMphioff_v1_parms',
                 'calotopoemcorr_pool', CALOCORR_COOL], keys],

    # v2 --- same, but with endcap shift removed.
    ['v2', cls, ['CaloTopoEMphioff.CaloTopoEMphioff_v2_parms',
                 'calotopoemcorr_pool', CALOCORR_COOL
                 ], keys],

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
def make_CaloTopoEMphioff (flags,
                           name = None,
                           suffix = None,
                           version = None,
                           key = CALOCORR_DEFAULT_KEY,
                           source = None,
                           confclass = None,
                           **kw):
    # Make the tool.
    return makecorr (flags,
                     versions  = CaloTopoEMphioff_versions,
                     name      = name,
                     basename  = 'topophioff',
                     suffix    = suffix,
                     version   = version,
                     key       = key,
                     sampling  = None,
                     source    = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)



# ... phi offset : barrel + endcap
# I think this was derived for 6-3-3 clusters.
# Currently applied for all emtopo clusters.
class CaloTopoEMphioff_v1_parms:
    region = CALOCORR_COMBINED2
    EdepA = [
        0.003568, 0.003794, 0.003693, 0.004167, 0.003905, 
        0.003608, 0.003133, 0.003018, 0.009932, 0.014734, 
        0.022201, 0.029784, 0.035050, 0.034313, 0.025003, 
        0.036740, 0.036417, 0.030943, 0.031863, 0.029199, 
        0.029680, 0.030302, 0.027069, -0.000001, -0.000001
        ]
    EdepB = [
        0.000151, 0.000151, 0.000183, 0.000246, 0.000384, 
        0.000463, 0.000543, 0.000654, 0.000260, 0.000338, 
        0.000426, 0.000592, 0.000861, 0.000965, 0.001584, 
        0.000329, 0.000303, 0.000321, 0.000347, 0.000351, 
        0.000370, 0.000431, 0.000513, 0.000517, 0.000790
        ]
    Granularity = 0.1
    EtaFrontier = [ 0.8, 1.5, 2.3, 2.5 ]
    FlipPhi = 1
    EndcapOffset = 2*math.pi/256/6


#
# The same as v1, but with the endcap shift removed.
#
class CaloTopoEMphioff_v2_parms:
    pass
for (k,v) in CaloTopoEMphioff_v1_parms.__dict__.items():
    if k[0] != '_':
        setattr (CaloTopoEMphioff_v2_parms, k, v)
CaloTopoEMphioff_v2_parms.EndcapOffset = 0
