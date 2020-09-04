# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
StandardFieldSvc=CompFactory.StandardFieldSvc

#to prevent unit tests failing when just running over simulation
import os
if "AthSimulation_DIR" not in os.environ:
    MagField__ForwardRegionFieldSvc=CompFactory.MagField.ForwardRegionFieldSvc

def StandardFieldSvcCfg(ConfigFlags,name="StandardField", **kwargs):
    result = ComponentAccumulator()

    result.addService(StandardFieldSvc(name, **kwargs))
    return result

def ForwardFieldSvcCfg(ConfigFlags, name="ForwardField", **kwargs):
    result = ComponentAccumulator()

    # #FIXME Once it exists this version should use the new MagField Service defined in ForwardRegionMgField
    # kwargs.setdefault("MagneticFieldSvc", result.getService("AtlasFieldSvc"))
    # #kwargs.setdefault("FieldOn", True)
    kwargs.setdefault("UseMagFieldSvc", True)
    result.addService(StandardFieldSvc(name, **kwargs))
    return result

def Q1FwdG4FieldSvcCfg(ConfigFlags, name='Q1FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()

    result.addService( MagField__ForwardRegionFieldSvc("Q1",
                                                        Magnet = 0, # FIXME find a better way to do this.
                                                        MQXA_DataFile = "MQXA_NOMINAL.dat"))

    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q1"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q2FwdG4FieldSvcCfg(ConfigFlags, name='Q2FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()

    result.addService( MagField__ForwardRegionFieldSvc("Q2",
                                                        Magnet = 1, # FIXME find a better way to do this.
                                                        MQXA_DataFile = "MQXA_NOMINAL.dat"))
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q2"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q3FwdG4FieldSvcCfg(ConfigFlags, name='Q3FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()

    result.addService( MagField__ForwardRegionFieldSvc("Q3",
                                                        Magnet = 2, # FIXME find a better way to do this.
                                                        MQXA_DataFile = "MQXA_NOMINAL.dat"))
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q3"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def D1FwdG4FieldSvcCfg(ConfigFlags, name='D1FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("D1",
                                                        Magnet = 3))# FIXME find a better way to do this.

    kwargs.setdefault("MagneticFieldSvc",           result.getService("D1"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def D2FwdG4FieldSvcCfg(ConfigFlags, name='D2FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()

    result.addService( MagField__ForwardRegionFieldSvc("D2",
                                                        Magnet = 4))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("D2"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q4FwdG4FieldSvcCfg(ConfigFlags, name='Q4FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q4",
                                                        Magnet = 5))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q4"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q5FwdG4FieldSvcCfg(ConfigFlags, name='Q5FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q5",
                                                        Magnet = 6))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q5"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q6FwdG4FieldSvcCfg(ConfigFlags, name='Q6FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q6",
                                                        Magnet = 7))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q6"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q7FwdG4FieldSvcCfg(ConfigFlags, name='Q7FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q7",
                                                        Magnet = 8))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q7"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q1HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q1HKick",
                                                        Magnet = 9))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q1HKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q1VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1VKickFwdG4FieldSvc', **kwargs): #note is lower case "v" in ForwardRegionMgFieldConfig.py
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q1VKick",
                                                        Magnet = 10))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q1VKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q2HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q2HKick",
                                                        Magnet = 11))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q2HKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q2VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2VKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q2VKick",
                                                        Magnet = 12))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q2VKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q3HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q3HKick",
                                                        Magnet = 13))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q3HKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q3VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3VKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q3VKick",
                                                        Magnet = 14))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q3VKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q4VKickAFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickAFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q4VKickA",
                                                        Magnet = 15))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q4VKickA"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q4HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q4HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q4HKick",
                                                        Magnet = 16))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q4HKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q4VKickBFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickBFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q4VKickB",
                                                        Magnet = 17))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q4VKickB"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q5HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q5HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q5HKick",
                                                        Magnet = 18))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q5HKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result

def Q6VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q6VKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    result.addService( MagField__ForwardRegionFieldSvc("Q6VKick",
                                                        Magnet = 19))# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc",           result.getService("Q6VKick"))
    result.merge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    return result
