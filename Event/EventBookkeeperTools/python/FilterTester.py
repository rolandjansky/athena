# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##=============================================================================
## Name:        FilterTester
##
## Author:      David Cote (DESY)
## Created:     August 2009
##
## Description: A dummy filter to test CutFlowSvc
##=============================================================================

__doc__ = """This dummy filter provides a simple example.
"""

import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

## Import the module that allows to use named units, e.g. GeV
import AthenaCommon.SystemOfUnits as Units

class FilterTester( PyAthena.AthFilterAlgorithm ):

    def __init__ ( self, name = "FilterTester", **kw ):
        kw['name'] = name
        super(FilterTester, self).__init__(**kw)
        return

    def initialize(self):
        self.nProcessed         = 0

        ## Get the StoreGate service
        self.storeGateSvc = PyAthena.py_svc('StoreGateSvc')
        if self.storeGateSvc is None:
            self.msg.error("Problem retrieving StoreGateSvc pointer !!")
            return StatusCode.Failure

        cutFlow = PyAthena.py_svc('CutFlowSvc',iface='ICutFlowSvc')
        return StatusCode.Success


    def execute(self):
        self.nProcessed+=1        

        self.setFilterPassed(False)
        if self.nProcessed%2 == 0:
            self.setFilterPassed(True)

            
            
        return StatusCode.Success


    def finalize(self):
        self.msg.info( ' Number of processed events:  %r' % self.nProcessed )
        return StatusCode.Success

