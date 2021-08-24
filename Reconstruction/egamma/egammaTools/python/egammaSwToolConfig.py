# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure cluster correction"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
import GaudiKernel.GaudiHandles as GaudiHandles
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def _configureClusterCorrections(flags, swTool):
    "Add attributes ClusterCorrectionToolsXX to egammaSwTool object"

    acc= ComponentAccumulator()
    from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrectionsCfg

    clusterTypes = dict(
      Ele35='ele35', Ele55='ele55', Ele37='ele37',
      Gam35='gam35_unconv', Gam55='gam55_unconv', Gam37='gam37_unconv',
      Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
    )

    if flags.Egamma.doSuperclusters:
        version = flags.Egamma.Calib.SuperClusterCorrectionVersion
        suffix = 'EGSuperCluster'
        attrPref = 'ClusterCorrectionToolsSuperCluster'
    else:
        version = flags.Egamma.Calib.ClusterCorrectionVersion
        suffix = 'EG'
        attrPref = 'ClusterCorrectionTools'

    for attrName, clName in clusterTypes.items():
        attrName = attrPref + attrName
        if not hasattr(swTool, attrName):
            continue
        toolsAcc = make_CaloSwCorrectionsCfg (flags, clName, suffix = suffix,
                                              version = version,
                                              cells_name = flags.Egamma.Keys.Input.CaloCells)
        tools =  acc.popToolsAndMerge(toolsAcc)
        setattr( swTool, attrName, GaudiHandles.PrivateToolHandleArray(tools) )

    return acc

def egammaSwToolCfg(flags, name='egammaSwTool', **kwargs):

    mlog = logging.getLogger(name)
    mlog.debug('Start configuration')

    acc = ComponentAccumulator()

    egswtool = CompFactory.egammaSwTool(name, **kwargs)

    # For now, the correction is called, but this might change.
    utilsAcc = _configureClusterCorrections(flags, egswtool)
    acc.merge(utilsAcc)

    acc.setPrivateTools(egswtool)
    return acc


