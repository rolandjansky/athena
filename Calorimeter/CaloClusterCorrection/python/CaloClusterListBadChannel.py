# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from CaloClusterCorrection.constants import \
     CALOCORR_NOPOOL, CALOCORR_DEFAULT_KEY
from CaloClusterCorrection.common import makecorr

cls = CompFactory.CaloClusterBadChannelList # CaloClusterCorrection

CaloClusterListBadChannel_versions = [
    ['',              cls,
     ['CaloClusterListBadChannel.CaloClusterListBadChannel_parms',
      CALOCORR_NOPOOL]],
    ]

def make_CaloClusterListBadChannel (flags,
                                    corrclass,
                                    name = None,
                                    suffix = None,
                                    version = None,
                                    key = CALOCORR_DEFAULT_KEY,
                                    source = None,
                                    confclass = None,
                                    **kw):

    return  makecorr(flags,
                     versions= CaloClusterListBadChannel_versions,
                     name = name,
                     basename = 'listBadChannels',
                     suffix = suffix,
                     version = version,
                     key = key,
                     sampling = None,
                     source = source,
                     confclass = confclass,
                     corrclass = corrclass,
                     **kw)


class CaloClusterListBadChannel_parms:
    pass
