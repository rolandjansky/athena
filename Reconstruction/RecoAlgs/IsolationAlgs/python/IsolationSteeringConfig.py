# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

__doc__ = """
          Instantiate the isolation
          """

from AthenaCommon.Logging import logging
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def IsolationSteeringCfg(flags, name = 'IsolationSteering', doIsoMuon = True):

    mlog = logging.getLogger(name)
    mlog.info('Starting Isolation steering')

    acc = ComponentAccumulator()

    from IsolationAlgs.IsoDensityConfig import (
        EMTopoInputAlgCfg, NFlowInputAlgCfg, DensityForIsoAlgCfg)
    if flags.Reco.EnableEgamma or flags.Reco.Reco.EnableCombinedMuon:
        if flags.Detector.EnableCalo:
            acc.merge(EMTopoInputAlgCfg(flags))
            acc.merge(DensityForIsoAlgCfg(flags))
            acc.merge(DensityForIsoAlgCfg(flags,name='ForwardDensityForTopoIso'))
        if flags.Reco.EnablePFlow:
            acc.merge(NFlowInputAlgCfg(flags))
            acc.merge(DensityForIsoAlgCfg(flags,name='CentralDensityForNFlowIso'))
            acc.merge(DensityForIsoAlgCfg(flags,name='ForwardDensityForNFlowIso'))

    from IsolationAlgs.IsolationBuilderConfig import egIsolationCfg, muIsolationCfg
    if flags.Reco.EnableEgamma:
        acc.merge(egIsolationCfg(flags,name = 'photonIsolation'))
        acc.merge(egIsolationCfg(flags,name = 'electronIsolation'))
    if flags.Reco.EnableCombinedMuon and doIsoMuon:
        acc.merge(muIsolationCfg(flags,name = 'muonIsolation'))

    # To use isolation CA within standard config
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
        for el in acc._allSequences:
            el.name = "TopAlg"

    return acc

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = True
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    flags.Input.Files = defaultTestFiles.RDO
    flags.Output.doWriteESD = True  # To test the ESD parts
    flags.Output.doWriteAOD = True  # To test the AOD parts
    flags.lock()

    mlog = logging.getLogger("isolationConfigTest")
    mlog.info("Configuring isolation: ")

    acc = MainServicesCfg(flags)
    acc.merge(IsolationSteeringCfg(flags))
    acc.printConfig(withDetails=True,
                    printDefaults=True)
    printProperties(mlog,
                    acc.getEventAlgo('photonIsolationBuilder'),
                    nestLevel=1,
                    printDefaults=True)

    with open("isolationconfig.pkl", "wb") as f:
        acc.store(f)
