# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file: AthenaPython/python/tests/PyTestsLib.py
# @purpose: a set of py-components to test various aspects of PyAthena
# @author: Sebastien Binet <binet@cern.ch>

__doc__ = """Module containing a set of py-components to test various aspects
of PyAthena.
"""
__version__ = "0.0.1"
__author__  = "Sebastien Binet <binet@cern.ch>"

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

class MyAlg( PyAthena.Alg ):
    """Simple test of a py-algorithm
    """
    def __init__(self, name = "MyAlg", **kw):
        ## init base class
        kw['name'] = name
        super(MyAlg,self).__init__(**kw)

        self.px           = kw.get('px',  10.*Units.GeV)
        self.eta          = kw.get('eta', 2.5)
        self.pt           = kw.get('pt',  40.*Units.GeV)
        self.mytool       = kw.get('mytool', MyTool("%s_mytool"%self.name()))
        self.filterPassed = kw.get('filterPassed', True)
        
    def initialize(self):
        self.sg = PyAthena.py_svc("StoreGateSvc")
        self.msg.info( "==> initializing [%s]...", self.name() )
        self.msg.info( "eta: %r",self.eta )
        self.msg.info( "pt:  %r",self.pt  )
        self.msg.info( "px:  %r",self.px  )
        self.mytool.counter += 1
        self.msg.info( "tool:%r %r",self.mytool.counter, self.mytool.name() )
        return StatusCode.Success

    def execute(self):
        _info = self.msg.info
        _info( "==> execute..." )
##         _info( "content of StoreGate..." )
##         self.sg.dump()
        _info("hasattr('_cppHandle'): %s", hasattr(self,'_cppHandle'))
        self._cppHandle.setFilterPassed(self.filterPassed)
        _info("has passed filter: %s", bool(self._cppHandle.filterPassed()))
        return StatusCode.Success

    def finalize(self):
        self.msg.info( "==> finalize..." )
        return StatusCode.Success

class MySvc( PyAthena.Svc ):
    """Simple test of a py-service
    """
    def __init__(self, name = "MySvc", **kw):
        ## init base class
        kw['name'] = name
        super(MySvc,self).__init__(**kw)

        self.counter = kw.get('counter', 0)
        
    def initialize(self):
        self.sg = PyAthena.py_svc("StoreGateSvc")
        self.msg.info( "==> initializing [%s]...", self.name() )
        self.msg.info( "cnt: %r",self.counter )
        return StatusCode.Success

    def finalize(self):
        self.msg.info( "==> finalize..." )
        self.msg.info( "cnt: %r",self.counter )
        return StatusCode.Success

class MyTool( PyAthena.AlgTool ):
    """Simple test of a py-tool
    """
    def __init__(self, name = "MyTool", **kw):
        ## init base class
        kw['name'] = name
        super(MyTool,self).__init__(**kw)

        self.counter = kw.get('counter', 0)
        
    def initialize(self):
        self.sg = PyAthena.py_svc("StoreGateSvc")
        self.msg.info( "==> initializing [%s]...", self.name() )
        self.msg.info( "cnt: %r",self.counter )
        return StatusCode.Success

    def finalize(self):
        self.msg.info( "==> finalize..." )
        self.msg.info( "cnt: %r",self.counter )
        return StatusCode.Success

class MyNameAud( PyAthena.Aud ):
    """Simple test of a py-auditor
    """
    def __init__(self, name="MyNameAud", **kw):
        ## init base class
        kw['name'] = name
        super(MyNameAud,self).__init__(**kw)

    def initialize(self):
        self.msg.info("==> initializing [%s]...", self.name())
        return StatusCode.Success

    def finalize(self):
        self.msg.info("==> finalizing [%s]...", self.name())
        return StatusCode.Success

    def before(self, evt_name, comp_name):
        self.msg.info("Entering %s [%s]...", evt_name.lower(), comp_name)
        return

    def after (self, evt_name, comp_name, sc):
        self.msg.info("Exiting  %s [%s]...", evt_name.lower(), comp_name)
        return
    
