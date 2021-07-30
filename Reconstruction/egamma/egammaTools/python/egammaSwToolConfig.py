# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = "Configure cluster correction"

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentFactory import CompFactory
import GaudiKernel.GaudiHandles as GaudiHandles
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator



#TODO move out generators for these tools to CaloClustercorrections package
def ToolConstantsAlgCfg(flags, prefix, version):
    acc = ComponentAccumulator()
    version = f'-{version}' if version != '' else ''
    folder = f'CaloSwClusterCorrections.{prefix}{version}'
    cond = CompFactory.ToolConstantsCondAlg(f'ToolConstantsCondAlg_CaloSwClusterCorrections_{prefix}{version}',
                                            DetStoreKey = folder,
                                            ToolConstantsKey = folder) # this is realy the folder name
    acc.addCondAlgo(cond)


    # depending on the flags configure one of the input sources
    if flags.Calo.ClusterCorrection.defaultSource == 'pool':
        from EventSelectorAthenaPool.CondProxyProviderConfig import CondProxyProviderCfg
        from CaloClusterCorrection.poolfiles import poolfiles
        fileID = poolfiles.get("caloswcorr_pool") #TODO figure out how this relates to CalibVersion flag
        acc.merge (CondProxyProviderCfg (flags, [fileID]))

    #TODO prepare tools when reading from COOL and JO
    return acc

def CaloScaleClusterCfg(flags, prefix, clusterType, suffix, version):
    acc = ComponentAccumulator()
    tool = CompFactory.CaloScaleCluster(f'{prefix}_{version}_{clusterType}{suffix}',
                                        DBHandleKey = f'CaloSwClusterCorrections.{prefix}-{version}',
                                        prefix = clusterType.split('_')[0]+'.')
    acc.setPrivateTools(tool)
    acc.merge( ToolConstantsAlgCfg(flags, prefix, version))

    return acc

def CaloSwEtaoff_v3Cfg(flags, prefix, clusterType, suffix, version):
    acc = ComponentAccumulator()
    prefix1,prefix2 = prefix.split('_')
    
    tool = CompFactory.CaloSwEtaoff_v3(f'{prefix1}{prefix2}_{version}_{clusterType}{suffix}',
                                        DBHandleKey = f'CaloSwClusterCorrections.{prefix1}-{version}',
                                        prefix = clusterType.split('_')[0]+prefix.split('_')[1]+'.')
    acc.setPrivateTools(tool)
    acc.merge( ToolConstantsAlgCfg(flags, prefix1, version))

    return acc


def CaloSwPhioff_v2Cfg(flags, prefix, clusterType, suffix, version):
    acc = ComponentAccumulator()
    prefix1,prefix2 = prefix.split('_')
    tool = CompFactory.CaloSwPhioff_v2(f'{prefix1}{prefix2}_{version}_{clusterType}{suffix}',
                                       DBHandleKey = f'CaloSwClusterCorrections.{prefix1}-{version}',
                                       prefix = clusterType.split('_')[0]+prefix.split('_')[1]+'.')
    acc.setPrivateTools(tool)
    acc.merge( ToolConstantsAlgCfg(flags, prefix1, version))
    return acc


def CaloClusterUpdateCfg(flags, prefix, clusterType, suffix, version):
    acc = ComponentAccumulator()
    tool = CompFactory.CaloClusterUpdate(f'{prefix}_{clusterType}{suffix}', 
                                        DBHandleKey = f'CaloSwClusterCorrections.larupdate',
                                        prefix = clusterType.split('_')[0]+'.')
    acc.setPrivateTools(tool)
    acc.merge(ToolConstantsAlgCfg(flags, prefix, ''))
    return acc

def CaloSwTimeCfg(flags, prefix, clusterType, suffix, version):
    acc = ComponentAccumulator()
    tool = CompFactory.CaloSwTime(f'{prefix}_{clusterType}{suffix}',
                                    order = 801 )#TODO figure out how to pick this number
    acc.setPrivateTools(tool)

    return acc

def CaloClusterBadChannelListCfg(flags, prefix, clusterType, suffix, version):
    acc = ComponentAccumulator()
    tool = CompFactory.CaloClusterBadChannelList(f'{prefix}_{clusterType}{suffix}',
                                                 badChannelTool = CompFactory.CaloBadChanTool(),
                                                 order = 820) #TODO figure out how to get this number
    acc.setPrivateTools(tool)
    return acc

