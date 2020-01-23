# from CfgPythonComp import log
from CfgPythonComp import str_type

# --- debug utilities
def logType(alogger,prefix,tool) :
    '''
    Dump type information about elements of a property to the given
    logger

    '''
    if tool is not None :
        if not isinstance(tool,str_type) and isinstance(tool,(tuple,list)) :
            # @TODO either mive dumpType here or logType to CfgDebug
            from CfgDebug import dumpType
            return dumpType(prefix+' tool[0]',tool[0])
        if hasattr(tool,'__class__') :
            from AthenaCommon.Configurable import Configurable
            if hasattr(tool,'getName') :
                # satisfy flake
                msg='%s type: %s %s cl=%s n=%s ' % (prefix,type(tool),
                                                    ' is Configurable' if isinstance(tool,Configurable) else '',
                                                    tool.__class__.__name__,tool.getName())
                alogger.debug(msg)
            else :
                # satisfy flake
                msg='%s type: %s %s cl=%s ' % (prefix,type(tool),
                                               ' is Configurable' if isinstance(tool,Configurable) else '',
                                               tool.__class__.__name__)
                alogger.debug(msg)
        else :
            msg='%s %s' % (prefix,tool)
            alogger.debug(msg)
    return tool

def logProps(alogger,cfg) :
    '''
    Dump property information of the given configurable to the given
    logger

    '''
    from AthenaCommon.Configurable import Configurable
    if isinstance( cfg, Configurable ) :
       # same for any of its private ToolHandle properties
        for k, v in cfg.getProperties().items():
            if isinstance( v, Configurable ):
                logType(alogger,'Properties of %s:' % cfg.getName(), v)
                logProps(alogger,v)
            else :
                msg='property %s.%s = %s' % (cfg.getName(),k,v)
                alogger.debug(msg)
    elif isinstance(cfg, (list,tuple)) :
        for elm in cfg :
            names=[]
            if isinstance( cfg, Configurable ) :
                names.append(cfg.name())
            else :
                names.append(type(cfg))
        msg='properties of %s:   %s' % (cfg.getName(),names)
        alogger.debug(msg)


def getProperty(configurable, key,modifier=lambda x : x) :
    '''
    Return the value or default value of the given property of the
    given configurable.

    '''
    from AthenaCommon.Configurable import Configurable
    val = configurable.getProperties()[key]
    if val == Configurable.propertyNoValue :
        val = configurable.getDefaultProperties()[key]
    return modifier(val)

def argsToStr(args) :
    '''
    Turn the given argument list into a string.  Exceptions while
    converting individual arguments into strings are caught and
    ignored.

    '''
    msg='['
    for arg in args :
        try :
            from AthenaCommon.Configurable import Configurable
            if arg is not  None and hasattr(arg,'__class__') and hasattr(arg,'getName') :
                msg += '%s %s cl=%s n=%s ' % (type(arg),' is Configurable' if isinstance(arg,Configurable) else '', arg.__class__.__name__,arg.getName())
            else :
                msg+="%s," % arg
        except Exception :
            msg+="?,"
    if msg[-1]==',' :
        msg=msg[:-1]
    msg+=']'
    return msg

def dictToStr(args) :
    '''
    Turn the given keyword argument list into a string.  Exceptions
    while converting individual arguments into strings are caught and
    ignored.

    '''
    if args is None :
        msg ='<None>'
    else :
        msg='['
        for arg,val in args.items() :
            try :
                if not isinstance(val,str_type) and isinstance(val,(tuple,list)) :
                    msg += '%s : %s ' % (arg,argsToStr(val))
                else :
                    from AthenaCommon.Configurable import Configurable
                    if val is not None and hasattr(val,'__class__') and hasattr(val,'getName') :
                        msg += '%s: %s %s cl=%s n=%s ' % (arg, type(val),' is Configurable' if isinstance(val,Configurable) else '', val.__class__.__name__,val.getName())
                    else :
                        msg+="%s : %s," % (arg,args[arg])
            except Exception :
                msg+="?,"
        if msg[-1]==',' :
            msg=msg[:-1]
        msg+=']'
    return msg

# -- convenience methods 

def childsInSet(cfg, aset) :
    '''
    Return True if the name of the configurable or the names of all
    its child configurables are in the given set.

    '''
    from AthenaCommon.Configurable import Configurable
    if cfg.getName() in aset :
        return True
    # same for any of its private ToolHandle properties
    # @TODO what about tool handle arrays ?
    for k, v in cfg.getProperties().items():
        if isinstance( v, Configurable ):
            # log( 'DEBUG check if %s should be locked' % (v.getName())) 
            if childsInSet(v,aset) :
                return True
    return False

# -- argument handling
def setDefaults( kwargs, **extra_kwargs) :
    '''
    Add the extra_kwargs key value pairs to the dictionary kwargs
    unless they are already present in kwargs.

    '''
    extra_kwargs.update(kwargs)
    return extra_kwargs

def stripArgs( kwargs, arg_list) :
    '''
    Remove the arguments whose keys are listed in arg_list from kwargs
    and return them if they existed in kwargs.

    '''
    if isinstance(arg_list,str_type) :
        arg_list = [arg_list]
    subset_args=dict()
    for arg in arg_list :
        if arg  in kwargs :
            subset_args[arg]=kwargs.pop(arg)
    return subset_args

