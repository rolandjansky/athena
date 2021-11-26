# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator



def HIRecCfg(flags):
    '''Configures entire HI reconstruction'''
    acc = ComponentAccumulator()

    if flags.HI.doGlobal:
        acc.merge(HIGlobalRecCfg(flags))

    if flags.HI.doJet:
        acc.merge(HIJetRecCfg(flags))

    if flags.HI.doEgamma:
        acc.merge(HIEgammaCfg(flags))
    # ... to add more
    return acc


def HIGlobalRecCfg(flags):
    acc = ComponentAccumulator()
    return acc


def HIJetRecCfg(flags):
    acc = ComponentAccumulator()    
    return acc

def HIEgammaCfg():
    acc = ComponentAccumulator()
    # this fragment should pull in egamma reco: i.e. like here: 
    from egammaConfig.egammaSteeringConfig import EGammaSteeringCfg
    acc.merge(EGammaSteeringCfg(flags))
    # add here HI specific parts
    
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    #TODO  need to redecide upon which data the test should run
    flags.Input.File = defaultTestFiles.d + "/TrigP1Test/data16_hip5TeV.00312649.physics_EnhancedBias.merge.RAW._lb0150._SFO-ALL._0001.data"
    flags.Exec.MaxEvents=5
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)
    acc.merge(HIRecCfg(flags))
    status = acc.run()
    if status.isFailure():
        import sys
        sys.exit(-1)