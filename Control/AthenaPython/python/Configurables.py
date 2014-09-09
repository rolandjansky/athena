# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: Configurables.py
# @purpose: a set of Configurables for the PyAthena components
# @author: Sebastien Binet <binet@cern.ch>

import AthenaCommon.Constants as Lvl
from AthenaCommon.Configurable import *
from AthenaCommon import CfgMgr

### 
class PyComponents(object):
    """@c PyComponents is a placeholder where all factories for the python
    components will be collected and stored for easy look-up and call from
    the C++ side.
    The @a PyComponents.instances dictionary will store the instance
    e.g.:
     PyComponents.instances[ 'alg1' ] = <PyAthena::Alg/alg1 instance>
    All this boilerplate code will of course be done automatically for the user
    as soon as she uses the (python) @PyConfigurable classes.
    """
    instances = {}
    pass

### helper methods ------------------------------------------------------------
def _get_prop_value(pycomp, propname):
    v = pycomp.properties()[propname]
    if v == pycomp.propertyNoValue:
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if propname == 'OutputLevel':
            # special case of OutputLevel...
            v = getattr(svcMgr.MessageSvc, propname)
        else:
            v = pycomp.getDefaultProperty(propname)
            pass
    return v

def declare_property (o, kw, property_name,
                      dflt_value=None, attr_name=None,
                      doc=None):
    """mimic the C++ declareProperty method.
       @param `kw` is a dictionary of properties where properties' values are
              looked for
       @param `property_name` is the property to look for
       @param `dflt_value` is the default value to set in case the property is
              not in the dictionary of properties
       @param `attr_name` is the name of the property inside the python class.
              if None, then `property_name` will be used

       example:
       >>> declare_property (self, kw, 'TopAlg', dflt_value=list(),
       ...                   attr_name='_top_alg')
       >>> assert self._top_alg == list()
       >>> self.TopAlg = [1]
       >>> assert self._top_alg == [1]
       >>> assert self.TopAlg == [1]
    """
    if doc is None:
        doc=''
    val = kw.get (property_name, dflt_value)
    n = attr_name if attr_name is not None else property_name
    setattr (o, n, val)
    if attr_name is not None:
        setattr (o, attr_name, val)
        # install a property dispatch too
        def fget (self):   return getattr(self, attr_name)
        def fset (self,v): return setattr(self, attr_name, v)
        _p_obj = property(fget=fget,
                          fset=fset,
                          doc=doc)
        setattr(o.__class__, property_name, _p_obj)
    return

### Configurable base class for PyAlgorithms ----------------------------------
class CfgPyAlgorithm( ConfigurableAlgorithm ):
    def __init__( self, name, **kw ):
        super( CfgPyAlgorithm, self ).__init__(name)
        for n,v in kw.iteritems():
            setattr(self,n,v)
    # pickling support
    def __getstate__( self ):
        dic = super(CfgPyAlgorithm, self).__getstate__()
        dic.update(self.__dict__)
        if 'msg' in dic:
            del dic['msg'] # logger cannot be pickled
        return dic

    @property
    def msg(self):
        import AthenaCommon.Logging as _L
        return _L.logging.getLogger( self.getJobOptName() )
    def getGaudiType( self ): return 'Algorithm'
    def getType(self):        return 'PyAthena::Alg'
    def getDlls(self):        return 'AthenaPython'
    def getHandle(self):      return None
    def setup(self):
        from AthenaCommon import CfgMgr
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr( svcMgr, 'PyComponentMgr' ):
            svcMgr += CfgMgr.PyAthena__PyComponentMgr('PyComponentMgr')

        import PyAthena
        
        ## populate the PyComponents instances repository
        name = self.getJobOptName()
        o = PyComponents.instances.get(name, None)
        if not (o is None) and not (o is self):
            err = "A python component [%r] has already been "\
                  "registered with the PyComponents registry !" % o
            raise RuntimeError(err)
        PyComponents.instances[name] = self
        setattr(PyAthena.algs, name, self)
        ## special case of the OutputLevel: take the value from the
        ## svcMgr.MessageSvc if none already set by user
        setattr(self, 'OutputLevel', _get_prop_value (self, 'OutputLevel') )
        return super(CfgPyAlgorithm, self).setup()
    pass # class CfgPyAlgorithm

