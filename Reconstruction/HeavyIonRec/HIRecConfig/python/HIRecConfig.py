# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator



def HIRecCfg(flags):
    '''Configures entire HI reconstruction'''
    acc = ComponentAccumulator()

    if flags.HeavyIon.doGlobal:
        from HIGlobal.HIGlobalConfig import HIGlobalRecCfg
        acc.merge(HIGlobalRecCfg(flags))

    if flags.HeavyIon.doJet:
        #        acc.merge(HIJetRecCfg(flags))
        pass

    if flags.HeavyIon.doEgamma:
        #        acc.merge(HIEgammaCfg(flags))
        pass
    # ... to add more
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

    flags.Input.Files = [defaultTestFiles.d + "/RecJobTransformTests/data18_hi.00367384.physics_HardProbes.daq.RAW._lb0145._SFO-8._0001.data"]
    flags.Exec.MaxEvents=5
    flags.Concurrency.NumThreads=1

    flags.fillFromArgs() # enable unit tests to switch only parts of reco: python -m HIRecConfig.HIRecConfig HeavyIon.doGlobal = 0 and so on
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)
    #      acc.getEventAlgo("SGInputLoader").FailIfNoProxy = True # enforce no missing data
#    acc.getService("PrecedenceSvc").DumpPrecedenceRules = True
    from TriggerJobOpts.TriggerByteStreamConfig import ByteStreamReadCfg
    readBSAcc = ByteStreamReadCfg(flags)
    acc.merge(HIRecCfg(flags))
    status = acc.run()
    if status.isFailure():
        import sys
        sys.exit(-1)