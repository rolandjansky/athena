# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: PyAthena.py
# @purpose: a set of Python classes for PyAthena
# @author: Sebastien Binet <binet@cern.ch>

__doc__     = """Module containing a set of Python base classes for PyAthena"""
__version__ = "$Revision: 1.16 $"
__author__  = "Sebastien Binet <binet@cern.ch>"

### data
__all__        = []
__pseudo_all__ = [ 'StatusCode',
                   'Alg',
                   'Svc',
                   'AlgTool',
                   'Aud',
                   'AthFilterAlgorithm',
                   'services',
                   'algs'
                   ]

### helpers
def _get_native_libname(libname):
    """ return the OS-native name from an OS-indenpendent one """
    import sys
    plat = sys.platform
    if plat.count('linux')>0:
        lib_prefix,lib_suffix = 'lib', '.so'
    elif plat == 'win32':
        lib_prefix,lib_suffix = '', '.dll'
    elif plat == 'darwin':
        lib_prefix,lib_suffix = 'lib','.dylib'
    else:
        raise RuntimeError ("sorry platform [%s] is not (yet?) supported"%plat)
    _sys_libname = libname
    if not _sys_libname.startswith (lib_prefix):
        _sys_libname = ''.join([lib_prefix,_sys_libname])
    if not _sys_libname.endswith (lib_suffix):
        _sys_libname = ''.join([_sys_libname, lib_suffix])
    return _sys_libname

def load_library (libname):
    """
    Helper method to load a library by its natural name, not the OS-native name.
    But if the OS-native name is given, it is safely handled too. Note that there's
    a problem on MacOSX, for which the OS-native name ends with .dylib, but for
    which ATLAS builds .so libraries. Override the OS-native name (which should probably
    be replaced by two attempts; one with the .dylib and the other with .so)
    usage:
     >>> load_library ('AthenaServices')
     >>> load_library ('AthenaServicesDict')
    """
    _sys_libname = _get_native_libname(libname)
    from sys import platform
    if platform == 'darwin':
        _sys_libname = _sys_libname.replace('.dylib', '.so')
    import ctypes
    return ctypes.cdll.LoadLibrary (_sys_libname)

def find_library(libname):
    """
    Helper function to find the (full)path to a library given its natural name.
     @return None on failure
     
    usage:
     >>> find_library('AthenaServices')
     '/afs/cern.ch/.../AtlasCore/[release]/InstallArea/.../libAthenaServices.so
    """
    import os, sys
    import ctypes.util as cu
    ## # ctypes.util.find_library does not return the path
    ## # to the library, just the basename of the so-name...
    ## lib = cu._findLib_ldconfig(libname) or cu._findLib_gcc(libname)
    ## if lib:
    ##     return os.path.abspath(lib)
    _sys_libname = _get_native_libname(libname)
    # FIXME: REALLY not portable...
    if os.name != 'posix':
        raise RuntimeError('sorry OS [%s] is not supported' % os.name)
    
    if 'LD_LIBRARY_PATH' in os.environ:
        for d in os.environ['LD_LIBRARY_PATH'].split(os.pathsep):
            lib = os.path.join(d, _sys_libname)
            if os.path.exists(lib):
                return lib
    return
    
def reload_module (modname):
    """
    Helper method to reload a python module by name.
    This is useful in the usual following case:
     >>> from Foo import MyAlg
     >>> assert (not 'Foo' in dir())
     >>> reload(Foo) # won't work
     >>> PyAthena.reload_module ('Foo') # will work
     >>> PyAthena.reload_module (Foo)   # will work too
    """
    import sys, types
    if isinstance (modname, types.ModuleType):
        modname = modname.__name__
    if sys.modules.has_key(modname):
        return reload (sys.modules[modname])
    raise ValueError('no module [%s] could be found'%modname)

