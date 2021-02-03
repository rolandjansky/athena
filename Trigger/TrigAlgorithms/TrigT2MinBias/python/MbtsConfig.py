# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from TrigT2MinBias.TrigT2MinBiasConf import MbtsFexMT
from TrigT2MinBias.TrigT2MinBiasMonitoringMT import MbtsFexMTMonitoring


def MbtsFexMTCfg(flags=None, name="MbtsFex", **kwargs):
    alg = MbtsFexMT(name, **kwargs)
    alg.MonTool =  MbtsFexMTMonitoring()
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import CaloDataAccessSvcDependencies
    alg.ExtraInputs = CaloDataAccessSvcDependencies
    return alg


