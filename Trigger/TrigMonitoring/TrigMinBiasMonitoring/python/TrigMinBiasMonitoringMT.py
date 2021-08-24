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
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    flags.Input.Files = defaultTestFiles.AOD
    flags.lock()

    acc = TrigMinBias(flags)
    acc.printConfig(withDetails=True, summariseProps=True)
