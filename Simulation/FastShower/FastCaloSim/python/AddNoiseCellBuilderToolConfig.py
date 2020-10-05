# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# File: FastCaloSim/python/AddNoiseCellBuilderTool.py
# Created: Aug 2019, sss
# Purpose: Configure AddNoiseCellBuilderTool.
#


from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def AddNoiseCellBuilderToolCfg (configFlags):
    result = ComponentAccumulator()

    from CaloTools.CaloEstimatedGainToolConfig import CaloEstimatedGainToolCfg
    acc = CaloEstimatedGainToolCfg (configFlags)
    estimatedGainTool = acc.popPrivateTools()
    result.merge (acc)

    from CaloTools.CaloNoiseCondAlgConfig import CaloNoiseCondAlgCfg
    result.merge (CaloNoiseCondAlgCfg (configFlags, 'electronicNoise'))

    AddNoiseCellBuilderTool=CompFactory.AddNoiseCellBuilderTool
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

    # Print a configurable with properties in sorted order.
    Configurable = CompFactory.AddNoiseCellBuilderTool.__bases__[0]
    def sorted_repr (c):
        if not isinstance(c, Configurable): return str(c)
        args = [c.name] + ['{}={!r}'.format(item[0], sorted_repr(item[1])) for item in sorted(c._properties.items())]
        return '{}({})'.format(type(c).__name__, ', '.join(args))

    print ('ComponentAccumulator:', sorted_repr (acc1.popPrivateTools()))

    acc1.wasMerged()
