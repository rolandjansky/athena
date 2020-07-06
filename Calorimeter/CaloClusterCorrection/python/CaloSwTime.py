# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# File: CaloClusterCorrection/python/CaloSwTime.py
# Created: May, 2008, M. Cooke
# Purpose: Steering module for duplicate removal tool

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_NOPOOL, CALOCORR_DEFAULT_KEY, CALOCORR_SW
from CaloClusterCorrection.common import makecorr

cls = CompFactory.CaloSwTime  # CaloClusterCorrection
CaloSwTime_versions = [
    ['',              cls,
     ['CaloSwTime.CaloSwTime_parms',
      CALOCORR_NOPOOL]],
    ]

def make_CaloSwTime (flags,
                     name = None,
                     suffix = None,
                     version = None,
                     key = CALOCORR_DEFAULT_KEY,
                     source = None,
                     confclass = None,
                     **kw):
    return makecorr (flags,
                     versions = CaloSwTime_versions,
                     name = name,
                     basename = 'time',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = CALOCORR_SW,
                     **kw)


class CaloSwTime_parms:
    pass
