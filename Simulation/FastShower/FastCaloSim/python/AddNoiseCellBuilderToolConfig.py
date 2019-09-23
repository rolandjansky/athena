# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# File: FastCaloSim/python/AddNoiseCellBuilderTool.py
# Created: Aug 2019, sss
# Purpose: Configure AddNoiseCellBuilderTool.
#


from __future__ import print_function
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def AddNoiseCellBuilderToolCfg (configFlags):
    result = ComponentAccumulator()

    from CaloTools.CaloEstimatedGainToolConfig import CaloEstimatedGainToolCfg
    acc = CaloEstimatedGainToolCfg (configFlags)
    estimatedGainTool = acc.popPrivateTools()
    result.merge (acc)

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge (CaloNoiseCondAlgCfg (configFlags, 'electronicNoise'))

    from FastCaloSim.FastCaloSimConf import AddNoiseCellBuilderTool
    tool = AddNoiseCellBuilderTool ('AddNoiseCellBuilderTool',
                                    NoiseKey = 'electronicNoise',
                                    CaloEstimatedGainTool = estimatedGainTool)
    result.setPrivateTools (tool)

    return result



if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RDO
    flags1.lock()
    acc1 = AddNoiseCellBuilderToolCfg (flags1)
    only = ['AddNoiseCellBuilderTool',
            'CaloNoiseCondAlg-']
    acc1.printCondAlgs(summariseProps=True, onlyComponents = only)
    print ('ComponentAccumulator:', acc1.popPrivateTools())
    acc1.wasMerged()
