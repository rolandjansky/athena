# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @file: PyAthenaComps.py
# @purpose: a set of Python classes for PyAthena
# @author: Sebastien Binet <binet@cern.ch>

from __future__ import print_function

__doc__     = """Module containing a set of Python base classes for PyAthena"""
__author__  = "Sebastien Binet <binet@cern.ch>"

### data
__all__ = [ 'StatusCode',
            'Alg',
            'Svc',
            'AlgTool',
            'Aud',
            'AthFilterAlgorithm',
            'algs',
            'services',
            ]

### imports
from AthenaCommon.Logging import logging
from AthenaCommon.Configurable  import *  # noqa: F401, F403
from AthenaPython.Configurables import (CfgPyAlgorithm,
                                        CfgPyService,
                                        CfgPyAlgTool,
                                        CfgPyAud)

### helpers -------------------------------------------------------------------
import weakref
class WeakRefHolder(object):
    """A convenient class to access py-components by name
    """
    pass
def __wk_setattr__(self, k, v):
    self.__setattr__(k, weakref.ref(v))

services = WeakRefHolder()
services.__doc__ = """The list of PyAthena::Svc which have been instantiated"""
services.__setattr__ = __wk_setattr__

algs = WeakRefHolder()
algs.__doc__ = """The list of PyAthena::Alg which have been instantiated"""
algs.__setattr__ = __wk_setattr__

del __wk_setattr__

### helper methods ------------------------------------------------------------
def _get_prop_value(pycomp, propname):
    if propname in pycomp.properties() and  pycomp.properties()[propname] != pycomp.propertyNoValue:
        return pycomp.properties()[propname]
    else:
        return pycomp.getDefaultProperty(propname)


### PyAthena.StatusCode -------------------------------------------------------
class StatusCode:
    Recoverable = 2
    Success     = 1
    Failure     = 0

### PyAthena.Alg --------------------------------------------------------------
class Alg( CfgPyAlgorithm ):
    """
    Base class from which all concrete algorithm classes should
    be derived.

    In order for a concrete algorithm class to do anything
    useful the methods initialize(), execute() and finalize()
    should be overridden.
    """
    def __init__(self, name = None, **kw):
        if name is None: name = kw.get('name', self.__class__.__name__)
        ## init base class
        super(Alg, self).__init__(name, **kw)
        self._pyath_evtstore = None # handle to the evt store
        self._pyath_detstore = None # handle to the det store
        self._pyath_condstore = None # handle to the cond store
        self._ctx = None
        self.__component_type__="Algorithm"
        self.__cpp_type__=self.__class__.__name__
        self.name=name
        return

    @property
    def evtStore(self):
        if not self._pyath_evtstore:
            import AthenaPython.PyAthena as PyAthena
            self._pyath_evtstore = PyAthena.py_svc('StoreGateSvc/StoreGateSvc')
        return self._pyath_evtstore
    
    @property
    def detStore(self):
        if not self._pyath_detstore:
            import AthenaPython.PyAthena as PyAthena
            self._pyath_detstore = PyAthena.py_svc('StoreGateSvc/DetectorStore')
        return self._pyath_detstore
    
    @property
    def condStore(self):
        if not self._pyath_condstore:
            import AthenaPython.PyAthena as PyAthena
            self._pyath_condtstore = PyAthena.py_svc('StoreGateSvc/ConditionStore')
        return self._pyath_condstore
    
    def sysInitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.initialize()
    
    def initialize(self):
        return StatusCode.Success

    def sysReinitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.reinitialize()
    
    def reinitialize(self):
        return StatusCode.Success

    def sysExecute(self, cppcontext):
        import cppyy
        self._ctx = cppyy.bind_object(cppcontext, "EventContext")
        try:
            ret = self.execute()
        finally:
            self._ctx = None
        return ret
    
    def execute(self):
        return StatusCode.Success

    def sysFinalize(self):
        return self.finalize()
    
    def finalize(self):
        return StatusCode.Success

    def sysStart(self):
        return self.start()
    
    def start(self):
        return StatusCode.Success

    def sysStop(self):
        return self.stop()
    
    def stop(self):
        return StatusCode.Success

    def filterPassed(self):
        """Did this algorithm pass or fail its filter criterion for the last event?"""
        return self._cppHandle.filterPassed()

    def setFilterPassed(self, state):
        """Set the filter passed flag to the specified state"""
        return self._cppHandle.setFilterPassed(state)
    
    def resetExecuted(self):
       self.setExecuted(False)
       return StatusCode.Success
    
    def setExecuted(self,state):
       return self._cppHandle.setExecuted(state)
    
    def isExecuted(self):
       return self._cppHandle.isExecuted()

    def getContext(self):
       return self._ctx
    
    pass # PyAthena.Alg