### Configurable base class for PyServices ------------------------------------
class CfgPyService( ConfigurableService ):
    def __init__( self, name, **kw ):
        super( CfgPyService, self ).__init__(name)
        for n,v in kw.iteritems():
            setattr(self,n,v)
    # pickling support
    def __getstate__( self ):
        dic = super(CfgPyService, self).__getstate__()
        dic.update(self.__dict__)
        if 'msg' in dic:
            del dic['msg'] # logger cannot be pickled
        return dic

    @property
    def msg(self):
        import AthenaCommon.Logging as _L
        return _L.logging.getLogger( self.getJobOptName() )
    def getGaudiType( self ): return 'Service'
    def getType(self):        return 'PyAthena::Svc'
    def getDlls(self):        return 'AthenaPython'
    def getHandle(self):      return None
    def setup(self):
        from AthenaCommon import CfgMgr
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr( svcMgr, 'PyComponentMgr' ):
            svcMgr += CfgMgr.PyAthena__PyComponentMgr('PyComponentMgr')
        
        import PyAthena

        ## populate the PyComponents instances repository
        name = self.getJobOptName()
        o = PyComponents.instances.get(name, None)
        if not (o is None) and not (o is self):
            err = "A python component [%r] has already been "\
                  "registered with the PyComponents registry !" % o
            raise RuntimeError(err)
        PyComponents.instances[name] = self
        setattr(PyAthena.services,name,self)
        ## special case of the OutputLevel: take the value from the
        ## svcMgr.MessageSvc if none already set by user
        setattr(self, 'OutputLevel', _get_prop_value (self, 'OutputLevel') )
        return super(CfgPyService, self).setup()
    pass # class CfgPyService

### Configurable base class for PyAlgTools ------------------------------------
class CfgPyAlgTool( ConfigurableAlgTool ):
    def __init__( self, name, **kw ):
        super( CfgPyAlgTool, self ).__init__(name)
        for n,v in kw.iteritems():
            setattr(self,n,v)
    # pickling support
    def __getstate__( self ):
        dic = super(CfgPyAlgTool, self).__getstate__()
        dic.update(self.__dict__)
        if 'msg' in dic:
            del dic['msg'] # logger cannot be pickled
        return dic

    @property
    def msg(self):
        import AthenaCommon.Logging as _L
        return _L.logging.getLogger( self.getJobOptName() )
    def getGaudiType( self ): return 'AlgTool'
    def getType(self):        return 'PyAthena::Tool'
    def getDlls(self):        return 'AthenaPython'
    def getHandle(self):      return None
    def setup(self):
        from AthenaCommon import CfgMgr
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr( svcMgr, 'PyComponentMgr' ):
            svcMgr += CfgMgr.PyAthena__PyComponentMgr('PyComponentMgr')
        
        ## populate the PyComponents instances repository
        name = self.getJobOptName()
        o = PyComponents.instances.get(name, None)
        if not (o is None) and not (o is self):
            err = "A python component [%r] has already been "\
                  "registered with the PyComponents registry !" % o
            raise RuntimeError(err)
        PyComponents.instances[name] = self
        ## special case of the OutputLevel: take the value from the
        ## svcMgr.MessageSvc if none already set by user
        setattr(self, 'OutputLevel', _get_prop_value (self, 'OutputLevel') )
        return super(CfgPyAlgTool, self).setup()
    pass # class CfgPyAlgTool

