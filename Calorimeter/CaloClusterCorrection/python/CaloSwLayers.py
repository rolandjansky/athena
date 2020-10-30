# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloSwLayers.py
# Created: Nov 2006, sss
# Purpose: Layer variable calculation.
#
# This `correction' is responsible for calculating the layer-by-layer
# variables (position, width, energy) from the list of cells composing
# a cluster.
#
# There are two versions of this correction.
#
# The one with an empty version string is the normal one
# that runs as the first correction
# for any cluster.  It fills the cluster with cells within the proper
# eta/phi window, and calculates the variables.
#
# version='wt' is used when recalculating the layer variables
# after the cells have been reweighted.  It does not recalculate the
# cell list; rather, the existing list is used.  The layer variables
# are recalculated based on these cells.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_NOPOOL, CALOCORR_DEFAULT_KEY, CALOCORR_SW
from CaloClusterCorrection.common import makecorr
import string

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
cls = CompFactory.CaloFillRectangularCluster # CaloClusterCorrection
CaloSwLayers_versions = [
    ['wt', cls, ['CaloSwLayers.CaloSwLayersWt_parms', CALOCORR_NOPOOL]],
    ['',   cls, ['CaloSwLayers.CaloSwLayers_parms',   CALOCORR_NOPOOL]],
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
def make_CaloSwLayers (flags,
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
                     versions = CaloSwLayers_versions,
                     name = name,
                     basename = 'layers',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)

def _parse_key (k):
    k = k.lstrip (string.ascii_letters + '_')
    k = k.rstrip (string.ascii_letters + '_')
    if k.find ('_') > 0:
        (eta, phi) = k.split ('_')
    else:
        (eta, phi) = (k[0], k[1])
    return (int (eta), int (phi))

class _etaphi_getter:
    def __init__ (self, idx):
        self.idx = idx
    def __getitem__ (self, k):
        return _parse_key (k)[self.idx]


class CaloSwLayers_parms:
    fill_cluster = True
    eta_size = _etaphi_getter (0)
    phi_size = _etaphi_getter (1)


class CaloSwLayersWt_parms:
    fill_cluster = False
    eta_size = _etaphi_getter (0)
    phi_size = _etaphi_getter (1)
