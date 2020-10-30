# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# File: CaloClusterCorrection/python/CaloTopoEMmoments.py
# Created: Mar 2007, sss, from old job options.
# Purpose: Cluster moments calculation.
#
# This `correction' is responsible for calculating various cluster
# parameters.
#
#

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_NOPOOL, CALOCORR_DEFAULT_KEY, CALOCORR_EMTOPO
from CaloClusterCorrection.common import makecorr
from AthenaCommon.SystemOfUnits   import deg

from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
CaloNoiseCondAlg()

#
# This table lists all available versions of this correction.
# See common.py for a description of the contents.
#
cls = CompFactory.CaloClusterMomentsMaker  # CaloRec
CaloTopoEMmoments_versions = [
    ['',  cls, ['CaloTopoEMmoments.CaloTopoEMmoments_parms', CALOCORR_NOPOOL]],
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
def make_CaloTopoEMmoments (flags,
                            name = None,
                            suffix = None,
                            version = None,
                            key = CALOCORR_DEFAULT_KEY,
                            source = None,
                            confclass = None,
                            cells_name = None,
                            **kw):

    return makecorr (flags,
                     versions = CaloTopoEMmoments_versions,
                     name = name,
                     basename = 'EMTopoMoments',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = CALOCORR_EMTOPO,
                     **kw)

class CaloTopoEMmoments_parms:
    MaxAxisAngle = 20*deg
    MinBadLArQuality = 4000
    MomentsNames = [
        "FIRST_PHI" 
        ,"FIRST_ETA"
        ,"SECOND_R" 
        ,"SECOND_LAMBDA"
        ,"DELTA_PHI"
        ,"DELTA_THETA"
        ,"DELTA_ALPHA" 
        ,"CENTER_X"
        ,"CENTER_Y"
        ,"CENTER_Z"
        ,"CENTER_MAG"
        ,"CENTER_LAMBDA"
        ,"LATERAL"
        ,"LONGITUDINAL"
        ,"FIRST_ENG_DENS"
        ,"SECOND_ENG_DENS"
        ,"ENG_FRAC_MAX"
        ,"ISOLATION"
        ,"ENG_BAD_CELLS"
        ,"N_BAD_CELLS"
        ,"N_BAD_CELLS_CORR"
        ,"BAD_CELLS_CORR_E"
        ,"BADLARQ_FRAC"
        ,"ENG_POS"
        ,"SIGNIFICANCE"
        ]
