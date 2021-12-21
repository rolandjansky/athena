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
    result.addService(StandardFieldSvc(name, **kwargs), primary = True)
    return result


def ForwardFieldSvcCfg(ConfigFlags, name="ForwardField", **kwargs):
    result = ComponentAccumulator()
    # #FIXME Once it exists this version should use the new MagField Service defined in ForwardRegionMgField
    # kwargs.setdefault("MagneticFieldSvc", result.getService("AtlasFieldSvc"))
    # #kwargs.setdefault("FieldOn", True)
    kwargs.setdefault("UseMagFieldSvc", True)
    result.addService(StandardFieldSvc(name, **kwargs), primary = True)
    return result


def Q1FwdG4FieldSvcCfg(ConfigFlags, name='Q1FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q1",
                                                        Magnet = 0, # FIXME find a better way to do this.
                                                        MQXA_DataFile = "MQXA_NOMINAL.dat"), primary = True)

    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q2FwdG4FieldSvcCfg(ConfigFlags, name='Q2FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q2",
                                                        Magnet = 1, # FIXME find a better way to do this.
                                                        MQXA_DataFile = "MQXA_NOMINAL.dat"), primary = True)
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q3FwdG4FieldSvcCfg(ConfigFlags, name='Q3FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q3",
                                                        Magnet = 2, # FIXME find a better way to do this.
                                                        MQXA_DataFile = "MQXA_NOMINAL.dat"), primary = True)
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def D1FwdG4FieldSvcCfg(ConfigFlags, name='D1FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("D1",
                                                        Magnet = 3), primary = True)# FIXME find a better way to do this.

    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def D2FwdG4FieldSvcCfg(ConfigFlags, name='D2FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("D2",
                                                        Magnet = 4), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q4FwdG4FieldSvcCfg(ConfigFlags, name='Q4FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q4",
                                                        Magnet = 5), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q5FwdG4FieldSvcCfg(ConfigFlags, name='Q5FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q5",
                                                        Magnet = 6), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q6FwdG4FieldSvcCfg(ConfigFlags, name='Q6FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q6",
                                                        Magnet = 7), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q7FwdG4FieldSvcCfg(ConfigFlags, name='Q7FwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q7",
                                                        Magnet = 8), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q1HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q1HKick",
                                                        Magnet = 9), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q1VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q1VKickFwdG4FieldSvc', **kwargs): #note is lower case "v" in ForwardRegionMgFieldConfig.py
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q1VKick",
                                                        Magnet = 10), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q2HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q2HKick",
                                                        Magnet = 11), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q2VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q2VKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q2VKick",
                                                        Magnet = 12), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q3HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q3HKick",
                                                        Magnet = 13), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q3VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q3VKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q3VKick",
                                                        Magnet = 14), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q4VKickAFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickAFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q4VKickA",
                                                        Magnet = 15), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q4HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q4HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q4HKick",
                                                        Magnet = 16), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q4VKickBFwdG4FieldSvcCfg(ConfigFlags, name='Q4VKickBFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q4VKickB",
                                                        Magnet = 17), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q5HKickFwdG4FieldSvcCfg(ConfigFlags, name='Q5HKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q5HKick",
                                                        Magnet = 18), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result


def Q6VKickFwdG4FieldSvcCfg(ConfigFlags, name='Q6VKickFwdG4FieldSvc', **kwargs):
    result = ComponentAccumulator()
    fieldAcc = ComponentAccumulator()
    fieldAcc.addService(MagField__ForwardRegionFieldSvc("Q6VKick",
                                                        Magnet = 19), primary = True)# FIXME find a better way to do this.
    kwargs.setdefault("MagneticFieldSvc", result.getPrimaryAndMerge(fieldAcc).name)
    fieldSvc = result.getPrimaryAndMerge(ForwardFieldSvcCfg(ConfigFlags, name, **kwargs))
    result.addService(fieldSvc, primary = True)
    return result
