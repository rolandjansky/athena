# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TRT_PAI_Process.TRT_PAI_ProcessConf import TRT_PAI_Process

def getTRT_PAI_Process_Xe(name="TRT_PAI_Process_Xe", **kwargs):
    return TRT_PAI_Process(name, **kwargs)


def getTRT_PAI_Process_Ar(name="TRT_PAI_Process_Ar", **kwargs):
    kwargs.setdefault("GasType", "Argon")
    return TRT_PAI_Process(name, **kwargs)


def getTRT_PAI_Process_Kr(name="TRT_PAI_Process_Kr", **kwargs):
    kwargs.setdefault("GasType", "Krypton")
    return TRT_PAI_Process(name, **kwargs)
