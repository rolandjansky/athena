# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from ForwardRegionMgField.ForwardRegionMgFieldConf import MagField__ForwardRegionFieldSvc

def Q1FieldSvcCfg(name="Q1", **kwargs):
    kwargs.setdefault("Magnet", 0) # FIXME find a better way to do this.
    kwargs.setdefault("MQXA_DataFile", "MQXA_NOMINAL.dat")
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q2FieldSvcCfg(name="Q2", **kwargs):
    kwargs.setdefault("Magnet", 1) # FIXME find a better way to do this.
    kwargs.setdefault("MQXB_DataFile", "MQXB_NOMINAL.dat")
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q3FieldSvcCfg(name="Q3", **kwargs):
    kwargs.setdefault("Magnet", 2) # FIXME find a better way to do this.
    kwargs.setdefault("MQXA_DataFile", "MQXA_NOMINAL.dat")
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def D1FieldSvcCfg(name="D1", **kwargs):
    kwargs.setdefault("Magnet", 3) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def D2FieldSvcCfg(name="D2", **kwargs):
    kwargs.setdefault("Magnet", 4) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4FieldSvcCfg(name="Q4", **kwargs):
    kwargs.setdefault("Magnet", 5) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q5FieldSvcCfg(name="Q5", **kwargs):
    kwargs.setdefault("Magnet", 6) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q6FieldSvcCfg(name="Q6", **kwargs):
    kwargs.setdefault("Magnet", 7) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q7FieldSvcCfg(name="Q7", **kwargs):
    kwargs.setdefault("Magnet", 8) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q1hkickFieldSvcCfg(name="Q1HKick", **kwargs):
    kwargs.setdefault("Magnet", 9) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q1vkickFieldSvcCfg(name="Q1VKick", **kwargs):
    kwargs.setdefault("Magnet", 10) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q2hkickFieldSvcCfg(name="Q2HKick", **kwargs):
    kwargs.setdefault("Magnet", 11) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q2vkickFieldSvcCfg(name="Q2VKick", **kwargs):
    kwargs.setdefault("Magnet", 12) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q3hkickFieldSvcCfg(name="Q3HKick", **kwargs):
    kwargs.setdefault("Magnet", 13) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q3vkickFieldSvcCfg(name="Q3VKick", **kwargs):
    kwargs.setdefault("Magnet", 14) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4vkickAFieldSvcCfg(name="Q4VKickA", **kwargs):
    kwargs.setdefault("Magnet", 15) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4hkickFieldSvcCfg(name="Q4HKick", **kwargs):
    kwargs.setdefault("Magnet", 16) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q4vkickBFieldSvcCfg(name="Q4VKickB", **kwargs):
    kwargs.setdefault("Magnet", 17) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q5hkickFieldSvcCfg(name="Q5HKick", **kwargs):
    kwargs.setdefault("Magnet", 18) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
def Q6vkickFieldSvcCfg(name="Q6VKick", **kwargs):
    kwargs.setdefault("Magnet", 19) # FIXME find a better way to do this.
    return MagField__ForwardRegionFieldSvc(name, **kwargs)
