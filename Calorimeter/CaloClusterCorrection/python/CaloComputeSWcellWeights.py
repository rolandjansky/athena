# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloComputeSWcellWeights.py
# Created: Nov 2006, sss
# Purpose: Reweight cluster cells to eliminate double-counting between
#          clusters.
#
# With the default cluster finding, a given cell may be included
# in more than one cluster, leading to double-counting of the cell's
# energy.  This correction pass assigns weights to such duplicated
# cells such that the sum of the weights over all clusters is one,
# thus removing the double-counting.
#
# Following this, the layer variables need to be recalculated.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_NOPOOL, CALOCORR_DEFAULT_KEY, CALOCORR_SW
from CaloClusterCorrection.common import makecorr


#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
cls = CompFactory.CaloComputeSWcellWeights # CaloClusterCorrection
CaloComputeSWcellWeights_versions = [
    ['',              cls,
     ['CaloComputeSWcellWeights.CaloComputeSWcellWeights_parms',
      CALOCORR_NOPOOL]],
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
def make_CaloComputeSWcellWeights (flags,
                                   name = None,
                                   suffix = None,
                                   version = None,
                                   key = CALOCORR_DEFAULT_KEY,
                                   source = None,
                                   confclass = None,
                                   **kw):
    return makecorr (flags,
                     versions = CaloComputeSWcellWeights_versions,
                     name = name,
                     basename = 'weightsEM',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)


class CaloComputeSWcellWeights_parms:
    pass
