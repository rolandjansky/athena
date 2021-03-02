# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TrigMinBias.TrigMinBiasConf import MbtsFexMT
from TrigMinBias.TrigMinBiasMonitoring import MbtsFexMTMonitoring


def MbtsFexMTCfg(flags=None, name="MbtsFex", **kwargs):
    alg = MbtsFexMT(name, **kwargs)
    alg.MonTool =  MbtsFexMTMonitoring()
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import CaloDataAccessSvcDependencies
    alg.ExtraInputs = CaloDataAccessSvcDependencies
    return alg


