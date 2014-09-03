# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CaloClusterCorrection.common import *

from CaloClusterCorrection import CaloClusterCorrectionConf 
cls = CaloClusterCorrectionConf.CaloClusterBadChannelList

CaloClusterListBadChannel_versions = [
    ['',              cls,
     ['CaloClusterListBadChannel.CaloClusterListBadChannel_parms',
      CALOCORR_NOPOOL]],
    ]

def make_CaloClusterListBadChannel (corrclass,
                                    name = None,
                                    suffix = None,
                                    version = None,
                                    key = CALOCORR_DEFAULT_KEY,
                                    source = None,
                                    confclass = None,
                                    **kw):

    return  makecorr(versions= CaloClusterListBadChannel_versions,
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
