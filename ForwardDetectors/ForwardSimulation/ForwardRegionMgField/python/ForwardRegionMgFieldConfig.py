# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getQ1FieldSvc(name="Q1", **kwargs):
    kwargs.setdefault("Magnet", 0) # FIXME find a better way to do this.
    kwargs.setdefault("MQXA_DataFile", "MQXA_NOMINAL.dat")
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ2FieldSvc(name="Q2", **kwargs):
    kwargs.setdefault("Magnet", 1) # FIXME find a better way to do this.
    kwargs.setdefault("MQXB_DataFile", "MQXB_NOMINAL.dat")
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ3FieldSvc(name="Q3", **kwargs):
    kwargs.setdefault("Magnet", 2) # FIXME find a better way to do this.
    kwargs.setdefault("MQXA_DataFile", "MQXA_NOMINAL.dat")
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getD1FieldSvc(name="D1", **kwargs):
    kwargs.setdefault("Magnet", 3) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getD2FieldSvc(name="D2", **kwargs):
    kwargs.setdefault("Magnet", 4) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ4FieldSvc(name="Q4", **kwargs):
    kwargs.setdefault("Magnet", 5) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ5FieldSvc(name="Q5", **kwargs):
    kwargs.setdefault("Magnet", 6) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ6FieldSvc(name="Q6", **kwargs):
    kwargs.setdefault("Magnet", 7) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ7FieldSvc(name="Q7", **kwargs):
    kwargs.setdefault("Magnet", 8) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ1hkickFieldSvc(name="Q1HKick", **kwargs):
    kwargs.setdefault("Magnet", 9) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ1vkickFieldSvc(name="Q1VKick", **kwargs):
    kwargs.setdefault("Magnet", 10) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ2hkickFieldSvc(name="Q2HKick", **kwargs):
    kwargs.setdefault("Magnet", 11) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ2vkickFieldSvc(name="Q2VKick", **kwargs):
    kwargs.setdefault("Magnet", 12) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ3hkickFieldSvc(name="Q3HKick", **kwargs):
    kwargs.setdefault("Magnet", 13) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ3vkickFieldSvc(name="Q3VKick", **kwargs):
    kwargs.setdefault("Magnet", 14) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ4vkickAFieldSvc(name="Q4VKickA", **kwargs):
    kwargs.setdefault("Magnet", 15) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ4hkickFieldSvc(name="Q4HKick", **kwargs):
    kwargs.setdefault("Magnet", 16) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ4vkickBFieldSvc(name="Q4VKickB", **kwargs):
    kwargs.setdefault("Magnet", 17) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ5hkickFieldSvc(name="Q5HKick", **kwargs):
    kwargs.setdefault("Magnet", 18) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
def getQ6vkickFieldSvc(name="Q6VKick", **kwargs):
    kwargs.setdefault("Magnet", 19) # FIXME find a better way to do this.
    return CfgMgr.MagField__ForwardRegionFieldSvc(name, **kwargs)
