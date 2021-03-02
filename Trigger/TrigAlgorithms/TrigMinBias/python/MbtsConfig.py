# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from TrigMinBias.TrigMinBiasConf import MbtsFex
from TrigMinBias.TrigMinBiasMonitoring import MbtsFexMonitoring


def MbtsFexCfg(flags=None, name="MbtsFex", **kwargs):
    alg = MbtsFex(name, **kwargs)
    alg.MonTool =  MbtsFexMonitoring()
    from TrigT2CaloCommon.TrigCaloDataAccessConfig import CaloDataAccessSvcDependencies
    alg.ExtraInputs = CaloDataAccessSvcDependencies
    return alg