def stripOrSetArgs( kwargs, **arg_list) :
    '''
    Remove the arguments whose keys are listed in arg_list from kwargs
    and return them or their default values from arg_list

    '''
    subset_args=dict()
    for arg in arg_list :
        if arg  in kwargs :
            subset_args[arg]=kwargs.pop(arg)
        else :
            subset_args[arg]=arg_list[arg]
    return subset_args

def copyArgs(kwargs_src, kwargs_dest, arg_list) :
    '''
    Copy the the arguments whose keys are listed in arg_list from
    kwargs_src to kwargs_dest and return them if they existed in
    kwargs.
    '''
    if isinstance(arg_list,str_type) :
        arg_list = [arg_list]
    for arg in arg_list :
        if arg in kwargs_src :
            # log( 'DEBUG copyArgs %s=%s' %(arg,kwargs_src[arg]) )
            kwargs_dest[arg]=kwargs_src[arg]
    return kwargs_dest

def checkAmbigiousSettings(kwargs, alias_dict ) :
    '''
    Raise an exception if kwargs contains properties which may contradict each other.
    '''
    for an_arg  in alias_dict :
        if an_arg in kwargs and alias_dict[an_arg] in kwargs :
            raise Exception("Both %s and %s specified" %(an_arg,alias_dict[an_arg]))


def importFrom(module_name, object_name, object_final_name=None) :
    '''
    Import from the module given by name the object given by name and
    optionally rename it to object_final_name if the latter is not
    None

    '''
    import importlib
    return getattr(importlib.import_module(module_name), object_name)

def selectFirst(options)  :
    '''
    Select the first option of the given list of condition,option
    pairs for which the condition is true.  The condition is expected
    to be a boolean value rather than a callable

    '''
    for cond, option in options :
        if cond :
            return option
    raise Exception( 'No matching option')

# --- extra argument handling

class SpecialArg(object) :
    '''
    Placeholder for default values for keyword arguments.
    '''
    Undefined      = 0
    Required       = 1
    Optional       = 2
    Evaluate       = 3
    FlagsEvaluate  = 4

    def __init__(self) :
        pass

class RequiredArg(SpecialArg) :
    '''
    An argument which has no meaningful default values and which has
    to be present in input keyword arguments

    '''
    def __init__(self) :
        self.type=SpecialArg.Required
        pass

class OptionalArg(SpecialArg) :
    '''
    An argument which is ignored i.e. if the in input kwargs do not
    contain this argument also the output kwargs will not contain it
    @param def optional argument which is ignored

    '''
    def __init__(self, default=None) :
        self.type=SpecialArg.Optional
        pass

class EvalArg(SpecialArg) :
    '''
    An argument which if missing from the input kwargs, is set in the
    output kwargs to the value of the given function.  @param func
    function to be evaluate to replace the keyword argument in case it
    is missing in the input

    '''
    def __init__(self, func) :
        self.type=SpecialArg.Evaluate
        self.value = func
        pass

class FlagsDependentArg(EvalArg) :
    '''
    An argument which if missing from the input kwargs, is set in the
    output kwargs to the value of the given function which gets the
    flags as argument.  @param func function which takes the flags as
    argument and which is evaluate to replace the keyword argument in
    case it is missing in the input

    '''
    def __init__(self,func) :
        super(FlagsDependentArg,self).__init__(func)
        self.type=SpecialArg.FlagsEvaluate


def extraArgs(**def_extra_kwargs) :
    '''
    Decorator to extend the input keyword arguments with default
    values.  The default values may by of type @ref SpecialArg, which
    are evaluated before they are passed to the decorated function.
    '''

    def decorator(inFunction) :
        def evaluateExtraArgs(factory=None, flags=None, *args,**kwargs):
            # @TODO add special value to prevent arguments to be set ?
            import copy
            _kwargs = copy.copy(kwargs)
            for k,v in def_extra_kwargs.items() :
                if k not in kwargs :
                    if isinstance(v, SpecialArg) :
                        if v.type == SpecialArg.Required :
                            if (k not in _kwargs) :
                                raise Exception('No value given for the required argument %s'  % k)
                        elif v.type == SpecialArg.FlagsEvaluate:
                            _kwargs[k]=v.value(flags)
                        elif v.type == SpecialArg.Optional:
                            continue
                        elif v.type == SpecialArg.Evaluate and k not in _kwargs:
                            _kwargs[k]=v.value()
                        else :
                            raise Exception('Unhandled special argument %s [typeId=%s]'  % (type(v), v.type))
                    else :
                        _kwargs.setdefault(k,v)

            # log ( 'DEBUG added axtra keyword args to %s : %s -> %s' % (inFunction.__name__, dictToStr(def_extra_kwargs), dictToStr(kwargs)) )
            return inFunction(factory, flags, *args,**_kwargs)

        # to allow pickling of decorated functions for hash computation:
        evaluateExtraArgs.__name__   = inFunction.__name__
        evaluateExtraArgs.__module__ = inFunction.__module__
        return evaluateExtraArgs
    return decorator


def mergeLists(dest,extra) :
    '''
    Add all elements contained in extra in order of appearence to the end of dest if they do not yet exist.
    Both dest and extra can be lists or single elements. The result will be always a list.
    '''
    import collections
    if isinstance(dest,str_type) or not isinstance(dest,list) :
        if not isinstance(dest,str_type) and isinstance( dest, collections.Iterable) :
            dest = list( dest)
        else :
            dest = [dest]
    if isinstance(extra, str_type) or not isinstance(extra,collections.Iterable) :
        extra=[extra]
    for elm in extra :
        if elm not in dest :
            dest.append(elm)
    return dest
