#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

"""
@brief configuration for the min bias monitoring
"""
from TrigMinBiasMonitoring.TrigMBTSMonitoringMT import TrigMBTS
from TrigMinBiasMonitoring.TrigSPTRKMonitoringMT import TrigSPTRK
from TrigMinBiasMonitoring.TrigEffMonitoring import TrigMinBiasEff
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator


def TrigMinBias(configFlags):
    acc = ComponentAccumulator()
    acc.merge(TrigSPTRK(configFlags))
    acc.merge(TrigMBTS(configFlags))
    acc.merge(TrigMinBiasEff(configFlags))
    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable

    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    flags.DQ.Environment = "AOD"
    flags.Output.HISTFileName = 'TestMinBiasMonitorOutput.root'
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    flags.Input.Files = defaultTestFiles.AOD
    flags.Exec.MaxEvents = 20
    flags.fillFromArgs()
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(flags)
    cfg.merge(PoolReadCfg(flags))
    cfg.merge(TrigMinBias(flags))

    status = cfg.run()
    if status.isFailure():
        import sys
        sys.exit(-1)