### Configurable base class for PyAud -----------------------------------------
class CfgPyAud( ConfigurableAuditor ):
    def __init__( self, name, **kw ):
        super( CfgPyAud, self ).__init__(name)
        for n,v in kw.iteritems():
            setattr(self,n,v)
    # pickling support
    def __getstate__( self ):
        dic = super(CfgPyAud, self).__getstate__()
        dic.update(self.__dict__)
        if 'msg' in dic:
            del dic['msg'] # logger cannot be pickled
        return dic

    @property
    def msg(self):
        import AthenaCommon.Logging as _L
        return _L.logging.getLogger( self.getJobOptName() )
    def getGaudiType( self ): return 'Auditor'
    def getType(self):        return 'PyAthena::Aud'
    def getDlls(self):        return 'AthenaPython'
    def getHandle(self):      return None
    def setup(self):
        from AthenaCommon import CfgMgr
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr( svcMgr, 'PyComponentMgr' ):
            svcMgr += CfgMgr.PyAthena__PyComponentMgr('PyComponentMgr')
        
        ## populate the PyComponents instances repository
        name = self.getJobOptName()
        o = PyComponents.instances.get(name, None)
        if not (o is None) and not (o is self):
            err = "A python component [%r] has already been "\
                  "registered with the PyComponents registry !" % o
            raise RuntimeError(err)
        PyComponents.instances[name] = self
        ## special case of the OutputLevel: take the value from the
        ## svcMgr.MessageSvc if none already set by user
        setattr(self, 'OutputLevel', _get_prop_value (self, 'OutputLevel') )
        return super(CfgPyAud, self).setup()
    pass # class CfgPyAud


## add the declare_property method...
for klass in (CfgPyAlgorithm, CfgPyService, CfgPyAlgTool, CfgPyAud):
    setattr(klass, 'declare_property', declare_property)
del declare_property
##


### -----
class _PyCompHandle(object):
    """a class to mimic the gaudi C++ {Tool,Svc}Handle classes: automatic call
    to `initialize` when __getattr__ is called on the instance.
    """
    def __init__(self, parent, attr_name):
        msg = parent.msg.verbose
        msg('installing py-comp-handle for [%s.%s]...',
            parent.name(), attr_name)
        self.__dict__.update({
            '_parent': parent,
            '_name': attr_name,
            '_attr': getattr(parent, attr_name),
            '_init_called': False,
            })
        msg('installing py-comp-handle for [%s.%s]... [done]',
            parent.name(), attr_name)
        return

    def __getattribute__(self, n):
        if n.startswith('_'):
            return super(_PyCompHandle, self).__getattribute__(n)
        obj = self.__dict__['_attr']
        if not self.__dict__['_init_called']:
            parent = self.__dict__['_parent']
            # FIXME: should we raise something in case initialize failed ?
            obj.initialize()
            self.__dict__['_init_called'] = True
            # replace the handle with the proxied object
            setattr(parent, self.__dict__['_name'], obj)
        return getattr(obj, n)

    def __setattr__(self, n, v):
        if n.startswith('_'):
            return super(_PyCompHandle, self).__setattr__(n,v)
        obj = self.__dict__['_attr']
        if not self.__dict__['_init_called']:
            parent = self.__dict__['_parent']
            # FIXME: should we raise something in case initialize failed ?
            obj.initialize()
            self.__dict__['_init_called'] = True
            # replace the handle with the proxied object
            setattr(parent, self.__dict__['_name'], obj)
        return setattr(obj, n, v)
        
    pass # _PyCompHandle

def _is_pycomp(obj):
    return isinstance(
        obj,
        (CfgPyAlgorithm, CfgPyService, CfgPyAlgTool, CfgPyAud)
        )

def _install_fancy_attrs():
    """loop over all pycomponents, inspect their attributes and install
    a handle in place of (sub) pycomponents to trigger the auto-initialize
    behaviour of (C++) XyzHandles.
    """
    import PyUtils.Logging as L
    msg = L.logging.getLogger('PyComponentMgr').verbose
    comps  = PyComponents.instances.items()
    ncomps = len(comps)
    msg('installing fancy getattrs... (%i)', ncomps)
    for k,comp in comps:
        msg('handling [%s]...', k)
        for attr_name, attr in comp.__dict__.iteritems():
            if _is_pycomp(attr):
                msg(' ==> [%s]...', attr_name)
                setattr(comp, attr_name,
                        _PyCompHandle(parent=comp, attr_name=attr_name))
                msg(' ==> [%s]... [done]', attr_name)
    msg('installing fancy getattrs... (%i) [done]',ncomps)
    return
