# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from G4AtlasServices.G4AtlasServicesConf import StandardFieldSvc

from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

from ForwardRegionMgField.ForwardRegionMgFieldConfigNew import Q1FieldSvcCfg, Q2FieldSvcCfg, Q3FieldSvcCfg, D1FieldSvcCfg, D2FieldSvcCfg, Q4FieldSvcCfg, Q5FieldSvcCfg, Q6FieldSvcCfg, Q7FieldSvcCfg, Q1hkickFieldSvcCfg, Q1vkickFieldSvcCfg, Q2hkickFieldSvcCfg, Q2vkickFieldSvcCfg, Q3hkickFieldSvcCfg, Q3vkickFieldSvcCfg, Q4vkickAFieldSvcCfg, Q4hkickFieldSvcCfg, Q4vkickBFieldSvcCfg, Q5hkickFieldSvcCfg, Q6vkickFieldSvcCfg

def StandardFieldSvcCfg(ConfigFlags,name="StandardField", **kwargs):
    result = ComponentAccumulator()

    #setup the field and add the magneticfield service
    acc = MagneticFieldSvcCfg(ConfigFlags)
    result.merge(acc)

    kwargs.setdefault("MagneticFieldSvc", result.getService("AtlasFieldSvc")) # TODO This should probably be based on simFlags.MagneticField?
    #kwargs.setdefault("FieldOn", True)

    result.addService(StandardFieldSvc(name, **kwargs))
    return result

def ForwardFieldSvcCfg(ConfigFlags, name="ForwardField", **kwargs):
    result = ComponentAccumulator()

    #setup the field and add the magneticfield service
    acc = MagneticFieldSvcCfg(ConfigFlags)
    result.merge(acc)

    #FIXME Once it exists this version should use the new MagField Service defined in ForwardRegionMgField
    kwargs.setdefault("MagneticFieldSvc", result.getService("AtlasFieldSvc"))
    #kwargs.setdefault("FieldOn", True)

    result.addService(StandardFieldSvc(name, **kwargs))
    return result

def Q1FwdG4FieldSvcCfg(ConfigFlags, name='Q1FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q1FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q2FwdG4FieldSvcCfg(ConfigFlags, name='Q2FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q2FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q3FwdG4FieldSvcCfg(ConfigFlags, name='Q3FwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q3FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def D1FwdG4FieldSvcCfg(ConfigFlags, name='D1FwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           D1FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def D2FwdG4FieldSvcCfg(ConfigFlags, name='D2FwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           D2FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q4FwdG4FieldSvcCfg(ConfigFlags, name='Q4FwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q4FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q5FwdG4FieldSvcCfg(ConfigFlags, name='Q5FwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q5FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q6FwdG4FieldSvcCfg(ConfigFlags, name='Q6FwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q6FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q7FwdG4FieldSvcCfg(ConfigFlags, name='Q7FwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q7FieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q1HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1HKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q1hkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q1VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1VKickFwdG4FieldSvc', **kwargs): #note is lower case "v" in ForwardRegionMgFieldConfig.py
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q1vkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q2HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2HKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q2hkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q2VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2VKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q2vkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q3HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3HKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q3hkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q3VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3VKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q3vkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q4VKickAFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickAFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q4vkickAFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q4HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q4HKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q4hkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q4VKickBFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickBFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q4vkickBFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q5HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q5HKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q5hkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result
def Q6VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q6VKickFwdG4FieldSvc', **kwargs): 
    result = ComponentAccumulator()
    kwargs.setdefault("MagneticFieldSvc",           Q6vkickFieldSvcCfg(ConfigFlags))
    result.addService(StandardFieldSvc(name, **kwargs))
    return result