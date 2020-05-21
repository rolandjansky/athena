# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# File: LumiBlockComps/python/LumiBlockMuWriterConfig.py
# Created: May 2020, sss
# Purpose: Configure LumiBlockMuWriter.
#


from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def LumiBlockMuWriterCfg (configFlags, name = 'LumiBlockMuWriter'):
    result = ComponentAccumulator()

    if configFlags.Beam.Type == 'cosmics':
        condkey = ''
    else:
        from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
        result.merge (LuminosityCondAlgCfg (configFlags))
        condkey = result.getCondAlgo ('LuminosityCondAlg').LuminosityOutputKey

    LumiBlockMuWriter = CompFactory.LumiBlockMuWriter # LumiBlockComps
    alg = LumiBlockMuWriter (name, LumiDataKey = condkey)
    result.addCondAlgo (alg)
    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.loadAllDynamicFlags()

    print ('--- collisions')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = defaultTestFiles.RAW
    flags1.lock()
    acc1 = LumiBlockMuWriterCfg (flags1)
    acc1.printCondAlgs (summariseProps=True)
    acc1.wasMerged()

    print ('--- cosmics')
    flags2 = ConfigFlags.clone()
    flags2.Beam.Type = 'cosmics'
    flags2.lock()
    acc2 = LumiBlockMuWriterCfg (flags2)
    acc2.printCondAlgs (summariseProps=True)
    acc2.wasMerged()
