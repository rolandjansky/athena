# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMClusterUpdate.py
# Created: Nov 2006, sss
# Purpose: Recalculate global cluster variable from layer variables.
#
# This `correction' recalculates the global cluster variables from the
# individual layer variables.  This is mainly the eta and phi positions.
# The energy may also be reset to the sum of all layer energies.
# There are two variants: the default version, with an empty version
# string, recalculates the energy.  The version named `dont_update_e'
# leaves the total cluster energy unchanged.
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_COOL, CALOCORR_DEFAULT_KEY, CALOCORR_EMTOPO
from CaloClusterCorrection.common import makecorr


#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
cls = CompFactory.CaloClusterUpdate  # CaloClusterCorrection
CaloTopoEMClusterUpdate_versions = [
    # This version doesn't recalculate the total cluster energy.
    ['dont_update_e', cls,
     ['CaloTopoEMClusterUpdate.CaloTopoEMClusterUpdate_dont_update_e_parms',
      'calotopoemcorr_pool', CALOCORR_COOL]],

    # This version does recalculate the total cluster energy.
    ['',              cls,
     ['CaloTopoEMClusterUpdate.CaloTopoEMClusterUpdate_parms',
      'calotopoemcorr_pool', CALOCORR_COOL]],
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
def make_CaloTopoEMClusterUpdate (flags,
                                  name = None,
                                  suffix = None,
                                  version = None,
                                  key = CALOCORR_DEFAULT_KEY,
                                  source = None,
                                  confclass = None,
                                  **kw):
    return makecorr (flags,
                     versions = CaloTopoEMClusterUpdate_versions,
                     name = name,
                     basename = 'larupdate',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)


class CaloTopoEMClusterUpdate_parms:
    update_energy = True


class CaloTopoEMClusterUpdate_dont_update_e_parms:
    update_energy = False


