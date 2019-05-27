# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from G4AtlasServices.G4AtlasServicesConf import StandardFieldSvc

from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg

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
    kwargs.setdefault("MagneticFieldSvc",           "Q1")
    return StandardFieldSvc(name, **kwargs)
def Q2FwdG4FieldSvcCfg(ConfigFlags, name='Q2FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2")
    return StandardFieldSvc(name, **kwargs)
def Q3FwdG4FieldSvcCfg(ConfigFlags, name='Q3FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3")
    return StandardFieldSvc(name, **kwargs)
def D1FwdG4FieldSvcCfg(ConfigFlags, name='D1FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "D1")
    return StandardFieldSvc(name, **kwargs)
def D2FwdG4FieldSvcCfg(ConfigFlags, name='D2FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "D2")
    return StandardFieldSvc(name, **kwargs)
def Q4FwdG4FieldSvcCfg(ConfigFlags, name='Q4FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4")
    return StandardFieldSvc(name, **kwargs)
def Q5FwdG4FieldSvcCfg(ConfigFlags, name='Q5FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q5")
    return StandardFieldSvc(name, **kwargs)
def Q6FwdG4FieldSvcCfg(ConfigFlags, name='Q6FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q6")
    return StandardFieldSvc(name, **kwargs)
def Q7FwdG4FieldSvcCfg(ConfigFlags, name='Q7FwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q7")
    return StandardFieldSvc(name, **kwargs)
def Q1HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1HKick")
    return StandardFieldSvc(name, **kwargs)
def Q1VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q1VKick")
    return StandardFieldSvc(name, **kwargs)
def Q2HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2HKick")
    return StandardFieldSvc(name, **kwargs)
def Q2VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q2VKick")
    return StandardFieldSvc(name, **kwargs)
def Q3HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3HKick")
    return StandardFieldSvc(name, **kwargs)
def Q3VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q3VKick")
    return StandardFieldSvc(name, **kwargs)
def Q4VKickAFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickAFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4VKickA")
    return StandardFieldSvc(name, **kwargs)
def Q4HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q4HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4HKick")
    return StandardFieldSvc(name, **kwargs)
def Q4VKickBFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickBFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q4VKickB")
    return StandardFieldSvc(name, **kwargs)
def Q5HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q5HKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q5HKick")
    return StandardFieldSvc(name, **kwargs)
def Q6VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q6VKickFwdG4FieldSvc', **kwargs):
    kwargs.setdefault("MagneticFieldSvc",           "Q6VKick")
    return StandardFieldSvc(name, **kwargs)
