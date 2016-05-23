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
        _kwargs[kw] = extra_kwargs[kw]
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