### PyAthena.Svc --------------------------------------------------------------
class Svc( CfgPyService ):
    """Base class for all services
    """
    def __init__(self, name = None, **kw):

        if name is None: name = kw.get('name', self.__class__.__name__)
        ## init base class
        super(Svc, self).__init__(name, **kw)
        return

    def sysInitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.initialize()
    
    def initialize(self):
        return StatusCode.Success

    def sysReinitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.reinitialize()
    
    def reinitialize(self):
        return StatusCode.Success

    def sysFinalize(self):
        return self.finalize()
    
    def finalize(self):
        return StatusCode.Success

    def sysStart(self):
        return self.start()
    
    def start(self):
        return StatusCode.Success

    def sysStop(self):
        return self.stop()
    
    def stop(self):
        return StatusCode.Success
    
    pass # PyAthena.Svc

### PyAthena.AlgTool ----------------------------------------------------------
class AlgTool( CfgPyAlgTool ):
    """
    Base class from which all concrete algtool classes should be derived.
    """
    
    def __init__(self, name=None, parent=None, **kw):
        if name is None: name = kw.get('name', self.__class__.__name__)
        if not (parent is None):
            if isinstance(parent, str): name = "%s.%s" % (parent,name)
            else:                       name = "%s.%s" % (parent.name(),name)
        ## init base class
        super(AlgTool, self).__init__(name, **kw)
        self.__dict__['msg']  = logging.getLogger( self.getJobOptName() )
        self._pyath_evtstore = None # handle to the evt store
        self._pyath_detstore = None # handle to the det store
        self._pyath_condstore = None # handle to the cond store
        self.__component_type__ = "AlgTool"
        self.__cpp_type__=self.__class__.__name__
        return

    @property
    def evtStore(self):
        if not self._pyath_evtstore:
            import AthenaPython.PyAthena as PyAthena
            self._pyath_evtstore = PyAthena.py_svc('StoreGateSvc/StoreGateSvc')
        return self._pyath_evtstore
    
    @property
    def detStore(self):
        if not self._pyath_detstore:
            import AthenaPython.PyAthena as PyAthena
            self._pyath_detstore = PyAthena.py_svc('StoreGateSvc/DetectorStore')
        return self._pyath_detstore
    
    @property
    def condStore(self):
        if not self._pyath_condstore:
            import AthenaPython.PyAthena as PyAthena
            self._pyath_condstore = PyAthena.py_svc('StoreGateSvc/ConditionStore')
        return self._pyath_condstore
    
    def sysInitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.initialize()
    
    def initialize(self):
        raise NotImplementedError(
            "You have to implement PyAthena.AlgTool.initialize() !"
            )

    def sysReinitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.reinitialize()
    
    def reinitialize(self):
        self.msg.info( "==> re-initialize..." )
        return StatusCode.Success

    def sysFinalize(self):
        return self.finalize()
    
    def finalize(self):
        raise NotImplementedError(
            "You have to implement PyAthena.AlgTool.finalize() !"
            )

    pass # PyAthena.AlgTool

