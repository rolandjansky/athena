# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure cluster correction"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from egammaTools.egammaToolsConf import egammaSwTool

import six

def _configureClusterCorrections(swTool, flags):
    "Add attributes ClusterCorrectionToolsXX to egammaSwTool object"
    from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrections, rfac, etaoff_b1, etaoff_e1, \
        etaoff_b2, etaoff_e2, phioff_b2, phioff_e2, update, time, listBadChannel
    from CaloRec.CaloRecMakers import _process_tools

    clusterTypes = dict(
      Ele35='ele35', Ele55='ele55', Ele37='ele37',
      Gam35='gam35_unconv', Gam55='gam55_unconv', Gam37='gam37_unconv',
      Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
    )

    for attrName, clName in six.iteritems (clusterTypes):
        x = 'ClusterCorrectionTools' + attrName
        if not hasattr(swTool, x) or getattr(swTool, x):
            continue
        y = make_CaloSwCorrections(clName, suffix='EG',
                                   version=flags.Egamma.Calib.ClusterCorrectionVersion,
                                   cells_name=flags.Egamma.Keys.Input.CaloCells)
        setattr(swTool, x, _process_tools(swTool, y))

    # Super cluster position only corrections
    if flags.Egamma.doSuperclusters:
        for attrName, clName in six.iteritems (clusterTypes):
            n = 'ClusterCorrectionToolsSuperCluster' + attrName
            if not hasattr(swTool, n) or getattr(swTool, n):
                continue

            setattr(swTool, n, _process_tools(swTool,
                                              make_CaloSwCorrections(clName, suffix='EGSuperCluster',
                                                                     corrlist=[[rfac, 'v5'], [etaoff_b1, 'v5'], [etaoff_e1, 'v5'],
                                                                               [etaoff_b2, 'v5'], [etaoff_e2, 'v5'], [phioff_b2, 'v5data'],
                                                                               [phioff_e2, 'v5data'], [update], [time], [listBadChannel]],
                                                                     cells_name=flags.Egamma.Keys.Input.CaloCells)))


def egammaSwToolCfg(flags, name='egammaSwTool', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    egswtool = egammaSwTool(name, **kwargs)

    # For now, the correction is called, but this might change.
    _configureClusterCorrections(egswtool, flags)

    acc.setPrivateTools(egswtool)
    return acc
