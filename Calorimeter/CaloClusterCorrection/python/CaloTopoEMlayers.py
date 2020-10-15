# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMlayers.py
# Created: Mar 2007, sss, from old job options.
# Purpose: Layer variable calculation.
#
# This `correction' is responsible for calculating the layer-by-layer
# variables (position, width, energy) from the list of cells composing
# a cluster.
#
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_NOPOOL, CALOCORR_DEFAULT_KEY, CALOCORR_EMTOPO
from CaloClusterCorrection.common import makecorr

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
cls = CompFactory.CaloTopoEMlayers  # CaloClusterCorrection
CaloTopoEMlayers_versions = [
    ['',   cls, ['CaloTopoEMlayers.CaloTopoEMlayers_parms',  CALOCORR_NOPOOL]],
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
# The following arguments set tool parameters:
#  CELLS_NAME specifies the SG key to use to find the scintillator cells.
#
# Additional keyword arguments may be passed to override any tool
# parameters/constants.
#
def make_CaloTopoEMlayers (flags,
                           name = None,
                           suffix = None,
                           version = None,
                           key = CALOCORR_DEFAULT_KEY,
                           source = None,
                           confclass = None,
                           cells_name = None,
                           **kw):

    if cells_name is not None:
        kw['cells_name'] = cells_name

    return makecorr (flags,
                     versions = CaloTopoEMlayers_versions,
                     name = name,
                     basename = 'layers',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)

# ... layer computations -- ugly workaround using a large window
#     to make "sure" all the TopoCluster's cells are inside
class CaloTopoEMlayers_parms:
    eta_size = 11
    phi_size = 11