def CaloSwCorrecionsCfg(flags, clusterType, suffix='EGCluster', toolsGen=[]):
    """
    Configures multiple correction tools following the same pattern
    The toolsGen is expected to be list of tuples with: function generating accumulator with the tool, prefix to pass to the function, version to pass to the function
    All these private tools are then repackaged into new acc.
    """
    acc = ComponentAccumulator()
    algTools = []
    for gen, prefix, ver in toolsGen:
        toolAcc = gen(flags, prefix=prefix, clusterType=clusterType, suffix=suffix, version=ver)
        if toolAcc:
            algTools.append(acc.popToolsAndMerge(toolAcc))
    acc.setPrivateTools(algTools)
    return acc


def _desiredCorrections(flags, clusterType, suffix):
                                                              # generator                    # prefix           #version
    return CaloSwCorrecionsCfg(flags,
                            clusterType=clusterType, 
                            suffix=suffix,
                            toolsGen=[
                                                              (CaloScaleClusterCfg,          'rfac',            'v5'), 
                                                              (CaloSwEtaoff_v3Cfg,           'etaoff_b1',       'v5'), 
                                                              (CaloSwEtaoff_v3Cfg,           'etaoff_e1',       'v5'),
                                                              (CaloSwEtaoff_v3Cfg,           'etaoff_b2',       'v5'),
                                                              (CaloSwEtaoff_v3Cfg,           'etaoff_e2',       'v5'),
                                                              (CaloSwPhioff_v2Cfg,           'phioff_b2',       'v5data'),
                                                              (CaloSwPhioff_v2Cfg,           'phioff_e2',       'v5data'),
                                                              (CaloClusterUpdateCfg,         'larupdate',       ''),
                                                              (CaloSwTimeCfg,                'time',            ''),
                                                              (CaloClusterBadChannelListCfg, 'listBadChannels', '')
                                                            ])




def _configureClusterCorrections(flags, swTool):
    "Add attributes ClusterCorrectionToolsXX to egammaSwTool object"

    acc= ComponentAccumulator()
    from CaloClusterCorrection.CaloSwCorrections import make_CaloSwCorrectionsCfg#, rfac, etaoff_b1, etaoff_e1, \
#        etaoff_b2, etaoff_e2, phioff_b2, phioff_e2, update, time, listBadChannel

    clusterTypes = dict(
      Ele35='ele35', Ele55='ele55', Ele37='ele37',
      Gam35='gam35_unconv', Gam55='gam55_unconv', Gam37='gam37_unconv',
      Econv35='gam35_conv', Econv55='gam55_conv', Econv37='gam37_conv'
    )
    #TODO expers check if this logic here is correct (i.e. we configure either one or the other)
    if not flags.Egamma.doSuperclusters:
        for attrName, clName in clusterTypes.items():
            attrName = 'ClusterCorrectionTools' + attrName
            if not hasattr(swTool, attrName):
                continue
            toolAcc = make_CaloSwCorrectionsCfg(flags, clName, suffix='EG',
                                                version=flags.Egamma.Calib.ClusterCorrectionVersion,
                                                cells_name=flags.Egamma.Keys.Input.CaloCells)
            tool = toolAcc.popPrivateTools()
            acc.merge(toolAcc)
            setattr( swTool, attrName, GaudiHandles.PrivateToolHandleArray(tool) )

    # Super cluster position only corrections
    if flags.Egamma.doSuperclusters:
        for attrName, clName in clusterTypes.items():
            attrName = 'ClusterCorrectionToolsSuperCluster' + attrName
            if not hasattr(swTool, attrName):
                continue
            toolsAcc = _desiredCorrections(flags, clusterType=clName, suffix='EGSuperCluster')
            #TODO once debugged we may restore the old way of doing the config
            # toolsAcc = make_CaloSwCorrectionsCfg(flags, clName, suffix='EGSuperCluster',
            #                                             corrlist=[[rfac, 'v5'], [etaoff_b1, 'v5'], [etaoff_e1, 'v5'],
            #                                                       [etaoff_b2, 'v5'], [etaoff_e2, 'v5'], [phioff_b2, 'v5data'],
            #                                                       [phioff_e2, 'v5data'], [update], [time], [listBadChannel]],
            #                                             cells_name=flags.Egamma.Keys.Input.CaloCells) #TODO hack to assure we use COOL for data
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


if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files = defaultTestFiles.RAW
    flags.lock()

    acc = _desiredCorrections(flags, 'ele37')
    acc.printConfig(withDetails=True, summariseProps=True)
    acc.popPrivateTools()
    acc.wasMerged()
