"""Define methods to construct configured TRT_PAI_Process tools

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""
#from TRT_PAI_Process.TRT_PAI_ProcessConf import 
from AthenaConfiguration.ComponentFactory import CompFactory
TRT_PAI_Process=CompFactory.TRT_PAI_Process

def TRT_PAI_Process_XeToolCfg(flags, name="TRT_PAI_Process_Xe", **kwargs):
    """Return a Xenon-configured TRT_PAI_Process"""
    kwargs.setdefault("GasType", "Xenon")
    return TRT_PAI_Process(name, **kwargs)

def TRT_PAI_Process_ArToolCfg(flags, name="TRT_PAI_Process_Ar", **kwargs):
    """Return an Argon-configured TRT_PAI_Process"""
    kwargs.setdefault("GasType", "Argon")
    return TRT_PAI_Process(name, **kwargs)

def TRT_PAI_Process_KrToolCfg(flags, name="TRT_PAI_Process_Kr", **kwargs):
    """Return a Krypton-configured TRT_PAI_Process"""
    kwargs.setdefault("GasType", "Krypton")
    return TRT_PAI_Process(name, **kwargs)

