# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def dumpConfigurables(obj) :
    print '==== %s ====' % obj.name()
    import collections
    for cfg in obj._properties :
        try:
           if ( len(cfg)<2 or cfg[0:2] != '__' ) :
               val = getattr(obj,cfg)
               # if not isinstance(val, collections.Callable):
               print ' %s / %s = %s | %s' % (cfg,type(val), val, obj._properties[cfg])
        except :
            print ' %s not an attribute' % (cfg)

def extractCollectionPrefix(track_collection_name) :
    return track_collection_name[:-6] if track_collection_name[-6:] == 'Tracks'  else track_collection_name

def setDefaults( kwargs, **extra_kwargs) :
    extra_kwargs.update(kwargs)
    return extra_kwargs

def toolFactory( tool_class ):
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,tool_class.__name__) :
        tool = getattr(ToolSvc,tool_class.__name__)
    else :
        # print 'DEBUG toolFactory create %s' % (tool_class.__name__)
        tool = tool_class()
        ToolSvc += tool 
    return tool

def serviceFactory( svc_class ):
    from AthenaCommon.AppMgr import ServiceMgr
    if hasattr(ServiceMgr,svc_class.__name__) :
        svc = getattr(ServiceMgr,svc_class.__name__)
    else :
        # print 'DEBUG svcFactory create %s' % (svc_class.__name__)
        svc = svc_class()
        ServiceMgr += svc 
    return svc
