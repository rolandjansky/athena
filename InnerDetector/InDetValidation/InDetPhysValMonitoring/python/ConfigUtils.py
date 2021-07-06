# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from InDetRecExample.TrackingCommon import setDefaults


def dumpConfigurables(obj):
    print('==== %s ====' % obj.name())
    for cfg in obj._properties:
        try:
            if (len(cfg) < 2 or cfg[0:2] != '__'):
                val = getattr(obj, cfg)
                # if not isinstance(val, collections.Callable):
                print(' %s / %s = %s | %s' %
                      (cfg, type(val), val, obj._properties[cfg]))
        except Exception:
            print(' %s not an attribute' % (cfg))


def extractCollectionPrefix(track_collection_name):
    return (
        track_collection_name[:-6] if track_collection_name[-6:] == 'Tracks'
        else track_collection_name)


def toolFactory(tool_class):
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, tool_class.__name__):
        tool = getattr(ToolSvc, tool_class.__name__)
    else:
        # print ('DEBUG toolFactory create %s' % (tool_class.__name__))
        tool = tool_class()
        ToolSvc += tool
    return tool


def serviceFactory(svc_class):
    from AthenaCommon.AppMgr import ServiceMgr
    if hasattr(ServiceMgr, svc_class.__name__):
        svc = getattr(ServiceMgr, svc_class.__name__)
    else:
        # print ('DEBUG svcFactory create %s' % (svc_class.__name__))
        svc = svc_class()
        ServiceMgr += svc
    return svc


def create(cfg_class, kwargs_in, **kwargs_default):
    kwargs = setDefaults(kwargs_in, **kwargs_default)
    name = kwargs.pop('name', cfg_class.__class__.__name__)
    return cfg_class(name=name, **kwargs)


def createPublicTool(cfg_class, kwargs_in, **kwargs_default):
    kwargs = setDefaults(kwargs_in, **kwargs_default)
    name = kwargs.pop('name', cfg_class.__class__.__name__)
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc, name):
        return getattr(ToolSvc, name)
    a_tool = cfg_class(name=name, **kwargs)
    ToolSvc += a_tool
    return a_tool


def createExtendNameIfNotDefault(cfg_class,
                                 check_prop,
                                 def_val,
                                 kwargs_in,
                                 **kwargs_default):
    kwargs = setDefaults(kwargs_in, **kwargs_default)
    def_name = cfg_class.getType() if hasattr(
        cfg_class, 'getType') else cfg_class.__class__.__name__
    the_name = kwargs.pop('name', def_name)
    if check_prop in kwargs and kwargs[check_prop] != def_val:
        if the_name == def_name:
            the_name += '_'
            the_name += kwargs[check_prop]
    elif check_prop not in kwargs:
        kwargs.setdefault(check_prop, def_val)
    return cfg_class(name=the_name, **kwargs)
