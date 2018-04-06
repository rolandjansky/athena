# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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

def injectNameArgument(inFunction):
    configurable_base_classes = ['ConfigurableAlgTool','ConfigurableAlgorithm']
    '''
    Auxiliary method to inject a name argument in the keyword argument dictionary
    '''

    def setConfigurables(_cls) :
        class_stack = [_cls]

        the_base_class = None
        while len(class_stack) > 0 : 
            the_class = class_stack.pop()
            for base in the_class.__bases__ :
               if base.__name__ in configurable_base_classes :
                   _cls.configurables = the_class.configurables
                   break
               class_stack.append(base)
        return

    def outFunction(*args,**kwargs):
        _cls = args[0]

        # set the name argument to be equal to the class name
        import copy
        kw_cp=copy.copy(kwargs)
        kw_cp['name']=_cls.__name__

        if len(_cls.configurables)==0 :
            setConfigurables(_cls)

        return inFunction(*args,**kw_cp)

    return outFunction


def checkKWArgs(inFunction):
    '''
    Auxiliary method to check that only a name kw arg is given.
    '''

    def outFunction(*args,**kwargs):
        if len(kwargs)>1 or kwargs.keys()[0] != 'name' :
            raise(' No keyword arguments shall be given')

        return inFunction(*args,**kwargs)

    return outFunction

def _args( kwargs, **extra_kwargs) :
    import copy
    _kwargs = copy.copy(kwargs)
    for kw in extra_kwargs :
        _kwargs.setdefault(kw,extra_kwargs[kw])
    return _kwargs;


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

def create(cfg_class, kwargs_in, **kwargs_default) :
    kwargs=_args(kwargs_in, **kwargs_default)
    name = kwargs.pop('name',cfg_class.__class__.__name__)
    return cfg_class(name = name,**kwargs)

def createPublicTool(cfg_class, kwargs_in, **kwargs_default) :
    kwargs=_args(kwargs_in, **kwargs_default)
    name = kwargs.pop('name',cfg_class.__class__.__name__)
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,name) :
        return  getattr(ToolSvc,name)
    a_tool = cfg_class(name = name,**kwargs)
    ToolSvc += a_tool
    return a_tool


def createExtendNameIfNotDefault(cfg_class, check_prop, def_val, kwargs_in, **kwargs_default) :
    kwargs=_args(kwargs_in, **kwargs_default)
    def_name = cfg_class.getType() if hasattr(cfg_class,'getType') else cfg_class.__class__.__name__
    the_name = kwargs.pop('name',def_name)
    if check_prop in kwargs and kwargs[check_prop] != def_val :
        if the_name == def_name :
           the_name += '_'
           the_name += kwargs[check_prop]
    elif check_prop not in kwargs :
        kwargs.setdefault(check_prop,def_val)
    return cfg_class(name = the_name,**kwargs)
