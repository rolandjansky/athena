#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

"""
@brief configuration for the min bias monitoring
"""
from TrigMinBiasMonitoring.TrigMBTSMonitoringMT import TrigMBTS
from TrigMinBiasMonitoring.TrigSPTRKMonitoringMT import TrigSPTRK
from TrigMinBiasMonitoring.TrigMinBiasEffMonitoring import TrigMinBiasEff
from TrigMinBiasMonitoring.TrigAFPSidHypoMonitoring import TrigAFPSidHypoMonitoring

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import BeamType


def TrigMinBias(configFlags):
    acc = ComponentAccumulator()
    acc.merge(TrigMBTS(configFlags))
    if configFlags.Beam.Type is not BeamType.Cosmics:
        acc.merge(TrigSPTRK(configFlags))
        acc.merge(TrigMinBiasEff(configFlags))
        acc.merge(TrigAFPSidHypoMonitoring(configFlags))
    return acc


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags

    flags.DQ.Environment = "AOD"
    flags.Output.HISTFileName = 'TestMinBiasMonitorOutput.root'
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    flags.Input.Files = defaultTestFiles.AOD
    flags.Exec.MaxEvents = -1
    # flags.Common.doExpressProcessing=True # enable for express stream data processing
    flags.fillFromArgs()
    flags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg = MainServicesCfg(flags)

    cfg.merge(PoolReadCfg(flags))
    cfg.merge(TrigMinBias(flags))

    with open("cfg.pkl", "wb") as f:
        cfg.store(f)

    status = cfg.run()
    if status.isFailure():
        import sys
        sys.exit(-1)
