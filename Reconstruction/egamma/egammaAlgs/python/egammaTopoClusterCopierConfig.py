# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Instantiate egammaTopoClusterCopier with default configuration"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaAlgs.egammaAlgsConf import egammaTopoClusterCopier


def egammaTopoClusterCopierCfg(flags, name='egammaTopoClusterCopier', **kwargs):

    mlog = logging.getLogger(name)
    mlog.info('Starting configuration')

    acc = ComponentAccumulator()

    kwargs.setdefault("InputTopoCollection", flags.Egamma.Keys.Input.TopoClusters)

    egtopocluster = flags.Egamma.Keys.Internal.EgammaTopoClusters
    kwargs.setdefault("OutputTopoCollection", egtopocluster)
    kwargs.setdefault("OutputTopoCollectionShallow", "tmp_"+egtopocluster)

    egcopierAlg = egammaTopoClusterCopier(name, **kwargs)

    acc.addEventAlgo(egcopierAlg)
    return acc
