# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from ForwardRegionMgField.ForwardRegionMgFieldConf import MagField__ForwardRegionFieldSvc

def Q1FieldSvcCfg(ConfigFlags, name="Q1", **kwargs):
    kwargs.setdefault("Magnet", 0) # FIXME find a better way to do this.
    kwargs.setdefault("MQXA_DataFile", "MQXA_NOMINAL.dat")
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q2FieldSvcCfg(ConfigFlags, name="Q2", **kwargs):
    kwargs.setdefault("Magnet", 1) # FIXME find a better way to do this.
    kwargs.setdefault("MQXB_DataFile", "MQXB_NOMINAL.dat")
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q3FieldSvcCfg(ConfigFlags, name="Q3", **kwargs):
    kwargs.setdefault("Magnet", 2) # FIXME find a better way to do this.
    kwargs.setdefault("MQXA_DataFile", "MQXA_NOMINAL.dat")
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def D1FieldSvcCfg(ConfigFlags, name="D1", **kwargs):
    kwargs.setdefault("Magnet", 3) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def D2FieldSvcCfg(ConfigFlags, name="D2", **kwargs):
    kwargs.setdefault("Magnet", 4) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4FieldSvcCfg(ConfigFlags, name="Q4", **kwargs):
    kwargs.setdefault("Magnet", 5) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q5FieldSvcCfg(ConfigFlags, name="Q5", **kwargs):
    kwargs.setdefault("Magnet", 6) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q6FieldSvcCfg(ConfigFlags, name="Q6", **kwargs):
    kwargs.setdefault("Magnet", 7) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q7FieldSvcCfg(ConfigFlags, name="Q7", **kwargs):
    kwargs.setdefault("Magnet", 8) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q1hkickFieldSvcCfg(ConfigFlags, name="Q1HKick", **kwargs):
    kwargs.setdefault("Magnet", 9) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q1vkickFieldSvcCfg(ConfigFlags, name="Q1VKick", **kwargs):
    kwargs.setdefault("Magnet", 10) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q2hkickFieldSvcCfg(ConfigFlags, name="Q2HKick", **kwargs):
    kwargs.setdefault("Magnet", 11) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q2vkickFieldSvcCfg(ConfigFlags, name="Q2VKick", **kwargs):
    kwargs.setdefault("Magnet", 12) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q3hkickFieldSvcCfg(ConfigFlags, name="Q3HKick", **kwargs):
    kwargs.setdefault("Magnet", 13) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q3vkickFieldSvcCfg(ConfigFlags, name="Q3VKick", **kwargs):
    kwargs.setdefault("Magnet", 14) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4vkickAFieldSvcCfg(ConfigFlags, name="Q4VKickA", **kwargs):
    kwargs.setdefault("Magnet", 15) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4hkickFieldSvcCfg(ConfigFlags, name="Q4HKick", **kwargs):
    kwargs.setdefault("Magnet", 16) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4vkickBFieldSvcCfg(ConfigFlags, name="Q4VKickB", **kwargs):
    kwargs.setdefault("Magnet", 17) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q5hkickFieldSvcCfg(ConfigFlags, name="Q5HKick", **kwargs):
    kwargs.setdefault("Magnet", 18) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q6vkickFieldSvcCfg(ConfigFlags, name="Q6VKick", **kwargs):
    kwargs.setdefault("Magnet", 19) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
