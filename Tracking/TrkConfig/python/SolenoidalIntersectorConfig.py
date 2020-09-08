# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def SolenoidalIntersectorCfg(flags, name='SolenoidalIntersector', **kwargs):
    result = ComponentAccumulator()

    cls = CompFactory.Trk.SolenoidParametrizationCondAlg # TrkExSolenoidalIntersector
    condalg = cls (AtlasFieldCacheCondObj = 'fieldCondObj',
                   WriteKey = 'SolenoidParametrization')
    result.addCondAlgo (condalg)

    kwargs.setdefault ('SolenoidParameterizationKey', 'SolenoidParametrization')
    cls = CompFactory.Trk.SolenoidalIntersector # TrkExSolenoidalIntersector
    tool = cls (name, **kwargs)
    result.setPrivateTools (tool)
    return result


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.loadAllDynamicFlags()

    flags1 = ConfigFlags.clone()
    acc1 = SolenoidalIntersectorCfg (flags1)
    print ('private tools:', acc1.popPrivateTools())
    acc1.printConfig (summariseProps=True)
    acc1.wasMerged()