def py_reload (*args):
    """simple minded function to reload objects, methods and modules

    example of usage:
    >>> # edit and modify the execute methods of some PyAthena.Alg
    ... # class, then load back that definition
    >>> PyAthena.py_reload (alg1.execute, alg2.execute)
    >>> PyAthena.py_reload (alg1.execute)
    >>> alg1.execute() # will use the new definition
    >>> theApp.nextEvent() # will also use the new definitions
    ... # of both alg1 and alg2
    """
    import types, sys
    for i,arg in enumerate(args):
        if isinstance (arg, types.ModuleType):
##             print " ==> moduletype"
            reload (arg)
        elif isinstance (arg, types.StringType):
            # no-op
            continue
        elif isinstance (arg, types.MethodType):
##             print " ==> methodtype"
            # reload module holding the class' definition
            modname = arg.im_self.__class__.__module__
            module = reload (sys.modules[modname])
            obj = arg.im_self
            klass = getattr (module, obj.__class__.__name__)
            # update the object with the new fct definition
            fct_name = arg.im_func.__name__
            new_fct = getattr (klass, fct_name)
            #new_fct.im_class = klass
##             print "-->fct:",new_fct.im_func
            setattr (obj, fct_name,
                     new_fct.__get__(obj))
        elif hasattr (arg, '__class__'):
##             print " ==> classtype"
            # reload module holding the class' definition
            modname = arg.__class__.__module__
            module = reload (sys.modules[modname])
            # get the class, in case new methods appeared...
            klass = getattr (module, arg.__class__.__name__)
            from AthenaCommon.Configurable import Configurable
            cfg_methods = dir(Configurable)
            d = (k for k in dir(klass)
                 if not k.startswith('__') and
                    not k in cfg_methods)
            for k in d:
                if not hasattr (arg, k):
                    v = getattr (klass, k)
                    try:
                        v = v.__get__ (arg)
                    except AttributeError, err:
                        # 'handle' not-yet-set gaudi properties
                        continue
                    setattr (arg, k, v.__get__(arg))
                # FIXME: should we remove methods which disappeared ?
                v = getattr (arg, k)
##                 print " ==> reloading [%s.%s <- %s]" % (
##                     arg.__class__.__name__,
##                     k, v)
                py_reload (v)
        else:
            print "*** unhandled type: [%s] (arg #%i) ***" % (type(arg),i)
        pass
    return

### imports
import PyAthenaComps
from Bindings import _PyAthenaBindingsCatalog as _pycat

### module facade to allow loading classes as attributes ----------------------
import types
class ModuleFacade(types.ModuleType):
    """a helper class to allow easy retrieval of automatically generated
    configurables (stolen from PyRoot)
    """
    def __init__( self, module ):
        types.ModuleType.__init__(self, module.__name__)
        self.__dict__[ 'module' ]   = module
        self.__dict__[ '__doc__'  ] = module.__doc__
        self.__dict__[ '__name__' ] = module.__name__
        self.__dict__[ '__file__' ] = module.__file__

        from .Bindings import py_svc
        self.__dict__ ['py_svc'] = py_svc

        from .Bindings import py_tool
        self.__dict__ ['py_tool'] = py_tool

        from .Bindings import py_alg
        self.__dict__ ['py_alg'] = py_alg

        self.__dict__ ['load_library']  = load_library
        self.__dict__ ['find_library']  = find_library
        self.__dict__ ['reload_module'] = reload_module
        self.__dict__ ['py_reload']     = py_reload

    def __getattr__(self, k):
        if k in self.__dict__:
            return self.__dict__.get(k)
        if k.startswith('__'):
            return types.ModuleType.__getattribute__(self, k)
        if k in __pseudo_all__: v = getattr(PyAthenaComps, k)
        else:                   v = _pycat.init(k)
        object.__getattribute__(self, '__dict__')[k] = v
        return v

import sys
sys.modules[ __name__ ] = ModuleFacade( sys.modules[ __name__ ] )
del ModuleFacade