### PyAthena.Aud --------------------------------------------------------------
class Aud( CfgPyAud ):
    """
    Base class from which all concrete auditor classes should be derived.
    """
    def __init__(self, name=None, **kw):

        if name is None: name = kw.get('name', self.__class__.__name__)
        ## init base class
        super(Aud, self).__init__(name, **kw)
        from AthenaCommon.Logging import logging
        self.__dict__['msg'] = logging.getLogger( self.getJobOptName() )

        return

    def sysInitialize(self):
        self.msg.setLevel(_get_prop_value(self,'OutputLevel'))
        return self.initialize()
    
    def initialize(self):
        raise NotImplementedError(
            "You have to implement PyAthena.Aud.initialize() !"
            )

    def sysFinalize(self):
        return self.finalize()
    
    def finalize(self):
        raise NotImplementedError(
            "You have to implement PyAthena.Aud.finalize() !"
            )

    ## default no-op implementation...

    def before(self, evt_name, comp_name):
        return

    def after (self, evt_name, comp_name):
        return
    
    pass # PyAthena.Aud

### PyAthena.AthFilterAlgorithm -----------------------------------------------
class AthFilterAlgorithm(Alg):
    """base class for a filter algorithm, making use of the cutflow-svc.

    instances of this class (and its derived class) shall have:
     - a `cutID` attribute of type `CutIdentifier*` from `initialize`
       (included) and onwards,
     - a `cutFlowSvc()` method from `initialize` (included) and onwards.

    The `cutID` attribute is the CutIdentifier returned by the `ICutFlowSvc`
    when `self` registered itself with the `ICutFlowSvc`.
    """
    _dflt_FilterDescription='N/A'
    
    def __init__(self, name = None, **kw):
        if name is None:
            name = kw.get('name', self.__class__.__name__)
        kw['name'] = name

        ## init base class
        super(AthFilterAlgorithm, self).__init__(**kw)

        # properties
        self._filter_descr = kw.get('FilterDescription',
                                    AthFilterAlgorithm._dflt_FilterDescription)
        '''describes to the cutflowsvc what this filter does.'''
        return

    def _get_filter_descr(self):
        return self._filter_descr
    def _set_filter_descr(self, descr):
        self._filter_descr = descr
        
    FilterDescription = property(_get_filter_descr,
                                 _set_filter_descr,
                                 doc='describes to the cutflowsvc what this filter does.')
    
    def cutFlowSvc(self):
        if not hasattr(self, '_cutflowsvc'):
            import AthenaPython.PyAthena as PyAthena
            self._cutflowsvc = PyAthena.py_svc('CutFlowSvc',
                                               iface='ICutFlowSvc')
        return self._cutflowsvc

    def setFilterDescription(self,descr):
        """This function updates self's CutFlow description if (and only if)
        it has not been explicitely set during the python-level configuration
        """
        if hasattr(self,'cutID'):
            if self.FilterDescription==AthFilterAlgorithm._dflt_FilterDescription:
                self.cutFlowSvc().setFilterDescription(self.cutID,descr)
        else:
            self.msg.error("AthFilterAlg has no self.cutID: could not set filter description.")
        return

    def sysInitialize(self):
        myName=self.name() if callable(self.name) else self.name 
        self.cutID = self.cutFlowSvc().registerFilter(myName, self._filter_descr)
        if not self.cutID:
            self.msg.error("could not register filter-cut with cutflowsvc")
            return StatusCode.Failure
        
        return super(AthFilterAlgorithm, self).sysInitialize()

    def setFilterPassed(self, state):
        """Set the filter passed flag to the specified state"""
        o = super(AthFilterAlgorithm, self).setFilterPassed(state)
        if state:
            # TODO: we should read a proper weight
            self.cutFlowSvc().addEvent(self.cutID, 1)
        return o
    
    pass # PyAthena.AthFilterAlgorithm
