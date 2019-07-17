# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## Configuration Access to OFFLINE DB (COMP200)

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from MuonCondAlg.MuonTopCondAlgConfigRUN2.py import MdtCondDbAlg,RpcCondDbAlg,CscCondDbAlg,TgcCondDbAlg

def MdtCondDbAlg(flags, **kwargs):
    result = ComponentAccumulator()
    alg    = MdtCondDbAlg(**kwargs)
    result.addCondAlgo(alg)
    return result, alg

def RpcCondDbAlg(flags, **kwargs):
    result = ComponentAccumulator()
    alg    = RpcCondDbAlg(**kwargs)
    result.addCondAlgo(alg)
    return result, alg

def CscCondDbAlg(flags, **kwargs):
    result = ComponentAccumulator()
    alg    = CscCondDbAlg(**kwargs)
    result.addCondAlgo(alg)
    return result, alg

def TgcCondDbAlg(flags, **kwargs):
    result = ComponentAccumulator()
    alg    = TgcCondDbAlg(**kwargs)
    result.addCondAlgo(alg)
    return result, alg

