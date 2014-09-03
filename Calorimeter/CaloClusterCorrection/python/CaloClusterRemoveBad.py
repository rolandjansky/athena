# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


# File: CaloClusterCorrection/python/CaloClusterRemoveBad.py
# Created: July 2010 Hong Ma
# Purpose: Steering module for bad cluster removal 

from CaloClusterCorrection import CaloClusterCorrectionConf
from CaloClusterCorrection.common import *

cls = CaloClusterCorrectionConf.CaloClusterRemoveBad
CaloClusterRemoveBad_versions = [
    ['',              cls,
     ['CaloClusterRemoveBad.CaloClusterRemoveBad_parms',
      CALOCORR_NOPOOL]],
    ]

def make_CaloClusterRemoveBad (name = None,
                                      suffix = None,
                                      version = None,
                                      key = CALOCORR_DEFAULT_KEY,
                                      source = None,
                                      confclass = None,
                                      **kw):
    return makecorr (versions = CaloClusterRemoveBad_versions,
                     name = name,
                     basename = 'removebad',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)


class CaloClusterRemoveBad_parms:
    # Energy threshold for removal.
    EMin = 0
